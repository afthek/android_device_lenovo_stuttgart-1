# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:= $(call my-dir)

ifeq ($(BOARD_USE_WFD), true)

#
# libwfdvideoservice
#

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := eng
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	wfdcsc/csc_ARGB8888_to_YUV420SP_NEON.s \
	wfdcsc/csc_interleave_memcpy_neon.s \
	wfdcsc/csc_linear_to_tiled_crop_neon.s \
	wfdcsc/csc_linear_to_tiled_interleave_crop_neon.s \
	wfdcsc/csc_tiled_to_linear_crop_neon.s \
	wfdcsc/csc_tiled_to_linear_deinterleave_crop_neon.s \
	wfdcsc/wfdcsc.cpp \
	wfdcsc/wfdhwconverter.cpp \
	wfdcsc/wfdswconverter.c \
	IWfdVideoService.cpp \
	WfdVideoService.cpp \
	MessageQueue.cpp

LOCAL_C_INCLUDES := \

LOCAL_SHARED_LIBRARIES := \
	libbinder \
	libutils \
	libcutils \
	liblog \
	libfimc

LOCAL_C_INCLUDES += device/samsung/exynos4/include
LOCAL_C_INCLUDES += device/samsung/exynos4/libfimc

LOCAL_MODULE := libwfdvideoservice

include $(BUILD_SHARED_LIBRARY)

#
# libwfdvideoclient
#

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := eng
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	IWfdVideoService.cpp \
	MessageQueue.cpp \
	WfdVideoInputClient.cpp \
	WfdVideoControlClient.cpp \
	WfdVideoOverlayInput.cpp \
	WfdVideoFbInput.cpp \
	FboInputClient.cpp

LOCAL_C_INCLUDES := \

LOCAL_SHARED_LIBRARIES := \
	libbinder \
	libutils \
	libcutils \
	liblog \
	libui \
       libgui \
       libEGL \
       libGLESv1_CM \
       libGLESv2

LOCAL_MODULE := libwfdvideoclient

include $(BUILD_SHARED_LIBRARY)

endif

