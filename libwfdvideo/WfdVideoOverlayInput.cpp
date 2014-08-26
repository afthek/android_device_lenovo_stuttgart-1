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

#define LOG_TAG "WfdVideoOverlayInput"

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

#include "WfdVideoOverlayInput.h"

namespace android {

static WfdVideoOverlayInput *g_WfdVideoOverlayInputSingleton;

WfdVideoOverlayInput *WfdVideoOverlayInput::getInstance(void)
{
    if (g_WfdVideoOverlayInputSingleton == NULL) {
        g_WfdVideoOverlayInputSingleton = new WfdVideoOverlayInput;
    }
    return g_WfdVideoOverlayInputSingleton;
}

WfdVideoOverlayInput::WfdVideoOverlayInput()
    : WfdVideoInputClient(WFDVIDEO_SOURCE_OVERLAY)
{
}

WfdVideoOverlayInput::~WfdVideoOverlayInput() 
{
}

}
