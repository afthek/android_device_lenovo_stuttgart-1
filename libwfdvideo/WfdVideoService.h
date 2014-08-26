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

#ifndef __WFDVIDEOSERVICE_H__
#define __WFDVIDEOSERVICE_H__

#include <stdint.h>
#include <sys/types.h>
#include <utils/RefBase.h>
#include <binder/IServiceManager.h>
#include <binder/IInterface.h>
#include <utils/Vector.h>

#include "MessageQueue.h"
#include "IWfdVideoService.h"

namespace android {

class WfdVideoService : public BnWfdVideoService
{
public :
    WfdVideoService();
    virtual ~WfdVideoService();
    static int32_t instantiate ();

    /* CSC related */
    int32_t   cscFlush();
    status_t  postMessage(
                  const sp<MessageBase>& message, 
                  nsecs_t relTime, 
                  uint32_t flags);
    int32_t   doConvert(
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
                  int64_t timeStamp);

    virtual void setSource(int32_t source);
    virtual int32_t getCurSource();
    virtual void start();
    virtual void stop();
    virtual void write(
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
                     int64_t timestamp);

    virtual int32_t read(
                        uint32_t *dstColorFormat,
                        uint32_t *dstWidth,
                        uint32_t *dstHeight,
                        uint32_t *dstStride,
                        uint32_t *dstYAddr,
                        uint32_t *dstCbAddr,
                        uint32_t *dstCrAddr,
                        uint32_t *dstBufIndex,
                        int64_t *timestamp);

    virtual void releaseBuffer(uint32_t dstBufIndex);

private:
    class CscFlushThread : public Thread 
    {
        WfdVideoService *mService;
    public:
        CscFlushThread(WfdVideoService *service):
            Thread(false),
            mService(service) { }
        virtual void onFirstRef() {
            run("CscFlushThread", PRIORITY_URGENT_DISPLAY);
        }
        virtual bool threadLoop() {
            mService->cscFlush();
            return false;
        }
    };

    int32_t   checkParams(
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
                  uint32_t rotate);

    void      getReservedBuffer(
                  uint32_t dstColorFormat, 
                  uint32_t dstWidth, 
                  uint32_t dstHeight,
                  uint32_t *dstYAddr, 
                  uint32_t *dstCbAddr, 
                  uint32_t *dstCrAddr);

    mutable Mutex             mLock;
    sp<CscFlushThread>        mCscFlushThread;
    mutable MessageQueue      mCscEventQueue;
    bool                      mExitCscFlushThread;
    void                      *mCscHandle;

    uint32_t             mSrcColorFormat;
    uint32_t             mSrcWidth, mSrcHeight;
    uint32_t             mSrcCropX, mSrcCropY, mSrcCropW, mSrcCropH;
    uint32_t             mSrcYAddr, mSrcCbAddr, mSrcCrAddr;
    uint32_t             mRotate;

    int32_t              mVideoSource;
    bool                 mEnable;

    struct addrs {
        uint32_t mType;
        uint32_t mYAddr;
        uint32_t mCbAddr;
        uint32_t mCrAddr;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mStride;
        uint32_t mColorFormat;
        uint32_t mBufIndex;
        int64_t  mTimeStamp;
    };

    uint32_t                 mBufIndex;
    Vector< struct addrs >   mFrames;
    struct addrs             mLastFrame;
    Condition                mCondition;

    /* Reserved buffer manage */
    int32_t                  mGrallocFd;
    uint32_t                 mGrallocPhyBaseAddr;
    uint32_t                 mGrallocVirBaseAddr;
    uint32_t                 mWfdPhyBaseAddr;
    uint32_t                 mWfdOffset;

    int64_t                  mCscCount;
    int64_t                  mCscSumInterval;
    int64_t                  mCscMaxInterval;
    int64_t                  mCscMinInterval;
};

};
#endif
