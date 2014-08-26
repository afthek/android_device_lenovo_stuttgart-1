/*
 * Copyright (C) 2012 The Android Open Source Project
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

/*
 * @file    csc.h
 *
 * @brief   color space convertion abstract header
 *
 * @author  Pyoungjae Jung (pjet.jung@samsung.com)
 *
 * @version 1.0
 *
 * @history
 *   2011.12.27 : Create
 */

#ifndef _WFD_CSC_H__
#define _WFD_CSC_H__

typedef enum _CSC_ERRORCODE {
    CSC_ErrorNone = 0,
    CSC_Error,
    CSC_ErrorNotInit,
    CSC_ErrorInvalidAddress,
    CSC_ErrorUnsupportFormat,
    CSC_ErrorNotImplemented
} CSC_ERRORCODE;

typedef enum _CSC_METHOD {
    CSC_METHOD_SW = 0,
    CSC_METHOD_HW,
    CSC_METHOD_PREFER_HW
} CSC_METHOD;

void *csc_init(CSC_METHOD *method);
CSC_ERRORCODE csc_deinit(void *handle);
CSC_ERRORCODE csc_get_method(void *handle, CSC_METHOD *method);

CSC_ERRORCODE csc_set_src_format(
    void           *handle,
    unsigned int    width,
    unsigned int    height,
    unsigned int    crop_left,
    unsigned int    crop_top,
    unsigned int    crop_width,
    unsigned int    crop_height,
    unsigned int    color_format);

CSC_ERRORCODE csc_set_dst_format(
    void           *handle,
    unsigned int    width,
    unsigned int    height,
    unsigned int    crop_left,
    unsigned int    crop_top,
    unsigned int    crop_width,
    unsigned int    crop_height,
    unsigned int    color_format);

CSC_ERRORCODE csc_set_rotate(void *handle, int rotate);

CSC_ERRORCODE csc_set_src_buffer(
    void           *handle,
    unsigned char  *y,
    unsigned char  *u,
    unsigned char  *v);

CSC_ERRORCODE csc_set_dst_buffer(
    void           *handle,
    unsigned char  *y,
    unsigned char  *u,
    unsigned char  *v);

CSC_ERRORCODE csc_convert(void *handle);

#endif
