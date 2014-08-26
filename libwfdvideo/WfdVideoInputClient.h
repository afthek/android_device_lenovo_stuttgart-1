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

#ifndef __WFDVIDEOINPUTCLIENT_H__
#define __WFDVIDEOINPUTCLIENT_H__

#include <stdint.h>
#include <sys/types.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>

#include "IWfdVideoService.h"

namespace android {

class WfdVideoInputClient
{
public:
    WfdVideoInputClient(int32_t source);
    WfdVideoInputClient();
    ~WfdVideoInputClient();

    void setSrcParams(uint32_t colorFormat, 
                      uint32_t width, 
                      uint32_t height,
                      uint32_t crop_x, 
                      uint32_t crop_y, 
                      uint32_t crop_w, 
                      uint32_t crop_h);

    void setRotate(uint32_t rotate);

    void setSrcBuffers(uint32_t phyYAddr, 
                       uint32_t phyCbAddr, 
                       uint32_t phyCrAddr);
    void write();
    void write(
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

private:
    int32_t checkStatus();
    sp<IWfdVideoService>  mService;
    int32_t               mWfdVideoSource;
    uint32_t              mSrcColorFormat;
    uint32_t              mSrcWidth, mSrcHeight;
    uint32_t              mSrcCropX, mSrcCropY, mSrcCropW, mSrcCropH;
    uint32_t              mSrcYAddr, mSrcCbAddr, mSrcCrAddr;
    uint32_t              mRotate;
};

};
#endif
