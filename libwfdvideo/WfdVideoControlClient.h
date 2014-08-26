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

#ifndef __WFDVIDEOCONTROLCLIENT_H__
#define __WFDVIDEOCONTROLCLIENT_H__

#include <stdint.h>
#include <sys/types.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>

#include "IWfdVideoService.h"

namespace android {

class WfdVideoControlClient
{
public:
    WfdVideoControlClient();
    ~WfdVideoControlClient();

    static WfdVideoControlClient *getInstance();

    void start();
    void stop();
    int32_t setSource(int32_t source);

    int32_t read(
                uint32_t *dstColorFormat,
                uint32_t *dstWidth,
                uint32_t *dstHeight,
                uint32_t *dstStride,
                uint32_t *dstYAddr,
                uint32_t *dstCbAddr,
                uint32_t *dstCrAddr,
                uint32_t *dstBufIndex,
                int64_t *timestamp);

    void releaseBuffer(uint32_t dstBufIndex);

private:
    sp<IWfdVideoService>  mService;
};

};
#endif
