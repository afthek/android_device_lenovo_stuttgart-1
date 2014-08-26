/*
**
** Copyright 2008, The Android Open Source Project
** Copyright 2010, Samsung Electronics Co. LTD
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/*
**
** @author  Zhigang Zhu(zhigang.zhu@samsung.com)
** @date    2012-03-07
*/

#define LOG_TAG "WfdVideoService"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/Log.h>

#include <media/stagefright/MetadataBufferType.h>

#include "wfdcsc/wfdcsc.h"

#include "sec_format.h"
#include "videodev2.h"
#include "videodev2_samsung.h"
#include "WfdVideoService.h"

namespace android {

#define WFD_ALIGN(value, base)        (((value) + ((base) - 1)) & ~((base) - 1))
#define WFD_FIMCBUFFER_DEV_NAME       "/dev/video3"

#define WFD_FIMCBUFFER_TOTAL_SIZE     (64 * 1024 * 1024)
#define WFD_FIMCBUFFER_START_OFFSET   (32 * 1024 * 1024)
#define WFD_FIMCBUFFER_RANGE_SIZE     (32 * 1024 * 1024)

class SecCscEventMsg : public MessageBase 
{
public:
    sp<WfdVideoService>    mService;
    uint32_t               mSrcColorFormat;
    uint32_t               mSrcWidth, mSrcHeight;
    uint32_t               mSrcCropX, mSrcCropY, mSrcCropW, mSrcCropH;
    uint32_t               mSrcYAddr, mSrcCbAddr, mSrcCrAddr;
    uint32_t               mRotate;
    int64_t                mTimeStamp;

    SecCscEventMsg(
        sp<WfdVideoService> service, 
        uint32_t srcColorFormat, 
        uint32_t srcWidth, 
        uint32_t srcHeight, 
        uint32_t srcCropX, 
        uint32_t srcCropY, 
        uint32_t srcCropW, 
        uint32_t srcCropH, 
        uint32_t srcYAddr, 
        uint32_t srcCbAddr, 
        uint32_t srcCrAddr,
        uint32_t rotate,
        int64_t timestamp)
            : mService(service), 
              mSrcColorFormat(srcColorFormat), 
              mSrcWidth(srcWidth), 
              mSrcHeight(srcHeight), 
              mSrcCropX(srcCropX), 
              mSrcCropY(srcCropY), 
              mSrcCropW(srcCropW), 
              mSrcCropH(srcCropH),
              mSrcYAddr(srcYAddr), 
              mSrcCbAddr(srcCbAddr), 
              mSrcCrAddr(srcCrAddr),
              mRotate(rotate),
              mTimeStamp(timestamp) { }

