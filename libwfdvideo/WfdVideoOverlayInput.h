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

#ifndef __WFDVIDEOOVERLAYINPUT_H__
#define __WFDVIDEOOVERLAYINPUT_H__

#include <sys/types.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <utils/Log.h>

#include "MessageQueue.h"
#include "WfdVideoInputClient.h"

namespace android {

class WfdVideoOverlayInput : public WfdVideoInputClient
{
public :
    WfdVideoOverlayInput();
    ~WfdVideoOverlayInput();

    static WfdVideoOverlayInput *getInstance(void);
};

};
#endif
