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
** @author  Junjie Wang(junjie.wang@samsung.com)
** @date    2012-03-16
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <cutils/properties.h>
#include <utils/RefBase.h>
#include <utils/Log.h>

#include <surfaceflinger/ISurfaceComposer.h>
#include <surfaceflinger/SurfaceComposerClient.h>
#include <ui/GraphicBufferMapper.h>
#include "FboInputClient.h"

namespace android {

#undef LOG_TAG
#define LOG_TAG "FboInputClient"

#define DBG_FBO 0


#if DBG_FBO
#define UE_PRINT              LOGE  //printf
#define UE_AppMalloc		malloc
#define UE_Free			free

typedef enum _RGB_COLOR_FORMAT_
{
	/* Basic types */
	DBG_PIXEL_FORMAT_UNKNOWN				=  0,
	DBG_PIXEL_FORMAT_RGB565				=  1,
	DBG_PIXEL_FORMAT_RGB555				=  2,
	DBG_PIXEL_FORMAT_RGB888				=  3,	/*!< 24bit */
	DBG_PIXEL_FORMAT_BGR888				=  4,	/*!< 24bit */
	DBG_PIXEL_FORMAT_GREY_SCALE			=  8,
	DBG_PIXEL_FORMAT_PAL12				= 13,
	DBG_PIXEL_FORMAT_PAL8				= 14,
	DBG_PIXEL_FORMAT_PAL4				= 15,
	DBG_PIXEL_FORMAT_PAL2				= 16,
	DBG_PIXEL_FORMAT_PAL1				= 17,
	DBG_PIXEL_FORMAT_ARGB1555			= 18,
	DBG_PIXEL_FORMAT_ARGB4444			= 19,
	DBG_PIXEL_FORMAT_ARGB8888			= 20,
	DBG_PIXEL_FORMAT_ABGR8888			= 21,
	DBG_PIXEL_FORMAT_YV12				= 22,
	DBG_PIXEL_FORMAT_I420				= 23,
	DBG_PIXEL_FORMAT_IMC2				= 25,
	DBG_PIXEL_FORMAT_XRGB8888			= 26,
	DBG_PIXEL_FORMAT_XBGR8888			= 27,
	DBG_PIXEL_FORMAT_BGRA8888			= 28,
	DBG_PIXEL_FORMAT_RGBA8888			= 29,
	DBG_PIXEL_FORMAT_XRGB4444			= 30,
	DBG_PIXEL_FORMAT_ARGB8332			= 31,
	DBG_PIXEL_FORMAT_A2RGB10				= 32,	/*!< 32bpp, 10 bits for R, G, B, 2 bits for A */
	DBG_PIXEL_FORMAT_A2BGR10				= 33,	/*!< 32bpp, 10 bits for B, G, R, 2 bits for A */
}RGB_COLOR_FORMAT;

/**
 *@brief	: change 24bpp RGB to 32bpp BGRx
 *@remarks	: 
 *@param	: 	[IN]		pInputBuf
 *				[IN]		width
 *				[IN]		height
 *				[OUT]	pOutputBuf
 *@return	: 	
 *@programmer	: 	wjj.SSCR, 091127
 *@revision	: 
 */
static void LHGP_GFX_Fit_32b_RGB(unsigned char* pInputBuf, int width, int height, unsigned char* pOutputBuf)
{
	int i;
	
	if(pInputBuf == NULL || pOutputBuf == NULL)
	{
		UE_PRINT("\n\033[0;42m Parameter pointer is NULL! \033[0m\n");
		return;
	}

	/*from 24bit to 32bit, RGB to BGRx*/
	for(i=0; i<width*height; i++)
	{
		pOutputBuf[i*4+3]=0xFF;
		pOutputBuf[i*4+2]=pInputBuf[i*3];
		pOutputBuf[i*4+1]=pInputBuf[i*3+1];
		pOutputBuf[i*4+0]=pInputBuf[i*3+2];
	}
	
}


/**
 *@brief	: Store 32bpp BGRx data from buffer to file
 *@remarks	: 32bpp Bmp file is BGRx color format
 *@param	: 	[OUT]	pchFileName
 *				[IN]		pui32Buff		--BGRx in buffer
 *				[IN]		ui16Width
 *				[IN]		ui16Height
 *@return	: 	0	OK
 *				-1	Error
 *@programmer	: 	wjj.SSCR, 091127
 *@revision	: 
 */
static int LHGP_GFX_DbgStore32RGBData2File(char * pchFileName,
                                                    unsigned char * pui32Buff,
                                                    unsigned short ui16Width,
                                                    unsigned short ui16Height)
{
	FILE* fp = NULL;
	unsigned char aui8Header[54];
	unsigned int ui32BufLen;
	unsigned int ui32FileLen;
	unsigned int* pui32Reverse;
	unsigned short ui16Loop;
	char achFullName[50] = {0, };

	if((NULL == pchFileName) || (NULL == pui32Buff) || (0 == ui16Width*ui16Height))
	{
		UE_PRINT("\n Store32RGBData2File args error \n");
		return -1;
	}

	/************************
	*	WRITE BMP HEADER
	*************************/
	memset(aui8Header, 0, 54);

	aui8Header[0] = 0x42;
	aui8Header[1] = 0x4D;

	/*file length*/
	ui32BufLen = ui16Height * ui16Width * sizeof(unsigned int);
	ui32FileLen = ui32BufLen + 54;

	aui8Header[2] = ui32FileLen & 0xFF;
	aui8Header[3] = (ui32FileLen >> 8) & 0xFF;
	aui8Header[4] = (ui32FileLen >> 16) & 0xFF;
	aui8Header[5] = (ui32FileLen >> 24) & 0xFF;

	/*bitmap data offset*/
	aui8Header[10] = 0x36; 

	/*bitmap header size*/
	aui8Header[14] = 0x28;

	/*width and height*/
	aui8Header[18] = ui16Width & 0xFF;
	aui8Header[19] = (ui16Width >> 8) & 0xFF;
	aui8Header[22] = ui16Height & 0xFF;
	aui8Header[23] = (ui16Height >> 8) & 0xFF;

	/*planes*/
	aui8Header[26] = 0x01;

	/*bits per pixel*/
	aui8Header[28] = 0x20;  

	strcpy(achFullName, pchFileName);
	strcat(achFullName, ".bmp");

	fp = fopen(achFullName, "wb");
	if(fp == NULL)
	{
		UE_PRINT("\n open bitmap file [%s] error!!! \n", achFullName);
		return -1;
	}

	fwrite(aui8Header, 54, 1, fp);


	/************************
	*	WRITE BMP DATA
	*************************/

	/*get the reverse buffer, bitmap scan is from the last line*/
	pui32Reverse = (unsigned int*)UE_AppMalloc(ui32BufLen);

	for(ui16Loop = 0; ui16Loop < ui16Height; ui16Loop++)
	{
		memcpy(&pui32Reverse[ui16Loop*ui16Width], 
		    &pui32Buff[(ui16Height-ui16Loop-1)*ui16Width* sizeof(unsigned int)], 
		    ui16Width * sizeof(unsigned int));
	}

	fwrite(pui32Reverse, ui32BufLen, 1, fp);

	UE_Free(pui32Reverse);
	fclose(fp);

	return 0;
}


/**
 *@brief	: Get 24bpp Bmp file information
 *@remarks	: 24bpp Bmp file is BGR color format
 *@param	: 	[IN]		pchFileName
 *				[OUT]	ui32FileLen
 *				[OUT]	pui16Width
 *				[OUT]	pui16Height
 *@return	: 	0	OK
 *				-1	Error
 *@programmer	: 	wjj.SSCR, 091127
 *@revision	: 
 */
static int LHGP_GFX_DbgGet24BmpFileInfo(char * pchFileName,
							   unsigned int * 	ui32FileLen,
                                                    unsigned short * pui16Width,
                                                    unsigned short * pui16Height)
{
	FILE* fp = NULL;
	unsigned char aui8Header[54];
	char achFullName[50] = {0, };

	*ui32FileLen = 0;
	*pui16Width = 0;
	*pui16Height = 0;

	if((NULL == pchFileName) || (NULL == ui32FileLen) || (NULL == pui16Width) || (NULL == pui16Height))
	{
		UE_PRINT("\n GetImgSizeFrom24BmpFile args error \n");
		return -1;
	}

	strcpy(achFullName, pchFileName);
	strcat(achFullName, ".bmp");

	fp = fopen(achFullName, "rb");
	if(fp == NULL)
	{
		UE_PRINT("\n open bitmap file [%s] error!!! \n", achFullName);
		return -1;
	}

	fread(aui8Header, 54, 1, fp);

	/*check bmp header*/
	if(aui8Header[0] != 0x42
		|| aui8Header[1] != 0x4D
		|| aui8Header[10] != 0x36
		|| aui8Header[14] != 0x28
		|| aui8Header[26] != 0x01
		|| aui8Header[28] != 0x18)
	{
		UE_PRINT("\n check bmp header error \n");
		fclose(fp);
		return -1;
	}

	/*get width and height*/
	*ui32FileLen = (aui8Header[5]<<24) + (aui8Header[4]<<16) + (aui8Header[3]<<8) + aui8Header[2];
	*pui16Width = (aui8Header[19]<<8) + aui8Header[18];
	*pui16Height = (aui8Header[23]<<8) + aui8Header[22];

	fclose(fp);
	return 0;
}


/************************************
 *		24bpp BMP FILE FORMAT
 *
 *	54 bytes bmp header
 *	BGR BGR BGR ... BGR STUFF  //width, 4 bytes align
 *	BGR BGR BGR ... BGR STUFF  //width, 4 bytes align
 *	BGR BGR BGR ... BGR STUFF  //width, 4 bytes align
 *	BGR BGR BGR ... BGR STUFF  //width, 4 bytes align
 *
*************************************/

/**
 *@brief	: Get 32bpp BGRx data from 24bpp Bmp file
 *@remarks	: 24bpp Bmp file is BGR color format
 *@param	: 	[IN]		pchFileName
 *				[OUT]	pui32Buff		--BGRx in buffer
 *@return	: 	0	OK
 *				-1	Error
 *@programmer	: 	wjj.SSCR, 091127
 *@revision	: 
 */
static int LHGP_GFX_DbgGet32RGBDataFrom24BmpFile(char * pchFileName, unsigned char * pui32Buff)
{
	FILE* fp = NULL;
	unsigned int ui32BufLen;
	unsigned int ui32FileLen;
	unsigned int ui32Start, ui32BytesOneLine;
	unsigned short i, j;
	unsigned short ui16Width, ui16Height;
	char * pch24Data = NULL;
	char * pch32Data = NULL;
	char * pch32Data_temp = NULL;
	char achFullName[50] = {0, };

	if((NULL == pchFileName) || (NULL == pui32Buff))
	{
		UE_PRINT("\n Get32RGBDataFrom24BmpFile args error \n");
		return -1;
	}

	if(0 != LHGP_GFX_DbgGet24BmpFileInfo(pchFileName, &ui32FileLen, &ui16Width, &ui16Height))
	{
		return -1;
	}

	strcpy(achFullName, pchFileName);
	strcat(achFullName, ".bmp");

	fp = fopen(achFullName, "rb");
	if(fp == NULL)
	{
		UE_PRINT("\n open bitmap file [%s] error!!! \n", achFullName);
		return -1;
	}

	if(0 != fseek(fp, 54, 0))		//set file pointer to bmp data position
	{
		UE_PRINT("\n fseek error \n");
		fclose(fp);
		return -1;
	}

	/************************
	*	GET BMP DATA
	*************************/
	
	/*Input buffer*/
	ui32FileLen -= 54;
	ui32BytesOneLine = 3*ui16Width;
	while( ui32BytesOneLine % 4 != 0 )	//  4 bytes align
     		++ui32BytesOneLine;

	#if 0
	/*check file len*/
	if(ui32FileLen != ui32BytesOneLine*ui16Height)
	{
		UE_PRINT("\n bitmap file lenth error \n");
		fclose(fp);
		return -1;
	}
	#endif
	
	pch24Data = (char *)UE_AppMalloc(ui32FileLen);
	fread(pch24Data, ui32FileLen, 1, fp);

	/*Output buffer*/
	/*get the reverse buffer, bitmap scan is from the last line*/
	ui32BufLen = ui16Width*ui16Height*sizeof(unsigned int);
	pch32Data = (char *)UE_AppMalloc(ui32BufLen);
	pch32Data_temp = pch32Data;

	/*get 32bpp BGRx data*/
	for(i = 0; i < ui16Height; i++)
	{
		ui32Start = (ui16Height-i-1)*ui32BytesOneLine;
		for(j=0; j<ui16Width; j++)
		{
			*pch32Data_temp++ = pch24Data[ui32Start+3*j];		//B
			*pch32Data_temp++ = pch24Data[ui32Start+3*j+1];	//G
			*pch32Data_temp++ = pch24Data[ui32Start+3*j+2];	//R
			*pch32Data_temp++ = (char)0xff;					//x
		}
	}

	memcpy(pui32Buff, pch32Data, ui32BufLen);

	UE_Free(pch24Data);
	UE_Free(pch32Data);
	fclose(fp);

	return 0;
}



/**
 *@brief	: Store BGR data from buffer to BMP file (32bpp->32bpp BMP, 16bpp->16bpp BMP)
 *@remarks	: Bmp file is BGR(x) color format
 *@param	: 	[OUT]	pchFileName
 *				[IN]		pui32Buff		--BGR(x) in buffer
 *				[IN]		ui16Width
 *				[IN]		ui16Height
 *				[IN]		format
 *@return	: 	0	OK
 *				-1	Error
 *@programmer	: 	wjj.SSCR, 110824
 *@revision	: 
 */
static int DBG_StoreRGBData_to_BMP(char * pchFileName,
                                                    unsigned char * pui32Buff,
                                                    unsigned short ui16Width,
                                                    unsigned short ui16Height,
                                                    RGB_COLOR_FORMAT format
                                                    )
{
	FILE* fp = NULL;
	unsigned int bpp;
	unsigned int stride, ui32Start;
	unsigned int ui32BufLen;
	unsigned int ui32FileLen;
	unsigned int bSeq = 0;
	unsigned char* pui8Reverse;
	unsigned char* pch32Data_temp;
	unsigned short ui16Loop, j;
	unsigned char aui8Header[54];
	char achFullName[50] = {0, };

	if((NULL == pchFileName) || (NULL == pui32Buff) || (0 == ui16Width*ui16Height))
	{
		UE_PRINT("\n StoreRGBData_to_32BMP(): args error \n");
		return -1;
	}

	if(format != DBG_PIXEL_FORMAT_RGB565 &&
		format != DBG_PIXEL_FORMAT_BGRA8888 &&
		format != DBG_PIXEL_FORMAT_RGBA8888)
	{
		UE_PRINT("\n StoreRGBData_to_32BMP(): format error \n");
		return -1;
	}

	/************************
	*	WRITE BMP HEADER
	*************************/
	memset(aui8Header, 0, 54);

	aui8Header[0] = 0x42;	//'B'
	aui8Header[1] = 0x4D;	//'M'

	/*file length*/
	switch(format)
	{
		case DBG_PIXEL_FORMAT_RGB565:	//DBG_PIXEL_FORMAT_BGR565
			bpp = 16;
			bSeq = 0;
			break;
		case DBG_PIXEL_FORMAT_BGRA8888:
			bpp = 32;
			bSeq = 0;
			break;
		case DBG_PIXEL_FORMAT_RGBA8888:
			bpp = 32;
			bSeq = 1;
			break;
		default:
			bpp = 32;
			bSeq = 0;
			break;
	}
	stride = ui16Width * bpp / 8;
	while(stride % 4 != 0)		//  4 bytes align in a line stride
		++stride;
	ui32BufLen = ui16Height * stride;
	ui32FileLen = ui32BufLen + 54;

	aui8Header[2] = ui32FileLen & 0xFF;
	aui8Header[3] = (ui32FileLen >> 8) & 0xFF;
	aui8Header[4] = (ui32FileLen >> 16) & 0xFF;
	aui8Header[5] = (ui32FileLen >> 24) & 0xFF;

	/*bitmap data offset*/
	aui8Header[10] = 0x36; 

	/*bitmap header size*/
	aui8Header[14] = 0x28;

	/*width and height*/
	aui8Header[18] = ui16Width & 0xFF;
	aui8Header[19] = (ui16Width >> 8) & 0xFF;
	aui8Header[22] = ui16Height & 0xFF;
	aui8Header[23] = (ui16Height >> 8) & 0xFF;

	/*planes*/
	aui8Header[26] = 0x01;

	/*bits per pixel*/
	aui8Header[28] = bpp;  

	strcpy(achFullName, pchFileName);
	strcat(achFullName, ".bmp");

	fp = fopen(achFullName, "wb");
	if(fp == NULL)
	{
		UE_PRINT("\n open bitmap file [%s] error!!! \n", achFullName);
		return -1;
	}

	fwrite(aui8Header, 54, 1, fp);


	/************************
	*	WRITE BMP DATA
	*************************/

	/*get the reverse buffer, bitmap scan is from the last line*/
	pui8Reverse = (unsigned char*)UE_AppMalloc(ui32BufLen);
	pch32Data_temp = pui8Reverse;
	memset(pui8Reverse, 0, ui32BufLen);

	for(ui16Loop = 0; ui16Loop < ui16Height; ui16Loop++)
	{
		if(bSeq == 0)
		{
			memcpy(&pui8Reverse[ui16Loop*stride], 
			    &pui32Buff[(ui16Height-ui16Loop-1)*ui16Width*bpp/8], 
			    ui16Width*bpp/8);
		}
		else if(format == DBG_PIXEL_FORMAT_RGBA8888)
		{
			ui32Start = (ui16Height-ui16Loop-1)*ui16Width*bpp/8;
			for(j=0; j<ui16Width; j++)
			{
				*pch32Data_temp++ = pui32Buff[ui32Start+j*bpp/8+2];		//B
				*pch32Data_temp++ = pui32Buff[ui32Start+j*bpp/8+1];		//G
				*pch32Data_temp++ = pui32Buff[ui32Start+j*bpp/8+0];		//R
				*pch32Data_temp++ = pui32Buff[ui32Start+j*bpp/8+3];		//A
			}
		}
	}

	fwrite(pui8Reverse, ui32BufLen, 1, fp);

	UE_Free(pui8Reverse);
	fclose(fp);

	return 0;
}

#endif


static FboInputClient* g_FboInputClientSingleton;

FboInputClient *FboInputClient::getInstance(void)
{
    if (g_FboInputClientSingleton == NULL) {
        g_FboInputClientSingleton = new FboInputClient;
    }
    return g_FboInputClientSingleton;
}

FboInputClient::FboInputClient()
        : WfdVideoInputClient(WFDVIDEO_SOURCE_FBO)
{
    sp<ISurfaceComposer> composer(ComposerService::getComposerService());
    mGraphicBufferAlloc = composer->createGraphicBufferAlloc();
}

FboInputClient::~FboInputClient()
{
    freeGraphicBuffer();
}

int32_t FboInputClient::pushTexNativeBuf(uint32_t target)
{
    int32_t width, height, format, usage, paddr, length;
    void* virGfxBufAddr = NULL;
    void* phyGfxBufAddr[3]; //according to gralloc_module.cpp::gralloc_getphys()
    GraphicBufferMapper& mapper(GraphicBufferMapper::get());
    int32_t err = 0;

    glGetTexParameteriv(target, GL_TEXTURE_WIDTH, &width);
    glGetTexParameteriv(target, GL_TEXTURE_HEIGHT, &height);
    glGetTexParameteriv(target, GL_TEXTURE_FORMAT, &format);
    glGetTexParameteriv(target, GL_TEXTURE_ADDRESS, &paddr);
    glGetTexParameteriv(target, GL_TEXTURE_LENGTH, &length);
    usage = GRALLOC_USAGE_SW_READ_NEVER | GRALLOC_USAGE_SW_WRITE_OFTEN
            | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP
            | GRALLOC_USAGE_HW_FIMC1;

    LOGD("~~~~~~ pushTexNativeBuf(): width=%d, height=%d, format=%d, paddr=0x%x, length=%d, usage=0x%x ~~~~~~",
        width, height, format, paddr, length, usage); //wjj test

    if ((mGraphicBuffer == NULL) ||
            (int32_t(mGraphicBuffer->width)  != width) ||
            (int32_t(mGraphicBuffer->height) != height) ||
            (int32_t(mGraphicBuffer->format) != format) ||
            ((int32_t(mGraphicBuffer->usage) & usage) != usage))
    {
        status_t error;
        sp<GraphicBuffer> graphicBuffer(
                mGraphicBufferAlloc->createGraphicBuffer(
                        width, height, format, usage, &error));
        if (graphicBuffer == 0) 
        {
            err = -1;
            goto FAIL_RETURN_1;
        }
        mGraphicBuffer = graphicBuffer;
        LOGD("~~~~~~ pushTexNativeBuf(): create GraphicBuffer OK. width=%d, height=%d, format=%d, stride=%d, usage=0x%x ~~~~~~",
            mGraphicBuffer->width, mGraphicBuffer->height, mGraphicBuffer->format, mGraphicBuffer->stride, mGraphicBuffer->usage); //wjj test
    }

    if((mGraphicBuffer->lock(GRALLOC_USAGE_SW_WRITE_OFTEN, (void**)&virGfxBufAddr)) < 0)
    {
        err = -1;
        goto FAIL_RETURN_2;
    }

    if (mapper.getphys(mGraphicBuffer->handle, phyGfxBufAddr) != 0) {
        err = -1;
        goto FAIL_RETURN_2;
    }

    LOGD("~~~~~~ pushTexNativeBuf(): phyGfxBufAddr=0x%x, virGfxBufAddr=0x%x ~~~~~~",
        phyGfxBufAddr[0], virGfxBufAddr); //wjj test

    /*copy texture data to GraphicBuffer*/
    memcpy(virGfxBufAddr, (void *)paddr, length);

    /*push GraphicBuffer's texture data into wifi device*/
    write(format, width, height, 0, 0, width, height, (uint32_t)phyGfxBufAddr[0], 0, 0, 0);

    

    /* for test */
#if DBG_FBO
    {
        static int i = 1;
        char pchFileName[20] = {0, };
        RGB_COLOR_FORMAT bmp_fmt;
        if(i<=10)
        {
            sprintf(pchFileName, "/data/%d", i);
            switch(format)
            {
                case HAL_PIXEL_FORMAT_RGBA_8888:
                case HAL_PIXEL_FORMAT_RGBX_8888:
                    bmp_fmt = DBG_PIXEL_FORMAT_RGBA8888;
                    break;
                case HAL_PIXEL_FORMAT_BGRA_8888:
                    bmp_fmt = DBG_PIXEL_FORMAT_BGRA8888;
                    break;
                case HAL_PIXEL_FORMAT_RGB_565:
                    bmp_fmt = DBG_PIXEL_FORMAT_RGB565;
                    break;
                default:
                    bmp_fmt = DBG_PIXEL_FORMAT_UNKNOWN;
                    break;
            }
            DBG_StoreRGBData_to_BMP(pchFileName, (unsigned char *)virGfxBufAddr, width, height, bmp_fmt);
            LOGD("~~~~~~ StoreRGBData_to_BMP(): %s ~~~~~~", pchFileName); //wjj test
        }
        i++;
    }
#endif

    return err;

FAIL_RETURN_2:
    mGraphicBuffer = 0;
FAIL_RETURN_1:
    return err;
}

void FboInputClient::freeGraphicBuffer()
{
    mGraphicBuffer = 0;
}

}

