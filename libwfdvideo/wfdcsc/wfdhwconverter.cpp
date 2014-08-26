/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <utils/Log.h>
#include "SecFimc.h"
#include "wfdhwconverter.h"

HardwareConverter::HardwareConverter()
{
    SecFimc* handle_fimc = new SecFimc();
    mSecFimc = (void *)handle_fimc;

    if (handle_fimc->create(SecFimc::DEV_2, SecFimc::MODE_MULTI_BUF, 1) == false)
    {
        LOGE("... create fimc csc failed ....");
        bHWconvert_flag = 0;
    }
    else
    {
        LOGE("... create fimc csc successfully ....");
        bHWconvert_flag = 1;
    }
}

HardwareConverter::~HardwareConverter()
{
    SecFimc* handle_fimc = (SecFimc*)mSecFimc;
    handle_fimc->destroy();
    delete mSecFimc;
}

bool HardwareConverter::convert(
        void ** src_addr,
        void ** dst_addr,
        uint32_t src_format,
        uint32_t src_width,
        uint32_t src_height,
        uint32_t src_crop_x,
        uint32_t src_crop_y,
        uint32_t src_crop_w,
        uint32_t src_crop_h,
        uint32_t dst_format,
        uint32_t dst_width,
        uint32_t dst_height,
        uint32_t rotate)
{
    SecFimc* handle_fimc = (SecFimc*)mSecFimc;

    int rotate_value = rotate;

    uint32_t dst_crop_x = 0;
    uint32_t dst_crop_y = 0;
    uint32_t dst_crop_width = dst_width;
    uint32_t dst_crop_height = dst_height;

    void **src_addr_array = src_addr;
    void **dst_addr_array = dst_addr;

    // set post processor configuration
    if (!handle_fimc->setSrcParams(src_width, src_height, src_crop_x, src_crop_y,
                                   &src_crop_w, &src_crop_h,
                                   src_format)) {
        LOGE("%s:: setSrcParms() failed", __func__);
        return false;
    }

    if (!handle_fimc->setSrcAddr((unsigned int)src_addr_array[0],
                                 (unsigned int)src_addr_array[1],
                                 (unsigned int)src_addr_array[2],
                                 src_format)) {
        LOGE("%s:: setSrcPhyAddr() failed", __func__);
        return false;
    }

    if (!handle_fimc->setRotVal(rotate_value)) {
        LOGE("%s:: setRotVal() failed", __func__);
        return false;
    }

    if (!handle_fimc->setDstParams(dst_width, dst_height, dst_crop_x, dst_crop_y,
                                   &dst_crop_width, &dst_crop_height,
                                   dst_format)) {
        LOGE("%s:: setDstParams() failed", __func__);
        return false;
    }

    if (!handle_fimc->setDstAddr((unsigned int)(dst_addr_array[0]),
                                 (unsigned int)(dst_addr_array[1]),
                                 (unsigned int)(dst_addr_array[2]))) {
        LOGE("%s:: setDstPhyAddr() failed", __func__);
        return false;
    }

    if (!handle_fimc->draw(0, 0)) {
        LOGE("%s:: handleOneShot() failed", __func__);
        return false;
    }

    return true;
}

