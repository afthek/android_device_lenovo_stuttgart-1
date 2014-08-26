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

#ifndef __WFDVIDEOFBINPUT_H__
#define __WFDVIDEOFBINPUT_H__

#include <sys/types.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <utils/Log.h>

#include "MessageQueue.h"
#include "WfdVideoInputClient.h"

namespace android {

class WfdVideoFbInput : public WfdVideoInputClient
{
public :
    WfdVideoFbInput();
    ~WfdVideoFbInput();

    static WfdVideoFbInput *getInstance(void);

    int32_t   WfdVideoFbThreadWrapper();
    void      setFbAddr(uint32_t addr);
    void      setFbRotate(int32_t rotate);

private:
    class WfdVideoFbThread : public Thread 
    {
        WfdVideoFbInput *mWfdVideoFbInput;
    public:
        WfdVideoFbThread(WfdVideoFbInput *obj):
            Thread(false),
            mWfdVideoFbInput(obj) { }
        virtual void onFirstRef() {
            run("WfdVideoFbThread", PRIORITY_NORMAL);
        }
        virtual bool threadLoop() {
            mWfdVideoFbInput->WfdVideoFbThreadWrapper();
            return false;
        }
    };

    mutable Mutex            mLock;
    sp<WfdVideoFbThread>     mThread;
    mutable MessageQueue     mEventQueue;
    bool                     mExitThread;

    Vector< uint32_t >       mFbAddrs;
    uint32_t                 mLastFbAddr;
    int32_t                  mRotate;
};

};
#endif
