/*************************************************************************/ /*!
@File           imgpixfmts_km.h
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@License        Dual MIT/GPLv2

The contents of this file are subject to the MIT license as set out below.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Alternatively, the contents of this file may be used under the terms of
the GNU General Public License Version 2 ("GPL") in which case the provisions
of GPL are applicable instead of those above.

If you wish to allow use of your version of this file only under the terms of
GPL, and not to allow others to use your version of this file under the terms
of the MIT license, indicate your decision by deleting the provisions above
and replace them with the notice and other provisions required by GPL as set
out in the file called "GPL-COPYING" included in this distribution. If you do
not delete the provisions above, a recipient may use your version of this file
under the terms of either the MIT license or GPL.

This License is also included in this distribution in the file called
"MIT-COPYING".

EXCEPT AS OTHERWISE STATED IN A NEGOTIATED AGREEMENT: (A) THE SOFTWARE IS
PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT; AND (B) IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ /**************************************************************************/

/*****************************************************************************
 **
 ** WARNING: This file is autogenerated - DO NOT EDIT.
 **
 *****************************************************************************/

#if !defined(_IMGPIXFMTS_KM_H_)
#define _IMGPIXFMTS_KM_H_

#include "imgyuv.h"


typedef enum _IMG_PIXFMT_
{
	IMG_PIXFMT_UNKNOWN = 0,
	IMG_PIXFMT_R8G8B8A8_UNORM = 31,
	IMG_PIXFMT_R8G8B8X8_UNORM = 36,
	IMG_PIXFMT_R8G8_UNORM = 61,
	IMG_PIXFMT_R8_UNORM = 75,
	IMG_PIXFMT_B5G6R5_UNORM = 84,
	IMG_PIXFMT_B5G5R5A1_UNORM = 85,
	IMG_PIXFMT_B5G5R5X1_UNORM = 86,
	IMG_PIXFMT_B8G8R8A8_UNORM = 87,
	IMG_PIXFMT_B8G8R8X8_UNORM = 88,
	IMG_PIXFMT_B4G4R4A4_UNORM = 143,
	IMG_PIXFMT_UYVY = 169,
	IMG_PIXFMT_VYUY = 170,
	IMG_PIXFMT_YUYV = 171,
	IMG_PIXFMT_YVYU = 172,
	IMG_PIXFMT_YVU420_2PLANE = 173,
	IMG_PIXFMT_YUV420_2PLANE = 174,
	IMG_PIXFMT_YVU420_2PLANE_MACRO_BLOCK = 175,
	IMG_PIXFMT_YUV420_3PLANE = 176,
	IMG_PIXFMT_YVU420_3PLANE = 177,
	IMG_PIXFMT_V8U8Y8A8 = 182,
	IMG_PIXFMT_YVU8_420_2PLANE_PACK8_P = 243,
	IMG_PIXFMT_YUV8_420_2PLANE_PACK8_P = 247,
	IMG_PIXFMT_UYVY10_422_1PLANE_PACK10_CUST1 = 250,
	IMG_PIXFMT_R5G6B5_UNORM = 252,
}	IMG_PIXFMT;

#endif /* _IMGPIXFMTS_KM_H_ */