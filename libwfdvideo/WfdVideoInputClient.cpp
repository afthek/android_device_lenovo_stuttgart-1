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

#define LOG_TAG "WfdVideoInputClient"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <binder/IServiceManager.h>
#include <utils/RefBase.h>
#include <utils/Timers.h>
#include <utils/Log.h>

#include "WfdVideoInputClient.h"

namespace android {

WfdVideoInputClient::WfdVideoInputClient()
{
    mWfdVideoSource = WFDVIDEO_SOURCE_NONE;
    mService = NULL;

    sp<IBinder> binder;
    sp<IServiceManager> sm = defaultServiceManager();
    binder = sm->getService(String16("WfdVideoService"));
    if (binder == 0) {
        LOGE("WfdVideoService not published ...");
        return;
    }
    mService = interface_cast<IWfdVideoService>(binder);
}

WfdVideoInputClient::WfdVideoInputClient(int32_t source)
{
    mWfdVideoSource = source;
    if ((mWfdVideoSource < WFDVIDEO_SOURCE_NONE)
        || (mWfdVideoSource > WFDVIDEO_SOURCE_MAX)) {
        mWfdVideoSource = WFDVIDEO_SOURCE_NONE;
    }
    mService = NULL;

    sp<IBinder> binder;
    sp<IServiceManager> sm = defaultServiceManager();
    binder = sm->getService(String16("WfdVideoService"));
    if (binder == 0) {
        LOGE("WfdVideoService not published ...");
        return;
    }
    mService = interface_cast<IWfdVideoService>(binder);
}

WfdVideoInputClient::~WfdVideoInputClient()
{
}

void WfdVideoInputClient::setSrcParams(uint32_t colorFormat, 
                                       uint32_t width, 
                                       uint32_t height,
                                       uint32_t crop_x, 
                                       uint32_t crop_y, 
                                       uint32_t crop_w, 
                                       uint32_t crop_h)
{
    mSrcColorFormat = colorFormat;
    mSrcWidth = width;
    mSrcHeight = height;
    mSrcCropX = crop_x;
    mSrcCropY = crop_y;
    mSrcCropW = crop_w;
    mSrcCropH = crop_h;
}

void WfdVideoInputClient::setRotate(uint32_t rotate)
{
    mRotate = rotate;
}

void WfdVideoInputClient::setSrcBuffers(uint32_t phyYAddr, 
                                        uint32_t phyCbAddr, 
                                        uint32_t phyCrAddr)
{
    mSrcYAddr = phyYAddr;
    mSrcCbAddr = phyCbAddr;
    mSrcCrAddr = phyCrAddr;
}

int32_t WfdVideoInputClient::checkStatus()
{
    int32_t source = WFDVIDEO_SOURCE_NONE;
    if (mService != NULL) {
        source = mService->getCurSource();
    }
    if ((source > WFDVIDEO_SOURCE_NONE)
        && (source < WFDVIDEO_SOURCE_MAX)
        && (source == mWfdVideoSource)) {
        return 0;
    }

    return -1;
}

void WfdVideoInputClient::write()
{
    if ((checkStatus() == 0) && (mService != NULL)) {
        mService->write(
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
                      systemTime());
    }
}

void WfdVideoInputClient::write(
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
    if ((checkStatus() == 0) && (mService != NULL)) {
        mService->write(
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
                      systemTime());
    }
}

}
