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

#ifndef __FBOINPUTCLIENT_H__
#define __FBOINPUTCLIENT_H__

#include <stdint.h>
#include <sys/types.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <utils/Log.h>

#include <ui/GraphicBuffer.h>
#include <surfaceflinger/IGraphicBufferAlloc.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "MessageQueue.h"
#include "WfdVideoInputClient.h"

namespace android {

#define GL_TEXTURE_WIDTH					0x8D70
#define GL_TEXTURE_HEIGHT					0x8D71
#define GL_TEXTURE_FORMAT					0x8D72
#define GL_TEXTURE_ADDRESS				0x8D73
#define GL_TEXTURE_LENGTH					0x8D74

class FboInputClient : public WfdVideoInputClient
{
public:
	FboInputClient();
	~FboInputClient();
	static FboInputClient* getInstance(void);
	int32_t pushTexNativeBuf(uint32_t target);
	void freeGraphicBuffer(void);

	sp<GraphicBuffer> mGraphicBuffer;
	sp<IGraphicBufferAlloc> mGraphicBufferAlloc;
};

}
#endif
