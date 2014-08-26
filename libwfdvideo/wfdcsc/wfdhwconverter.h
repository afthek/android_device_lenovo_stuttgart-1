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

#ifndef _WFD_HARDWARE_CONVERTER_H_

#define _WFD_HARDWARE_CONVERTER_H_

class HardwareConverter {
public:
    HardwareConverter();
    ~HardwareConverter();
    bool convert(
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
        uint32_t rotate);

    bool bHWconvert_flag;
private:
    void *mSecFimc;
};

#endif  // HARDWARE_CONVERTER_H_