    virtual bool handler() 
    {
        mService->doConvert(
                      mSrcColorFormat, 
                      mSrcWidth, 
                      mSrcHeight, 
                      mSrcCropX, 
                      mSrcCropY, 
                      mSrcCropW, 
                      mSrcCropH, 
                      mSrcYAddr, 
                      mSrcCbAddr, 
                      mSrcCrAddr,
                      mRotate,
                      mTimeStamp);
        return true;
    }
};

int32_t WfdVideoService::instantiate()
{
    LOGD("WfdVideoService instantiate");
    int32_t ret = defaultServiceManager()->addService(String16("WfdVideoService"), new WfdVideoService);
    LOGD("Add WfdVideoService ret = %d", ret);

    return ret;
}

WfdVideoService::WfdVideoService()
    : mVideoSource(WFDVIDEO_SOURCE_NONE),
      mEnable(false),
      mSrcColorFormat(0),
      mSrcWidth(0), 
      mSrcHeight(0),
      mSrcCropX(0),
      mSrcCropY(0),
      mSrcCropW(0),
      mSrcCropH(0),
      mSrcYAddr(0), 
      mSrcCbAddr(0),
      mSrcCrAddr(0),
      mRotate(0),
      mExitCscFlushThread(false),
      mCscFlushThread(new CscFlushThread(this)),
      mCscHandle(NULL),
      mCscCount(0),
      mCscSumInterval(0),
      mCscMaxInterval(0),
      mCscMinInterval(30000)
{
    /* CSC handle */
    CSC_METHOD csc_method = CSC_METHOD_HW;
    mCscHandle = csc_init(&csc_method);

    /* reserved buffer */
    int ret;
    struct v4l2_control vc;
    mGrallocFd = open(WFD_FIMCBUFFER_DEV_NAME, O_RDWR);
    if (mGrallocFd < 0) {
        goto WFDSERVICE_ERROR;
    }

    vc.id = V4L2_CID_RESERVED_MEM_BASE_ADDR;
    vc.value = 0;
    ret = ioctl(mGrallocFd, VIDIOC_G_CTRL, &vc);
    if (ret < 0) {
        goto WFDSERVICE_ERROR;
    }
    mGrallocPhyBaseAddr = (uint32_t)vc.value;
    mWfdPhyBaseAddr = mGrallocPhyBaseAddr + WFD_FIMCBUFFER_START_OFFSET;
    //mfc NV12(NV21) encode, the address must be 2k aligned.
    mWfdOffset = WFD_ALIGN((mWfdPhyBaseAddr), 2048) - mWfdPhyBaseAddr;

    mGrallocVirBaseAddr = (uint32_t)mmap(0, 
                                         WFD_FIMCBUFFER_TOTAL_SIZE, 
                                         PROT_READ|PROT_WRITE, 
                                         MAP_SHARED, 
                                         mGrallocFd, 
                                         0);
    if ((void *)mGrallocVirBaseAddr == MAP_FAILED) {
        goto WFDSERVICE_ERROR;
    }

    LOGD(">>> WfdVideoService::WfdVideoService OK <<<");
    return;

WFDSERVICE_ERROR:
    LOGE("WfdVideoService error: fd=%d, phy addr=0x%08x, vir addr=0x%08x", 
        mGrallocFd, mGrallocPhyBaseAddr, mGrallocVirBaseAddr);
}

WfdVideoService::~WfdVideoService() 
{
    LOGD ("WfdVideoService destroyed");
    if (mCscFlushThread != NULL) {
        mCscFlushThread->requestExit();
        mExitCscFlushThread = true;
        mCscFlushThread->requestExitAndWait();
        mCscFlushThread.clear();
    }

    if (mCscHandle != NULL) {
        csc_deinit (mCscHandle);
        mCscHandle = NULL;
    }

    if (mGrallocFd >= 0) {
        munmap((void *)mGrallocVirBaseAddr, WFD_FIMCBUFFER_TOTAL_SIZE);
        close(mGrallocFd);
        mGrallocFd = -1;
    }
}

int32_t WfdVideoService::cscFlush()
{
    while (!mExitCscFlushThread) {
        nsecs_t timeout = -1;
        sp<MessageBase> msg = mCscEventQueue.waitMessage(timeout);
    }

    return 0;
}

status_t WfdVideoService::postMessage(
                              const sp<MessageBase>& message, 
                              nsecs_t relTime, 
                              uint32_t flags)
{
    mCscEventQueue.postMessage(message, relTime, flags);
    return 0;
}

void WfdVideoService::getReservedBuffer(
                          uint32_t dstColorFormat, 
                          uint32_t dstWidth, 
                          uint32_t dstHeight,
                          uint32_t *dstYAddr, 
                          uint32_t *dstCbAddr, 
                          uint32_t *dstCrAddr)
{
    Mutex::Autolock _l(mLock);

    uint32_t size;
    uint32_t uoffset, voffset;
    switch (dstColorFormat) {
        case HAL_PIXEL_FORMAT_YCbCr_420_P: {
            size = WFD_ALIGN((dstWidth * dstHeight), 4096);
            uoffset = size;
            size += WFD_ALIGN((WFD_ALIGN((dstWidth>>1), 16) * WFD_ALIGN((dstHeight>>1), 16)), 4096);
            voffset = size;
            size += WFD_ALIGN((WFD_ALIGN((dstWidth>>1), 16) * WFD_ALIGN((dstHeight>>1), 16)), 4096);
        } break;

        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
        case HAL_PIXEL_FORMAT_YCbCr_420_SP: {
            size = WFD_ALIGN((dstWidth * dstHeight), 4096);
            uoffset = size;
            voffset = size;
            size += WFD_ALIGN((WFD_ALIGN((dstWidth>>1), 16) * WFD_ALIGN((dstHeight>>1), 16)), 4096);
            size += WFD_ALIGN((WFD_ALIGN((dstWidth>>1), 16) * WFD_ALIGN((dstHeight>>1), 16)), 4096);
        } break;

        default: {
            size = WFD_ALIGN((WFD_ALIGN(dstWidth, 128) * WFD_ALIGN(dstHeight, 32)), 8192);
            uoffset = size;
            voffset = size;
            size += WFD_ALIGN((WFD_ALIGN(dstWidth, 128) * WFD_ALIGN((dstHeight>>1), 32)), 8192);
        }break;
    }

    if ((mWfdOffset + size) > WFD_FIMCBUFFER_RANGE_SIZE) {
        mWfdOffset = WFD_ALIGN((mWfdPhyBaseAddr), 2048) - mWfdPhyBaseAddr;
    }

    /* check buffer overide */
    uint32_t encoding_addr = mWfdPhyBaseAddr + mWfdOffset;
    uint32_t avail_addr = mWfdPhyBaseAddr + mWfdOffset;
    uint32_t avail_space = WFD_FIMCBUFFER_RANGE_SIZE;
    if (mFrames.size() > 0) {
        encoding_addr = mFrames[0].mYAddr;
        if (avail_addr >  encoding_addr) {
            avail_space = WFD_FIMCBUFFER_RANGE_SIZE - (avail_addr - encoding_addr);
        } else {
            avail_space = encoding_addr - avail_addr;
        }
    }

    if (avail_space <= size) {
        LOGE ("Not enough free buffer to do CSC ... ");
        *dstYAddr = NULL;
        *dstCbAddr = NULL;
        *dstCrAddr = NULL;
        return;
    }

    *dstYAddr = mWfdPhyBaseAddr + mWfdOffset;
    *dstCbAddr = *dstYAddr + uoffset;
    *dstCrAddr = *dstYAddr + voffset;
    mWfdOffset += size;
    mWfdOffset = WFD_ALIGN((mWfdPhyBaseAddr+mWfdOffset), 2048) - mWfdPhyBaseAddr;
}

int32_t WfdVideoService::doConvert(
                               uint32_t srcColorFormat, 
                               uint32_t srcWidth, 
                               uint32_t srcHeight, 
                               uint32_t srcCropX, 
                               uint32_t srcCropY, 
                               uint32_t srcCropW, 
                               uint32_t srcCropH, 
                               uint32_t srcYAddr, 
                               uint32_t srcCbAddr, 
                               uint32_t srcCrAddr,
                               uint32_t rotate,
                               int64_t timeStamp)
{
    struct timeval begin, end;
    uint32_t dstYAddr, dstCbAddr, dstCrAddr;
    uint32_t dstColorFormat, dstWidth, dstHeight;
    int32_t scaling = 0;
    CSC_METHOD csc_old_method, csc_new_method;

    if ((srcCropX != 0) || (srcCropY != 0) 
        || (WFD_ALIGN(srcCropW, 16) != WFD_ALIGN(srcWidth, 16))
        || (WFD_ALIGN(srcCropH, 16) != WFD_ALIGN(srcHeight, 16))) {
        LOGD ("notice: crop info: w(%d), h(%d), crop: [%d,%d], [%d,%d]",
          srcWidth, srcHeight, srcCropX, srcCropY, srcCropW, srcCropH);
    }

    gettimeofday(&begin, NULL);
    if (rotate != 0) {
        rotate = 360 - rotate;
    }

    dstColorFormat = HAL_PIXEL_FORMAT_YCbCr_420_SP;
    dstWidth = srcCropW;
    dstHeight = srcCropH;

    if ((rotate == 90) || (rotate == 270)) {
        uint32_t tmp = dstWidth;
        dstWidth = dstHeight;
        dstHeight = tmp;
    }

    if ((dstWidth > 1280) || (dstHeight > 720)) {
        if ((dstWidth / 1280.0) > (dstHeight > 720.0)) {
            dstHeight = dstHeight * 1280 / dstWidth;
            dstWidth = 1280;
        } else {
            dstWidth = dstWidth * 720 / dstHeight;
            dstHeight = 720;
        }
        dstWidth &= ~15;
        dstHeight &= ~15;
        scaling = 1;
    }

#if 0
    if ((rotate != 0) || (scaling == 1)) {
         csc_new_method = CSC_METHOD_HW;
    } else {
        switch (srcColorFormat) {
            case HAL_PIXEL_FORMAT_YCbCr_420_P:
            case HAL_PIXEL_FORMAT_YCbCr_420_SP:
            case HAL_PIXEL_FORMAT_CUSTOM_YCbCr_420_SP_TILED:
                csc_new_method = CSC_METHOD_HW;
                break;
            default:
                csc_new_method = CSC_METHOD_HW;
                break;
        }
    }
#else
    csc_new_method = CSC_METHOD_HW;
#endif

    csc_get_method(mCscHandle, &csc_old_method);
    if (csc_old_method != csc_new_method) {
        if (mCscHandle != NULL) {
            csc_deinit (mCscHandle);
        }
        mCscHandle = csc_init(&csc_new_method);
    }

    getReservedBuffer(dstColorFormat, 
                      WFD_ALIGN(dstWidth, 16), 
                      WFD_ALIGN(dstHeight, 16), 
                      &dstYAddr, 
                      &dstCbAddr, 
                      &dstCrAddr);
    if (dstYAddr == NULL) {
        return 0;
    }

    /* do fimc csc */
    uint8_t *srcAddr[3];
    uint8_t *dstAddr[3];

    if (csc_new_method == CSC_METHOD_HW) {
        srcAddr[0] = (uint8_t *)srcYAddr;
        srcAddr[1] = (uint8_t *)srcCbAddr;
        srcAddr[2] = (uint8_t *)srcCrAddr;
        dstAddr[0] = (uint8_t *)dstYAddr;
        dstAddr[1] = (uint8_t *)dstCbAddr;
        dstAddr[2] = (uint8_t *)dstCrAddr;
    } else {
        /* convert to virtual address */
        srcAddr[0] = (uint8_t *)(srcYAddr - mGrallocPhyBaseAddr + mGrallocVirBaseAddr);
        srcAddr[1] = (uint8_t *)(srcCbAddr - mGrallocPhyBaseAddr + mGrallocVirBaseAddr);
        srcAddr[2] = (uint8_t *)(srcCrAddr - mGrallocPhyBaseAddr + mGrallocVirBaseAddr);
        dstAddr[0] = (uint8_t *)(dstYAddr - mGrallocPhyBaseAddr + mGrallocVirBaseAddr);
        dstAddr[1] = (uint8_t *)(dstCbAddr - mGrallocPhyBaseAddr + mGrallocVirBaseAddr);
        dstAddr[2] = (uint8_t *)(dstCrAddr - mGrallocPhyBaseAddr + mGrallocVirBaseAddr);
    }

    csc_set_src_format(
        mCscHandle,                    /* handle */
        WFD_ALIGN(srcWidth, 16),       /* width */
        WFD_ALIGN(srcHeight, 16),      /* height */
        srcCropX,                      /* crop_left */
        srcCropY,                      /* crop_right */
        WFD_ALIGN(srcCropW, 16),       /* crop_width */
        WFD_ALIGN(srcCropH, 16),       /* crop_height */
        srcColorFormat);               /* color_format */

    csc_set_dst_format(
        mCscHandle,                     /* handle */
        WFD_ALIGN(dstWidth, 16),        /* width */
        WFD_ALIGN(dstHeight, 16),       /* height */
        0,                              /* crop_left */
        0,                              /* crop_right */
        WFD_ALIGN(dstWidth, 16),        /* crop_width */
        WFD_ALIGN(dstHeight, 16),       /* crop_height */
        dstColorFormat);                /* color_format */

    csc_set_rotate(mCscHandle, rotate);

    csc_set_src_buffer(
        mCscHandle,      /* handle */
        srcAddr[0],      /* y addr */
        srcAddr[1],      /* u addr or uv addr */
        srcAddr[2]);     /* v addr or none */

     csc_set_dst_buffer(
        mCscHandle,      /* handle */
        dstAddr[0], 
        dstAddr[1], 
        dstAddr[2]);

    csc_convert(mCscHandle);

    if (mEnable)
    {
        Mutex::Autolock _l(mLock);

        struct addrs frame;
        frame.mType = kMetadataBufferTypeCameraSource;
        frame.mYAddr = dstYAddr;
        frame.mCbAddr = dstCbAddr;
        frame.mCrAddr = dstCrAddr;
        frame.mWidth = dstWidth;
        frame.mHeight = dstHeight;
        frame.mStride = WFD_ALIGN(dstWidth, 16);
        frame.mColorFormat = dstColorFormat;
        frame.mBufIndex = mBufIndex++;
        frame.mTimeStamp = timeStamp;

        mLastFrame = frame;
        mFrames.add(frame);
        mCondition.signal();
    }

    gettimeofday(&end, NULL);
    mCscCount++;
    int64_t diffTime = ((end.tv_sec * 1000000) + end.tv_usec) - ((begin.tv_sec * 1000000) + begin.tv_usec);
    if (diffTime > mCscMaxInterval) {
        mCscMaxInterval = diffTime;
    }
    if (diffTime < mCscMinInterval) {
        mCscMinInterval = diffTime;
    }
    mCscSumInterval += diffTime;
    if (mCscCount == 100) {
        LOGI("max_interval = %lld, min_interval = %lld, avr_interval = %lld\n",
            mCscMaxInterval, mCscMinInterval, mCscSumInterval/mCscCount);
        mCscCount = 0;
        mCscSumInterval = 0;
    }

    if (0) {
        char filename[64];
        LOGD ("w x h = %d x %d ... ", dstWidth , dstHeight);
        sprintf (filename, "/data/csc%dx%d.yuv", dstWidth, dstHeight);
        FILE *fp = fopen (filename, "a+");
        if (fp != NULL) {
            fwrite ((uint8_t *)(mGrallocVirBaseAddr+dstYAddr-mGrallocPhyBaseAddr), 1, WFD_ALIGN(dstWidth, 16) * WFD_ALIGN(dstHeight, 16), fp);
            fwrite ((uint8_t *)(mGrallocVirBaseAddr+dstCbAddr-mGrallocPhyBaseAddr), 1, WFD_ALIGN(dstWidth, 16) * WFD_ALIGN(dstHeight, 16) / 4, fp);
            fwrite ((uint8_t *)(mGrallocVirBaseAddr+dstCrAddr-mGrallocPhyBaseAddr), 1, WFD_ALIGN(dstWidth, 16) * WFD_ALIGN(dstHeight, 16) / 4, fp);
            fclose (fp);
        } else {
            LOGE("CSC Service dump data, create /data/csc.yuv error");
        }
    }

    return 0;
}

void WfdVideoService::setSource(int32_t source)
{
    Mutex::Autolock _l(mLock);
    if ((source > WFDVIDEO_SOURCE_NONE) && 
        (source < WFDVIDEO_SOURCE_MAX)) {
        mVideoSource = source;
    }
}

int32_t WfdVideoService::getCurSource()
{
    Mutex::Autolock _l(mLock);

    return mVideoSource;
}

void WfdVideoService::start()
{
    Mutex::Autolock _l(mLock);

    if (mEnable) {
        return;
    }

    /* init buffers */
    mWfdOffset = 0;
    mWfdOffset = WFD_ALIGN((mWfdPhyBaseAddr+mWfdOffset), 2048) - mWfdPhyBaseAddr;

    mFrames.clear();
    mBufIndex = 0;

    mEnable = true;
}

void WfdVideoService::stop()
{
    Mutex::Autolock _l(mLock);

    if (!mEnable) {
        return;
    }

    mEnable = false;
    mCondition.signal();
}

int32_t WfdVideoService::checkParams(
                             uint32_t srcColorFormat, 
                             uint32_t srcWidth, 
                             uint32_t srcHeight,
                             uint32_t srcCropX, 
                             uint32_t srcCropY, 
                             uint32_t srcCropW, 
                             uint32_t srcCropH,
                             uint32_t srcYAddr,
                             uint32_t srcCbAddr,
                             uint32_t srcCrAddr,
                             uint32_t rotate)
{
    if (!mEnable) {
        LOGE ("WfdVideoService not started ... ");
        goto PARAMS_ERROR;
    }

    if ((srcWidth == 0) || (srcHeight == 0) || (srcCropW == 0) || (srcCropH == 0)) {
        goto PARAMS_ERROR;
    }

    if (srcYAddr == 0) {
        goto PARAMS_ERROR;
    }

    if ((rotate != 0) && (rotate != 90) && (rotate != 180) && (rotate != 270)) {
        goto PARAMS_ERROR;
    }

    return 0;

PARAMS_ERROR:
    LOGE ("WFD CSC Params error ... ");
    return -1;
}

void WfdVideoService::write(
                          uint32_t srcColorFormat, 
                          uint32_t srcWidth, 
                          uint32_t srcHeight,
                          uint32_t srcCropX, 
                          uint32_t srcCropY, 
                          uint32_t srcCropW, 
                          uint32_t srcCropH,
                          uint32_t srcYAddr,
                          uint32_t srcCbAddr,
                          uint32_t srcCrAddr,
                          uint32_t rotate,
                          int64_t timestamp)
{
    if (checkParams(
            srcColorFormat, 
            srcWidth, 
            srcHeight,
            srcCropX, 
            srcCropY, 
            srcCropW, 
            srcCropH,
            srcYAddr,
            srcCbAddr,
            srcCrAddr,
            rotate) == 0)
    {
        sp<MessageBase> msg;
        msg = new SecCscEventMsg(
                      this,
                      srcColorFormat, 
                      srcWidth, 
                      srcHeight,
                      srcCropX, 
                      srcCropY, 
                      srcCropW, 
                      srcCropH,
                      srcYAddr, 
                      srcCbAddr, 
                      srcCrAddr,
                      rotate,
                      timestamp);
        postMessage(msg, 0, 0);
    }
}

int32_t WfdVideoService::read(
                             uint32_t *dstColorFormat,
                             uint32_t *dstWidth,
                             uint32_t *dstHeight,
                             uint32_t *dstStride,
                             uint32_t *dstYAddr,
                             uint32_t *dstCbAddr,
                             uint32_t *dstCrAddr,
                             uint32_t *dstBufIndex,
                             int64_t *timestamp)
{
    Mutex::Autolock _l(mLock);
    while(mFrames.empty() && mEnable) {
        mCondition.wait(mLock);
    }

    if (mFrames.empty()) {
        return -1;
    }

    *dstColorFormat = mFrames[0].mColorFormat;
    *dstWidth = mFrames[0].mWidth;
    *dstHeight = mFrames[0].mHeight;
    *dstStride = mFrames[0].mStride;
    *dstYAddr = mFrames[0].mYAddr;
    *dstCbAddr = mFrames[0].mCbAddr;
    *dstCrAddr = mFrames[0].mCrAddr;
    *dstBufIndex = mFrames[0].mBufIndex;
    *timestamp = mFrames[0].mTimeStamp;

    return 0;
}

void WfdVideoService::releaseBuffer(uint32_t dstBufIndex)
{
    Mutex::Autolock _l(mLock);
    if (mEnable) {
        int32_t i = 0;
        int32_t size = mFrames.size();
        if (mFrames.empty()) {
            LOGE("WfdVideoService::releaseBuffer: no buffer ... ");
        } else {
            if (dstBufIndex == mFrames[0].mBufIndex) {
                mFrames.removeAt(0);
            } else {
                LOGE("releaseBuffer: cur idx (%d), release idx (%d) ... ",
                        mFrames[0].mBufIndex, dstBufIndex);
                while ((i < size) && (dstBufIndex > mFrames[i].mBufIndex)) i++;
                if (i > 0) {
                    mFrames.removeItemsAt(0, i);
                }
            }
        }
    } else {
        LOGE("WfdVideoService::releaseBuffer: already stopped ... ");
    }
}

}
