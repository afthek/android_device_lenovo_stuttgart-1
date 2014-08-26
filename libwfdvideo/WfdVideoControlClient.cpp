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

#define LOG_TAG "WfdVideoControlClient"

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
#include <utils/Log.h>

#include "WfdVideoControlClient.h"

namespace android {

static WfdVideoControlClient *g_WfdVideoControlSingleton;

WfdVideoControlClient *WfdVideoControlClient::getInstance(void)
{
    if (g_WfdVideoControlSingleton == NULL) {
        g_WfdVideoControlSingleton = new WfdVideoControlClient;
    }
    return g_WfdVideoControlSingleton;
}

WfdVideoControlClient::WfdVideoControlClient()
{
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

WfdVideoControlClient::~WfdVideoControlClient()
{
}

void WfdVideoControlClient::start()
{
    if (mService != NULL) {
        mService->start();
    }
}

void WfdVideoControlClient::stop()
{
    if (mService != NULL) {
        mService->stop();
    }
}

int32_t WfdVideoControlClient::setSource(int32_t source)
{
    if (mService != NULL) {
        mService->setSource(source);
    }
    return 0;
}

int32_t WfdVideoControlClient::read(
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
    if ((dstColorFormat == NULL)
        || (dstWidth == NULL)
        || (dstHeight == NULL)
        || (dstStride == NULL)
        || (dstYAddr == NULL)
        || (dstCbAddr == NULL)
        || (dstCrAddr == NULL)
        || (dstBufIndex == NULL)
        || (timestamp == NULL)) {
        return -1;
    }
    if (mService != NULL) {
        return mService->read(
                             dstColorFormat,
                             dstWidth,
                             dstHeight,
                             dstStride,
                             dstYAddr,
                             dstCbAddr,
                             dstCrAddr,
                             dstBufIndex,
                             timestamp);
    }

    return -1;
}

void WfdVideoControlClient::releaseBuffer(uint32_t dstBufIndex)
{
    if (mService != NULL) {
        mService->releaseBuffer(dstBufIndex);
    }
}

}
