/*************************************************************************/ /*!
@File
@Title          RGX build options
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

/* Each build option listed here is packed into a dword which
 * provides up to (32 - log2(RGX_BUILD_OPTIONS_MASK_KM + 1)) flags.
 * The corresponding bit is set if the build option 
 * was enabled at compile time.
 *
 * In order to extract the enabled build flags the INTERNAL_TEST
 * switch should be enabled in a client program which includes this
 * header. Then the client can test specific build flags by reading
 * the bit value at ##OPTIONNAME##_SET_OFFSET in RGX_BUILD_OPTIONS.
 *
 * IMPORTANT: add new options to unused bits or define a new dword
 * (e.g. RGX_BUILD_OPTIONS2) so that the bitfield remains backwards
 * compatible.
 */

#include "rgx_options_km.h"

#if defined(SUPPORT_PERCONTEXT_FREELIST) || defined (INTERNAL_TEST)
	#define OPTIONS_BIT31		(0x1ul << 31)
	#if OPTIONS_BIT31 <= RGX_BUILD_OPTIONS_MASK_KM
	#error "Bit exceeds reserved range"
	#endif
	#define SUPPORT_PERCONTEXT_FREELIST_SET_OFFSET	OPTIONS_BIT31
#else
	#define OPTIONS_BIT31		0x0
#endif /* SUPPORT_PERCONTEXT_FREELIST */

#define _KM_RGX_BUILD_OPTIONS_ RGX_BUILD_OPTIONS

#define RGX_BUILD_OPTIONS	\
	RGX_BUILD_OPTIONS_KM |\
	OPTIONS_BIT31

