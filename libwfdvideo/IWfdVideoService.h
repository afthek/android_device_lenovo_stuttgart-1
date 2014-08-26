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

#ifndef __IWFDVIDEOSERVICE_H__
#define __IWFDVIDEOSERVICE_H__

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {

enum {
    WFDVIDEOSERVICE_SET_SOURCE = IBinder::FIRST_CALL_TRANSACTION,
    WFDVIDEOSERVICE_GET_CURSOURCE,
    WFDVIDEOSERVICE_START,
    WFDVIDEOSERVICE_STOP,
    WFDVIDEOSERVICE_WRITE,
    WFDVIDEOSERVICE_READ,
    WFDVIDEOSERVICE_RELEASEBUFFER,
};

enum {
    WFDVIDEO_SOURCE_NONE,
    WFDVIDEO_SOURCE_FRAMEBUFFER,
    WFDVIDEO_SOURCE_OVERLAY,
    WFDVIDEO_SOURCE_FBO,
    WFDVIDEO_SOURCE_MAX,
};

class IWfdVideoService: public IInterface
{
    public:
        DECLARE_META_INTERFACE(WfdVideoService);

        virtual void setSource(int32_t source) = 0;
        virtual int32_t getCurSource() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;

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
                         int64_t timestamp) = 0;

        virtual int32_t read(
                            uint32_t *dstColorFormat,
                            uint32_t *dstWidth,
                            uint32_t *dstHeight,
                            uint32_t *dstStride,
                            uint32_t *dstYAddr,
                            uint32_t *dstCbAddr,
                            uint32_t *dstCrAddr,
                            uint32_t *dstBufIndex,
                            int64_t *timestamp) = 0;

        virtual void releaseBuffer(uint32_t dstBufIndex) = 0;
};

//-----------------------------------------------------------------------------

class BnWfdVideoService: public BnInterface<IWfdVideoService>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}

#endif

