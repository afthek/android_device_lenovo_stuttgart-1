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
** @date    2012-02-03
*/

#define LOG_TAG "WfdVideoFbInput"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <cutils/properties.h>
#include <utils/RefBase.h>
#include <utils/Log.h>

#include "WfdVideoFbInput.h"

#define WFDVIDEO_DEBUG

#ifdef WFDVIDEO_DEBUG
#include "WfdVideoControlClient.h"
#endif

namespace android {

#ifdef WFDVIDEO_DEBUG
#define WFDVIDEOSERVICE_SOURCE_PROP "wfdvideo.source"
#define WFDVIDEOSERVICE_CONTROL_PROP "wfdvideo.control"

class WfdVideoDebug
{
public :
    WfdVideoDebug();
    ~WfdVideoDebug();

    int32_t   WfdVideoDebugThreadWrapper();

private:
    class WfdVideoDebugThread : public Thread 
    {
        WfdVideoDebug *mWfdVideoDebug;
    public:
        WfdVideoDebugThread(WfdVideoDebug *obj):
            Thread(false),
            mWfdVideoDebug(obj) { }
        virtual void onFirstRef() {
            run("WfdVideoDebugThread", PRIORITY_NORMAL);
        }
        virtual bool threadLoop() {
            mWfdVideoDebug->WfdVideoDebugThreadWrapper();
            return false;
        }
    };

    sp<WfdVideoDebugThread>     mThread;
    bool                     mExitThread;
};

WfdVideoDebug::WfdVideoDebug()
{
    mExitThread = false;
    mThread = new WfdVideoDebugThread(this);
}

WfdVideoDebug::~WfdVideoDebug() 
{
    if (mThread != NULL) {
        mThread->requestExit();
        mExitThread = true;
        mThread->requestExitAndWait();
        mThread.clear();
    }
}

int32_t WfdVideoDebug::WfdVideoDebugThreadWrapper()
{
    while (!mExitThread) {
        WfdVideoControlClient *p = WfdVideoControlClient::getInstance();
        if (p != NULL) {
            uint32_t dstColorFormat;
            uint32_t dstWidth;
            uint32_t dstHeight;
            uint32_t dstStride;
            uint32_t dstYAddr;
            uint32_t dstCbAddr;
            uint32_t dstCrAddr;
            uint32_t dstBufIndex;
            int64_t  timestamp;
            int32_t ret = p->read(
                              &dstColorFormat, 
                              &dstWidth, 
                              &dstHeight,
                              &dstStride, 
                              &dstYAddr,
                              &dstCbAddr,
                              &dstCrAddr,
                              &dstBufIndex,
                              &timestamp);
            if (ret == 0) {
                p->releaseBuffer(dstBufIndex);
                LOGD ("release Buffer: YAddr(0x%08x), width (%d), height (%d),  index (%d)", 
                        dstYAddr, dstWidth, dstHeight, dstBufIndex);
            }
        }
    }

    return 0;
}

static WfdVideoDebug *g_WfdVideoDebug;
#endif

static WfdVideoFbInput *g_WfdVideoFbInputSingleton;

WfdVideoFbInput *WfdVideoFbInput::getInstance(void)
{
    if (g_WfdVideoFbInputSingleton == NULL) {
        g_WfdVideoFbInputSingleton = new WfdVideoFbInput;
    }
    return g_WfdVideoFbInputSingleton;
}

WfdVideoFbInput::WfdVideoFbInput()
    : WfdVideoInputClient(WFDVIDEO_SOURCE_FRAMEBUFFER)
{
    mExitThread = false;
    mThread = new WfdVideoFbThread(this);
}

WfdVideoFbInput::~WfdVideoFbInput() 
{
    if (mThread != NULL) {
        mThread->requestExit();
        mExitThread = true;
        mThread->requestExitAndWait();
        mThread.clear();
    }
}

int32_t WfdVideoFbInput::WfdVideoFbThreadWrapper()
{
    char prop[64];
    uint32_t phyAddr;
    while (!mExitThread) {
        nsecs_t timeout = 40000000; // 40ms
        sp<MessageBase> msg = mEventQueue.waitMessage(timeout);

#ifdef WFDVIDEO_DEBUG
        if (property_get(WFDVIDEOSERVICE_SOURCE_PROP, prop, NULL) != 0) {
            WfdVideoControlClient *p = WfdVideoControlClient::getInstance();
            if (strcmp(prop, "framebuffer") == 0) {
                if (p != NULL) {
                    p->setSource(WFDVIDEO_SOURCE_FRAMEBUFFER);
                }
                property_set(WFDVIDEOSERVICE_SOURCE_PROP, "none");
                LOGD("WfdVideoSerivce: set source FRAMEBUFFER ... ");
            } else if (strcmp(prop, "overlay") == 0) {
                if (p != NULL) {
                    p->setSource(WFDVIDEO_SOURCE_OVERLAY);
                }
                property_set(WFDVIDEOSERVICE_SOURCE_PROP, "none");
                LOGD("WfdVideoSerivce: set source OVERLAY ... ");
            } else if (strcmp(prop, "fbo") == 0) {
                if (p != NULL) {
                    p->setSource(WFDVIDEO_SOURCE_FBO);
                }
                property_set(WFDVIDEOSERVICE_SOURCE_PROP, "none");
                LOGD("WfdVideoSerivce: set source FBO ... ");
            }
        }

        if (property_get(WFDVIDEOSERVICE_CONTROL_PROP, prop, NULL) != 0) {
            WfdVideoControlClient *p = WfdVideoControlClient::getInstance();
            if (strcmp(prop, "start") == 0) {
                if (p != NULL) {
                    p->start();
                }
                property_set(WFDVIDEOSERVICE_CONTROL_PROP, "none");
                LOGD("WfdVideoSerivce: start ... ");
            } else if (strcmp(prop, "stop") == 0) {
                if (p != NULL) {
                    p->stop();
                }
                property_set(WFDVIDEOSERVICE_CONTROL_PROP, "none");
                LOGD("WfdVideoSerivce: stop ... ");
            } else if (strcmp(prop, "read") == 0) {
                if (g_WfdVideoDebug == NULL) {
                    g_WfdVideoDebug = new WfdVideoDebug;
                }
                property_set(WFDVIDEOSERVICE_CONTROL_PROP, "none");
                LOGD("WfdVideoSerivce: read ... ");
            }
        }
#endif

        {
            Mutex::Autolock _l(mLock);
            if (mFbAddrs.size() != 0) {
                phyAddr = mFbAddrs[0];
                mFbAddrs.removeAt(0);
                mLastFbAddr = 0;
                setRotate(mRotate);
            } else {
                phyAddr = mLastFbAddr;
            }
        }
        //if (mLastFbAddr != phyAddr) {  //for test
            setSrcBuffers(phyAddr, 0, 0);
            write();
        //}
        mLastFbAddr = phyAddr;
    }

    return 0;
}

void WfdVideoFbInput::setFbAddr(uint32_t addr)
{
    Mutex::Autolock _l(mLock);

    if (mFbAddrs.size() != 0) {
        mFbAddrs.clear();
    }

    mFbAddrs.add(addr);
}

void WfdVideoFbInput::setFbRotate(int32_t rotate)
{
    Mutex::Autolock _l(mLock);
    mRotate = rotate;
}

}
