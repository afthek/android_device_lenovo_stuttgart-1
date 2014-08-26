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

#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <utils/Log.h>

#include "IWfdVideoService.h"

namespace android {

class BpWfdVideoService: public BpInterface<IWfdVideoService>
{
public:
    BpWfdVideoService(const sp<IBinder>& impl) 
        : BpInterface<IWfdVideoService>(impl) {}

    virtual void setSource(int32_t source)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IWfdVideoService::getInterfaceDescriptor());
        data.writeInt32(source);
        remote()->transact(WFDVIDEOSERVICE_SET_SOURCE, data, &reply);
    }

    virtual int32_t getCurSource()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IWfdVideoService::getInterfaceDescriptor());
        remote()->transact(WFDVIDEOSERVICE_GET_CURSOURCE, data, &reply);
        return reply.readInt32();
    }

    virtual void start()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IWfdVideoService::getInterfaceDescriptor());
        remote()->transact(WFDVIDEOSERVICE_START, data, &reply);
    }

    virtual void stop()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IWfdVideoService::getInterfaceDescriptor());
        remote()->transact(WFDVIDEOSERVICE_STOP, data, &reply);
    }

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
                     int64_t timestamp)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IWfdVideoService::getInterfaceDescriptor());
        data.writeInt32(srcColorFormat);
        data.writeInt32(srcWidth);
        data.writeInt32(srcHeight);
        data.writeInt32(srcCropX);
        data.writeInt32(srcCropY);
        data.writeInt32(srcCropW);
        data.writeInt32(srcCropH);
        data.writeInt32(srcYAddr);
        data.writeInt32(srcCbAddr);
        data.writeInt32(srcCrAddr);
        data.writeInt32(rotate);
        data.writeInt64(timestamp);
        remote()->transact(WFDVIDEOSERVICE_WRITE, data, &reply);
    }

    virtual int32_t read(
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
        Parcel data, reply;
        data.writeInterfaceToken(IWfdVideoService::getInterfaceDescriptor());
        remote()->transact(WFDVIDEOSERVICE_READ, data, &reply);
        *dstColorFormat = reply.readInt32();
        *dstWidth = reply.readInt32();
        *dstHeight = reply.readInt32();
        *dstStride = reply.readInt32();
        *dstYAddr = reply.readInt32();
        *dstCbAddr = reply.readInt32();
        *dstCrAddr = reply.readInt32();
        *dstBufIndex = reply.readInt32();
        *timestamp = reply.readInt64();
        return reply.readInt32();
    }

    virtual void releaseBuffer(uint32_t dstBufIndex)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IWfdVideoService::getInterfaceDescriptor());
        data.writeInt32(dstBufIndex);
        remote()->transact(WFDVIDEOSERVICE_RELEASEBUFFER, data, &reply);
    }

};

IMPLEMENT_META_INTERFACE(WfdVideoService, "android.os.IWfdVideoService");

//-----------------------------------------------------------------------------

status_t BnWfdVideoService::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        case WFDVIDEOSERVICE_SET_SOURCE: {
            CHECK_INTERFACE(IWfdVideoService, data, reply);
            int32_t source = data.readInt32();
            setSource(source);
        } break;
        case WFDVIDEOSERVICE_GET_CURSOURCE: {
            CHECK_INTERFACE(IWfdVideoService, data, reply);
            int32_t source = getCurSource();
            reply->writeInt32(source);
        } break;
        case WFDVIDEOSERVICE_START: {
            CHECK_INTERFACE(IWfdVideoService, data, reply);
            start();
        } break;
        case WFDVIDEOSERVICE_STOP: {
            CHECK_INTERFACE(IWfdVideoService, data, reply);
            stop();
        } break;
        case WFDVIDEOSERVICE_WRITE: {
            CHECK_INTERFACE(IWfdVideoService, data, reply);
            uint32_t srcColorFormat = data.readInt32();
            uint32_t srcWidth = data.readInt32();
            uint32_t srcHeight = data.readInt32();
            uint32_t srcCropX = data.readInt32();
            uint32_t srcCropY = data.readInt32();
            uint32_t srcCropW = data.readInt32();
            uint32_t srcCropH = data.readInt32();
            uint32_t srcYAddr = data.readInt32();
            uint32_t srcCbAddr = data.readInt32();
            uint32_t srcCrAddr = data.readInt32();
            uint32_t rotate = data.readInt32();
            int64_t  timestamp = data.readInt64();
            write(
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
                timestamp);
        } break;
        case WFDVIDEOSERVICE_READ: {
            CHECK_INTERFACE(IWfdVideoService, data, reply);
            uint32_t dstColorFormat;
            uint32_t dstWidth;
            uint32_t dstHeight;
            uint32_t dstStride;
            uint32_t dstYAddr;
            uint32_t dstCbAddr;
            uint32_t dstCrAddr;
            uint32_t dstBufIndex;
            int64_t  timestamp;
            int32_t ret = read(
                              &dstColorFormat, 
                              &dstWidth, 
                              &dstHeight,
                              &dstStride, 
                              &dstYAddr,
                              &dstCbAddr,
                              &dstCrAddr,
                              &dstBufIndex,
                              &timestamp);
            reply->writeInt32(dstColorFormat);
            reply->writeInt32(dstWidth);
            reply->writeInt32(dstHeight);
            reply->writeInt32(dstStride);
            reply->writeInt32(dstYAddr);
            reply->writeInt32(dstCbAddr);
            reply->writeInt32(dstCrAddr);
            reply->writeInt32(dstBufIndex);
            reply->writeInt64(timestamp);
            reply->writeInt32(ret);
        } break;
        case WFDVIDEOSERVICE_RELEASEBUFFER: {
            CHECK_INTERFACE(IWfdVideoService, data, reply);
            uint32_t bufIndex = data.readInt32();
            releaseBuffer(bufIndex);
        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }

    return NO_ERROR;
}

};

