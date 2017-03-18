/*************************************************************************/ /*!
@File
@Title          Services Firmware image utilities used at init time
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    Services Firmware image utilities used at init time
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

#include "rgxfwimageutils.h"

#if defined(RGX_FEATURE_MIPS)
#include "pvr_debug.h"
#endif


/************************************************************************
* FW Segments configuration
************************************************************************/
typedef struct _RGX_FW_SEGMENT_
{
	IMG_UINT32 ui32SegId;        /*!< Segment Id */
	IMG_UINT32 ui32SegStartAddr; /*!< Segment Start Addr */
	IMG_UINT32 ui32SegAllocSize; /*!< Amount of memory to allocate for that segment */
	IMG_UINT32 ui32FWMemOffset;  /*!< Offset of this segment in the collated FW mem allocation */
	IMG_CHAR   *pszSegName;
} RGX_FW_SEGMENT;


#if defined(RGX_FEATURE_META)
static RGX_FW_SEGMENT asRGXFWCodeSegments[] = {
/* Seg ID                 Seg Start Addr           Alloc size   FWMem offset  Name */
{RGXFW_SEGMMU_BOOTLDR_ID, RGXFW_BOOTLDR_META_ADDR, 0x1000,      0,            "Bootldr"}, /* Has to be the first one to get the proper DevV addr */
{RGXFW_SEGMMU_TEXT_ID,    0x18880000,              0x30000,     0x1000,       "Local Text"},
};
static RGX_FW_SEGMENT asRGXFWDataSegments[] = {
/* Seg ID                 Seg Start Addr           Alloc size   FWMem offset  Name */
{RGXFW_SEGMMU_SHARED_ID,  0x38880000,              0x5000,      0,            "Local Shared"},
{RGXFW_SEGMMU_DATA_ID,    0x78880000,              0x12000,     0x5000,       "Local Data"},
};
#else
static RGX_FW_SEGMENT asRGXFWCodeSegments[] = {
/* Seg ID                 Seg Start Addr           Alloc size             FWMem offset           Name */
{    0,                   0xC0000000,          RGXMIPSFW_CODE_SIZE,         0x0,         "Text and static data"},
};
static RGX_FW_SEGMENT asRGXFWDataSegments[] = {
/* Seg ID                 Seg Start Addr           Alloc size             FWMem offset           Name */
{    0,                   0xC0000000,          RGXMIPSFW_PAGE_SIZE,          0x0,              "Stack"},
};
#endif

#define RGXFW_NUM_CODE_SEGMENTS  (sizeof(asRGXFWCodeSegments)/sizeof(asRGXFWCodeSegments[0]))
#define RGXFW_NUM_DATA_SEGMENTS  (sizeof(asRGXFWDataSegments)/sizeof(asRGXFWDataSegments[0]))


#if defined(RGX_FEATURE_META)
/*!
*******************************************************************************

 @Function      RGXFWConfigureSegID

 @Description   Configures a single segment of the Segment MMU
                (base, limit and out_addr)

 @Input         hPrivate        : Implementation specific data
 @Input         ui64SegOutAddr  : Segment output base address (40 bit devVaddr)
 @Input         ui32SegBase     : Segment input base address (32 bit FW address)
 @Input         ui32SegLimit    : Segment size
 @Input         ui32SegID       : Segment ID
 @Input         pszName         : Segment name
 @Input         ppui32BootConf  : Pointer to bootloader data

 @Return        void

******************************************************************************/
static void RGXFWConfigureSegID(const void *hPrivate,
                                IMG_UINT64 ui64SegOutAddr,
                                IMG_UINT32 ui32SegBase,
                                IMG_UINT32 ui32SegLimit,
                                IMG_UINT32 ui32SegID,
                                IMG_CHAR* pszName,
                                IMG_UINT32 **ppui32BootConf)
{
	IMG_UINT32 *pui32BootConf = *ppui32BootConf;
	IMG_UINT32 ui32SegOutAddr0  = ui64SegOutAddr & 0x00000000FFFFFFFFUL;
	IMG_UINT32 ui32SegOutAddr1  = (ui64SegOutAddr >> 32) & 0x00000000FFFFFFFFUL;

	/* META segments have a minimum size */
	IMG_UINT32 ui32LimitOff = (ui32SegLimit < RGXFW_SEGMMU_ALIGN) ?
	                          RGXFW_SEGMMU_ALIGN : ui32SegLimit;
	/* the limit is an offset, therefore off = size - 1 */
	ui32LimitOff -= 1;

	RGXCommentLogInit(hPrivate,
	                  "* FW %s - seg%d: meta_addr = 0x%08x, devv_addr = 0x%llx, limit = 0x%x",
	                  pszName, ui32SegID,
	                  ui32SegBase, (unsigned long long)ui64SegOutAddr,
	                  ui32LimitOff);

	ui32SegBase |= RGXFW_SEGMMU_ALLTHRS_WRITEABLE;

	*pui32BootConf++ = META_CR_MMCU_SEGMENTn_BASE(ui32SegID);
	*pui32BootConf++ = ui32SegBase;

	*pui32BootConf++ = META_CR_MMCU_SEGMENTn_LIMIT(ui32SegID);
	*pui32BootConf++ = ui32LimitOff;

	*pui32BootConf++ = META_CR_MMCU_SEGMENTn_OUTA0(ui32SegID);
	*pui32BootConf++ = ui32SegOutAddr0;

	*pui32BootConf++ = META_CR_MMCU_SEGMENTn_OUTA1(ui32SegID);
	*pui32BootConf++ = ui32SegOutAddr1;

	*ppui32BootConf = pui32BootConf;
}

/*!
*******************************************************************************

 @Function      RGXFWConfigureSegMMU

 @Description   Configures META's Segment MMU

 @Input         hPrivate             : Implementation specific data
 @Input         psFWCodeDevVAddrBase : FW code base device virtual address
 @Input         psFWDataDevVAddrBase : FW data base device virtual address
 @Input         ppui32BootConf       : Pointer to bootloader data

 @Return        void

******************************************************************************/
static void RGXFWConfigureSegMMU(const void       *hPrivate,
                                 IMG_DEV_VIRTADDR *psFWCodeDevVAddrBase,
                                 IMG_DEV_VIRTADDR *psFWDataDevVAddrBase,
                                 IMG_UINT32       **ppui32BootConf)
{
	IMG_UINT64 ui64SegOutAddr;
	IMG_UINT32 i;

	/* Configure Segment MMU */
	RGXCommentLogInit(hPrivate, "********** FW configure Segment MMU **********");

	/* Set-up the Segment MMU except for the bootloader segment, (skip it) */
	for (i = 1; i < RGXFW_NUM_CODE_SEGMENTS; i++)
	{
		ui64SegOutAddr = (psFWCodeDevVAddrBase->uiAddr |
		                  RGXFW_SEGMMU_OUTADDR_TOP(0, RGXFW_SEGMMU_META_DM_ID)) +
		                  asRGXFWCodeSegments[i].ui32FWMemOffset;

		RGXFWConfigureSegID(hPrivate,
		                    ui64SegOutAddr,
		                    asRGXFWCodeSegments[i].ui32SegStartAddr,
		                    asRGXFWCodeSegments[i].ui32SegAllocSize,
		                    asRGXFWCodeSegments[i].ui32SegId,
		                    asRGXFWCodeSegments[i].pszSegName,
		                    ppui32BootConf); /*write the sequence to the bootldr */
	}

	for (i = 0; i < RGXFW_NUM_DATA_SEGMENTS; i++)
	{
		ui64SegOutAddr = (psFWDataDevVAddrBase->uiAddr |
		                  RGXFW_SEGMMU_OUTADDR_TOP(0, RGXFW_SEGMMU_META_DM_ID)) +
		                  asRGXFWDataSegments[i].ui32FWMemOffset;

		RGXFWConfigureSegID(hPrivate,
		                    ui64SegOutAddr,
		                    asRGXFWDataSegments[i].ui32SegStartAddr,
		                    asRGXFWDataSegments[i].ui32SegAllocSize,
		                    asRGXFWDataSegments[i].ui32SegId,
		                    asRGXFWDataSegments[i].pszSegName,
		                    ppui32BootConf); /*write the sequence to the bootldr */
	}
}

/*!
*******************************************************************************

 @Function      FindMMUSegment

 @Description   Given a 32 bit FW address attempt to find the corresponding
                pointer to FW allocation

 @Input         ui32OffsetIn      : 32 bit FW address
 @Input         pvHostFWCodeAddr  : Pointer to FW code
 @Input         pvHostFWDataAddr  : Pointer to FW data
 @Input         uiHostAddrOut     : CPU pointer equivalent to ui32OffsetIn

 @Return        PVRSRV_ERROR

******************************************************************************/
static PVRSRV_ERROR FindMMUSegment(IMG_UINT32 ui32OffsetIn,
                                   void *pvHostFWCodeAddr,
                                   void *pvHostFWDataAddr,
                                   void **uiHostAddrOut)
{
	RGX_FW_SEGMENT *psSegArr;
	IMG_UINT32 i;

	psSegArr = asRGXFWCodeSegments;
	for (i = 0; i < RGXFW_NUM_CODE_SEGMENTS; i++)
	{
		if ((ui32OffsetIn >= psSegArr[i].ui32SegStartAddr) &&
		    (ui32OffsetIn < (psSegArr[i].ui32SegStartAddr + psSegArr[i].ui32SegAllocSize)))
		{
			*uiHostAddrOut = pvHostFWCodeAddr;
			goto found;
		}
	}

	psSegArr = asRGXFWDataSegments;
	for (i = 0; i < RGXFW_NUM_DATA_SEGMENTS; i++)
	{
		if ((ui32OffsetIn >= psSegArr[i].ui32SegStartAddr) &&
		   (ui32OffsetIn < (psSegArr[i].ui32SegStartAddr + psSegArr[i].ui32SegAllocSize)))
		{
			*uiHostAddrOut = pvHostFWDataAddr;
			goto found;
		}
	}

	return PVRSRV_ERROR_INIT_FAILURE;

found:
	/* Direct Mem write to mapped memory */
	ui32OffsetIn -= psSegArr[i].ui32SegStartAddr;
	ui32OffsetIn += psSegArr[i].ui32FWMemOffset;

	/* Add offset to pointer to FW allocation only if
	 * that allocation is available
	 */
	if (*uiHostAddrOut)
	{
		*(IMG_UINT8 **)uiHostAddrOut += ui32OffsetIn;
	}

	return PVRSRV_OK;
}

/*!
*******************************************************************************

 @Function      RGXFWConfigureMetaCaches

 @Description   Configure and enable the Meta instruction and data caches

 @Input         hPrivate          : Implementation specific data
 @Input         ui32NumThreads    : Number of FW threads in use
 @Input         ui32MainThreadID  : ID of the FW thread in use
                                    (only meaningful if ui32NumThreads == 1)
 @Input         ppui32BootConf    : Pointer to bootloader data

 @Return        void

******************************************************************************/
static void RGXFWConfigureMetaCaches(const void *hPrivate,
                                     IMG_UINT32 ui32NumThreads,
                                     IMG_UINT32 ui32MainThreadID,
                                     IMG_UINT32 **ppui32BootConf)
{
	IMG_UINT32 *pui32BootConf = *ppui32BootConf;
	IMG_UINT32 ui32DCacheT0, ui32ICacheT0;
	IMG_UINT32 ui32DCacheT1, ui32ICacheT1;
	IMG_UINT32 ui32DCacheT2, ui32ICacheT2;
	IMG_UINT32 ui32DCacheT3, ui32ICacheT3;

#define META_CR_MMCU_LOCAL_EBCTRL                        (0x04830600)
#define META_CR_MMCU_LOCAL_EBCTRL_ICWIN                  (0x3 << 14)
#define META_CR_MMCU_LOCAL_EBCTRL_DCWIN                  (0x3 << 6)
#define META_CR_SYSC_DCPART(n)                           (0x04830200 + (n)*0x8)
#define META_CR_SYSC_DCPARTX_CACHED_WRITE_ENABLE         (0x1 << 31)
#define META_CR_SYSC_ICPART(n)                           (0x04830220 + (n)*0x8)
#define META_CR_SYSC_XCPARTX_LOCAL_ADDR_OFFSET_TOP_HALF  (0x8 << 16)
#define META_CR_SYSC_XCPARTX_LOCAL_ADDR_FULL_CACHE       (0xF)
#define META_CR_SYSC_XCPARTX_LOCAL_ADDR_HALF_CACHE       (0x7)
#define META_CR_MMCU_DCACHE_CTRL                         (0x04830018)
#define META_CR_MMCU_ICACHE_CTRL                         (0x04830020)
#define META_CR_MMCU_XCACHE_CTRL_CACHE_HITS_EN           (0x1)

	RGXCommentLogInit(hPrivate, "********** Meta caches configuration *********");

	/* Initialise I/Dcache settings */
	ui32DCacheT0 = ui32DCacheT1 = META_CR_SYSC_DCPARTX_CACHED_WRITE_ENABLE;
	ui32DCacheT2 = ui32DCacheT3 = META_CR_SYSC_DCPARTX_CACHED_WRITE_ENABLE;
	ui32ICacheT0 = ui32ICacheT1 = ui32ICacheT2 = ui32ICacheT3 = 0;

	if (ui32NumThreads == 1)
	{
		if (ui32MainThreadID == 0)
		{
			ui32DCacheT0 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_FULL_CACHE;
			ui32ICacheT0 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_FULL_CACHE;
		}
		else
		{
			ui32DCacheT1 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_FULL_CACHE;
			ui32ICacheT1 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_FULL_CACHE;
		}
	}
	else
	{
		ui32DCacheT0 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_HALF_CACHE;
		ui32ICacheT0 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_HALF_CACHE;

		ui32DCacheT1 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_HALF_CACHE |
		                META_CR_SYSC_XCPARTX_LOCAL_ADDR_OFFSET_TOP_HALF;
		ui32ICacheT1 |= META_CR_SYSC_XCPARTX_LOCAL_ADDR_HALF_CACHE |
		                META_CR_SYSC_XCPARTX_LOCAL_ADDR_OFFSET_TOP_HALF;
	}

	/* Local region MMU enhanced bypass: WIN-3 mode for code and data caches */
	*pui32BootConf++ = META_CR_MMCU_LOCAL_EBCTRL;
	*pui32BootConf++ = META_CR_MMCU_LOCAL_EBCTRL_ICWIN |
	                   META_CR_MMCU_LOCAL_EBCTRL_DCWIN;

	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_MMCU_LOCAL_EBCTRL,
	                  META_CR_MMCU_LOCAL_EBCTRL_ICWIN | META_CR_MMCU_LOCAL_EBCTRL_DCWIN);

	/* Data cache partitioning thread 0 to 3 */
	*pui32BootConf++ = META_CR_SYSC_DCPART(0);
	*pui32BootConf++ = ui32DCacheT0;
	*pui32BootConf++ = META_CR_SYSC_DCPART(1);
	*pui32BootConf++ = ui32DCacheT1;
	*pui32BootConf++ = META_CR_SYSC_DCPART(2);
	*pui32BootConf++ = ui32DCacheT2;
	*pui32BootConf++ = META_CR_SYSC_DCPART(3);
	*pui32BootConf++ = ui32DCacheT3;

	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_DCPART(0), ui32DCacheT0);
	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_DCPART(1), ui32DCacheT1);
	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_DCPART(2), ui32DCacheT2);
	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_DCPART(3), ui32DCacheT3);

	/* Enable data cache hits */
	*pui32BootConf++ = META_CR_MMCU_DCACHE_CTRL;
	*pui32BootConf++ = META_CR_MMCU_XCACHE_CTRL_CACHE_HITS_EN;

	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_MMCU_DCACHE_CTRL,
	                  META_CR_MMCU_XCACHE_CTRL_CACHE_HITS_EN);

	/* Instruction cache partitioning thread 0 to 3 */
	*pui32BootConf++ = META_CR_SYSC_ICPART(0);
	*pui32BootConf++ = ui32ICacheT0;
	*pui32BootConf++ = META_CR_SYSC_ICPART(1);
	*pui32BootConf++ = ui32ICacheT1;
	*pui32BootConf++ = META_CR_SYSC_ICPART(2);
	*pui32BootConf++ = ui32ICacheT2;
	*pui32BootConf++ = META_CR_SYSC_ICPART(3);
	*pui32BootConf++ = ui32ICacheT3;

	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_ICPART(0), ui32ICacheT0);
	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_ICPART(1), ui32ICacheT1);
	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_ICPART(2), ui32ICacheT2);
	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_SYSC_ICPART(3), ui32ICacheT3);

	/* Enable instruction cache hits */
	*pui32BootConf++ = META_CR_MMCU_ICACHE_CTRL;
	*pui32BootConf++ = META_CR_MMCU_XCACHE_CTRL_CACHE_HITS_EN;

	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  META_CR_MMCU_ICACHE_CTRL,
	                  META_CR_MMCU_XCACHE_CTRL_CACHE_HITS_EN);

	*pui32BootConf++ = 0x040000C0;
	*pui32BootConf++ = 0;

	RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
	                  0x040000C0, 0);

	*ppui32BootConf = pui32BootConf;
}

/*!
*******************************************************************************

 @Function      ProcessLDRCommandStream

 @Description   Process the output of the Meta toolchain in the .LDR format
                copying code and data sections into their final location and
                passing some information to the Meta bootloader

 @Input         hPrivate             : Implementation specific data
 @Input         pbLDR                : Pointer to FW blob
 @Input         pvHostFWCodeAddr     : Pointer to FW code
 @Input         pvHostFWDataAddr     : Pointer to FW data
 @Input         pvHostFWCorememAddr  : Pointer to FW coremem code
 @Input         ppui32BootConf       : Pointer to bootloader data

 @Return        PVRSRV_ERROR

******************************************************************************/
static PVRSRV_ERROR ProcessLDRCommandStream(const void *hPrivate,
                                            const IMG_BYTE* pbLDR,
                                            void* pvHostFWCodeAddr,
                                            void* pvHostFWDataAddr,
                                            void* pvHostFWCorememAddr,
                                            IMG_UINT32 **ppui32BootConf)
{
	RGX_META_LDR_BLOCK_HDR *psHeader = (RGX_META_LDR_BLOCK_HDR *) pbLDR;
	RGX_META_LDR_L1_DATA_BLK *psL1Data =
	    (RGX_META_LDR_L1_DATA_BLK*) ((IMG_UINT8 *) pbLDR + psHeader->ui32SLData);

	IMG_UINT32 *pui32BootConf = *ppui32BootConf;

	RGXCommentLogInit(hPrivate, "**********************************************");
	RGXCommentLogInit(hPrivate, "************** Begin LDR Parsing *************");
	RGXCommentLogInit(hPrivate, "**********************************************");

	while (psL1Data != NULL)
	{
		if (RGX_META_LDR_BLK_IS_COMMENT(psL1Data->ui16Cmd))
		{
			/* Dont process comment blocks */
			goto NextBlock;
		}

		switch (psL1Data->ui16Cmd & RGX_META_LDR_CMD_MASK)
		{
			case RGX_META_LDR_CMD_LOADMEM:
			{
				RGX_META_LDR_L2_DATA_BLK *psL2Block =
				    (RGX_META_LDR_L2_DATA_BLK*) (((IMG_UINT8 *) pbLDR) + psL1Data->aui32CmdData[1]);
				IMG_UINT32 ui32Offset = psL1Data->aui32CmdData[0];
				IMG_UINT32 ui32DataSize = psL2Block->ui16Length - 6 /* L2 Tag length and checksum */;
				void *pvWriteAddr;
				PVRSRV_ERROR eError;

				if (RGX_META_IS_COREMEM_CODE(ui32Offset))
				{
					if(pvHostFWCorememAddr == NULL)
					{
						RGXErrorLogInit(hPrivate,
						                "ProcessLDRCommandStream: Coremem code found"
 						                "but no coremem allocation available!");

						return PVRSRV_ERROR_INIT_FAILURE;
					}

					/* Copy coremem data to buffer. The FW copies it to the actual coremem */
					ui32Offset -= RGX_META_COREMEM_CODE_BADDR;
#if defined(RGX_META_COREMEM_CODE) || defined(RGX_META_COREMEM_DATA)
					RGXMemCopy(hPrivate,
					           (void*)((IMG_UINT8 *)pvHostFWCorememAddr + ui32Offset),
					           psL2Block->aui32BlockData,
					           ui32DataSize);
#else
					/* no need to copy the dummy symbols */
					PVR_UNREFERENCED_PARAMETER(pvHostFWCorememAddr);
#endif
				}
				else
				{
					/* Global range is aliased to local range */
					ui32Offset &= ~META_MEM_GLOBAL_RANGE_BIT;

					eError = FindMMUSegment(ui32Offset,
					                        pvHostFWCodeAddr,
					                        pvHostFWDataAddr,
					                        &pvWriteAddr);

					if (eError != PVRSRV_OK)
					{
						RGXErrorLogInit(hPrivate,
						                "ProcessLDRCommandStream: Addr 0x%x (size: %d) not found in any segment",
						                ui32Offset, ui32DataSize);
						return eError;
					}

					/* Write to FW allocation only if available */
					if (pvWriteAddr)
					{
						RGXMemCopy(hPrivate,
						           pvWriteAddr,
						           psL2Block->aui32BlockData,
						           ui32DataSize);
					}
				}

				break;
			}
			case RGX_META_LDR_CMD_LOADCORE:
			case RGX_META_LDR_CMD_LOADMMREG:
			{
				return PVRSRV_ERROR_INIT_FAILURE;
			}
			case RGX_META_LDR_CMD_START_THREADS:
			{
				/* Don't process this block */
				break;
			}
			case RGX_META_LDR_CMD_ZEROMEM:
			{
				IMG_UINT32 ui32Offset = psL1Data->aui32CmdData[0];
				IMG_UINT32 ui32ByteCount = psL1Data->aui32CmdData[1];
				void *pvWriteAddr;
				PVRSRV_ERROR  eError;

				if (RGX_META_IS_COREMEM_DATA(ui32Offset))
				{
					/* cannot zero coremem directly */
					break;
				}

				/* Global range is aliased to local range */
				ui32Offset &= ~META_MEM_GLOBAL_RANGE_BIT;

				eError = FindMMUSegment(ui32Offset,
				                        pvHostFWCodeAddr,
				                        pvHostFWDataAddr,
				                        &pvWriteAddr);

				if(eError != PVRSRV_OK)
				{
					RGXErrorLogInit(hPrivate,
					                "ProcessLDRCommandStream: Addr 0x%x (size: %d) not found in any segment",
					                ui32Offset, ui32ByteCount);
					return eError;
				}

				/* Write to FW allocation only if available */
				if (pvWriteAddr)
				{
					RGXMemSet(hPrivate, pvWriteAddr, 0, ui32ByteCount);
				}

				break;
			}
			case RGX_META_LDR_CMD_CONFIG:
			{
				RGX_META_LDR_L2_DATA_BLK *psL2Block =
				    (RGX_META_LDR_L2_DATA_BLK*) (((IMG_UINT8 *) pbLDR) + psL1Data->aui32CmdData[0]);
				RGX_META_LDR_CFG_BLK *psConfigCommand = (RGX_META_LDR_CFG_BLK*) psL2Block->aui32BlockData;
				IMG_UINT32 ui32L2BlockSize = psL2Block->ui16Length - 6 /* L2 Tag length and checksum */;
				IMG_UINT32 ui32CurrBlockSize = 0;

				while(ui32L2BlockSize)
				{
					switch (psConfigCommand->ui32Type)
					{
						case RGX_META_LDR_CFG_PAUSE:
						case RGX_META_LDR_CFG_READ:
						{
							ui32CurrBlockSize = 8;
							return PVRSRV_ERROR_INIT_FAILURE;
						}
						case RGX_META_LDR_CFG_WRITE:
						{
							IMG_UINT32 ui32RegisterOffset = psConfigCommand->aui32BlockData[0];
							IMG_UINT32 ui32RegisterValue  = psConfigCommand->aui32BlockData[1];

							/* Only write to bootloader if we got a valid
							 * pointer to the FW code allocation
							 */
							if (pui32BootConf)
							{
								/* Do register write */
								*pui32BootConf++ = ui32RegisterOffset;
								*pui32BootConf++ = ui32RegisterValue;
							}

							RGXCommentLogInit(hPrivate, "Meta SP: [0x%08x] = 0x%08x",
							                  ui32RegisterOffset, ui32RegisterValue);

							ui32CurrBlockSize = 12;
							break;
						}
						case RGX_META_LDR_CFG_MEMSET:
						case RGX_META_LDR_CFG_MEMCHECK:
						{
							ui32CurrBlockSize = 20;
							return PVRSRV_ERROR_INIT_FAILURE;
						}
						default:
						{
							return PVRSRV_ERROR_INIT_FAILURE;
						}
					}
					ui32L2BlockSize -= ui32CurrBlockSize;
					psConfigCommand = (RGX_META_LDR_CFG_BLK*) (((IMG_UINT8*) psConfigCommand) + ui32CurrBlockSize);
				}

				break;
			}
			default:
			{
				return PVRSRV_ERROR_INIT_FAILURE;
			}
		}

NextBlock:

		if (psL1Data->ui32Next == 0xFFFFFFFF)
		{
			psL1Data = NULL;
		}
		else
		{
			psL1Data = (RGX_META_LDR_L1_DATA_BLK*) (((IMG_UINT8 *) pbLDR) + psL1Data->ui32Next);
		}
	}

	*ppui32BootConf = pui32BootConf;

	RGXCommentLogInit(hPrivate, "**********************************************");
	RGXCommentLogInit(hPrivate, "************** End Loader Parsing ************");
	RGXCommentLogInit(hPrivate, "**********************************************");

	return PVRSRV_OK;
}
#else /* RGX_FEATURE_META */
/*!
*******************************************************************************

 @Function      ProcessELFCommandStream

 @Description   Process the output of the Mips toolchain in the .ELF format
                copying code and data sections into their final location

 @Input         hPrivate          : Implementation specific data
 @Input         pbELF             : Pointer to FW blob
 @Input         pvHostFWCodeAddr  : Pointer to FW code

 @Return        PVRSRV_ERROR

******************************************************************************/
static PVRSRV_ERROR ProcessELFCommandStream(const void *hPrivate,
                                            const IMG_BYTE *pbELF,
                                            void *pvHostFWCodeAddr)
{
	IMG_UINT32 ui32Entry;
	IMG_UINT8 *pui8BootBase = pvHostFWCodeAddr;
	IMG_UINT32 ui32FWBootCodeBase = RGXMIPSFW_BOOTCODE_BASE_PAGE * RGXMIPSFW_PAGE_SIZE;
	IMG_UINT32 ui32NMIBase = RGXMIPSFW_NMI_BASE_PAGE * RGXMIPSFW_PAGE_SIZE;
	IMG_UINT32 ui32FWExceptionsCodeBase = RGXMIPSFW_EXCEPTIONSVECTORS_BASE_PAGE * RGXMIPSFW_PAGE_SIZE;
	IMG_UINT32 ui32FWCodeBase = RGXMIPSFW_CODE_BASE_PAGE * RGXMIPSFW_PAGE_SIZE;
	RGX_MIPS_ELF_HDR *psHeader = (RGX_MIPS_ELF_HDR *)pbELF;
	RGX_MIPS_ELF_PROGRAM_HDR *psProgramHeader =
	    (RGX_MIPS_ELF_PROGRAM_HDR *)(pbELF + psHeader->ui32Ephoff);

/*
 * The final image in memory is as follows:
 * FWCODE
 * FWDATA
 * EXCEPTION
 * BOOT CODE
 * INJECTED BOOT DATA
 * NMI_DEBUG_CODE
 */

	for (ui32Entry = 0; ui32Entry < psHeader->ui32Ephnum; ui32Entry++, psProgramHeader ++)
	{
		if (psProgramHeader->ui32Pvaddr >= RGXMIPSFW_BOOT_VIRTUAL_BASE &&
		    psProgramHeader->ui32Pvaddr < RGXMIPSFW_BOOT_VIRTUAL_BASE + RGXMIPSFW_PAGE_SIZE &&
		    psProgramHeader->ui32Ptype == ELF_PT_LOAD)
		{
			/*Copy the FW boot code*/
			/*The AND operation is to retain the offset in the page*/
			RGXMemCopy(hPrivate,
			           pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32FWBootCodeBase,
			           (IMG_UINT8 *)(pbELF + psProgramHeader->ui32Poffset),
			           psProgramHeader->ui32Pfilesz);
			RGXMemSet(hPrivate,
			          pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32FWBootCodeBase + psProgramHeader->ui32Pfilesz,
			          0,
			          psProgramHeader->ui32Pmemsz - psProgramHeader->ui32Pfilesz);
		}

		else if (psProgramHeader->ui32Pvaddr >= (IMG_UINT32)RGXMIPSFW_EXCEPTIONS_VIRTUAL_BASE &&
		         psProgramHeader->ui32Pvaddr < RGXMIPSFW_PAGE_SIZE + (IMG_UINT32)RGXMIPSFW_EXCEPTIONS_VIRTUAL_BASE &&
		         psProgramHeader->ui32Ptype == ELF_PT_LOAD)
		{
			/*Copy the exception_vectors*/
			RGXMemCopy(hPrivate,
			           pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32FWExceptionsCodeBase,
			           (IMG_UINT8 *)(pbELF + psProgramHeader->ui32Poffset),
			           psProgramHeader->ui32Pfilesz);
			RGXMemSet(hPrivate,
			          pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32FWExceptionsCodeBase + psProgramHeader->ui32Pfilesz,
			          0,
			          psProgramHeader->ui32Pmemsz - psProgramHeader->ui32Pfilesz);
		}

		else if (psProgramHeader->ui32Pvaddr >= (asRGXFWCodeSegments[0]).ui32SegStartAddr &&
		         psProgramHeader->ui32Pvaddr < (asRGXFWCodeSegments[0]).ui32SegStartAddr + RGXMIPSFW_FWCODE_SIZE &&
		         psProgramHeader->ui32Ptype == ELF_PT_LOAD)
		{
			/*Copy the FW code*/
			RGXMemCopy(hPrivate,
			           pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32FWCodeBase,
			           (IMG_UINT8 *)(pbELF + psProgramHeader->ui32Poffset),
			           psProgramHeader->ui32Pfilesz);
			RGXMemSet(hPrivate,
			          pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32FWCodeBase + psProgramHeader->ui32Pfilesz,
			          0,
			          psProgramHeader->ui32Pmemsz - psProgramHeader->ui32Pfilesz);
		}
		else if (psProgramHeader->ui32Pvaddr >= RGXMIPSFW_NMI_VIRTUAL_BASE &&
		         psProgramHeader->ui32Pvaddr < RGXMIPSFW_NMI_VIRTUAL_BASE + RGXMIPSFW_PAGE_SIZE &&
		         psProgramHeader->ui32Ptype == ELF_PT_LOAD)
		{
			/*Copy the NMI handling code*/
			RGXMemCopy(hPrivate,
			           pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32NMIBase,
			           (IMG_UINT8 *)(pbELF + psProgramHeader->ui32Poffset),
			           psProgramHeader->ui32Pfilesz);
			RGXMemSet(hPrivate,
			          pui8BootBase + (psProgramHeader->ui32Pvaddr & (RGXMIPSFW_PAGE_SIZE - 1)) + ui32NMIBase + psProgramHeader->ui32Pfilesz,
			          0,
			          psProgramHeader->ui32Pmemsz - psProgramHeader->ui32Pfilesz);
		}
		else
		{
			/* Ignore anything else */
		}
	}
	return PVRSRV_OK;
}
#endif /* RGX_FEATURE_META */


void RGXGetFWImageAllocSize(IMG_DEVMEM_SIZE_T *puiFWCodeAllocSize,
                            IMG_DEVMEM_SIZE_T *puiFWDataAllocSize,
                            IMG_DEVMEM_SIZE_T *puiFWCorememAllocSize)
{
	IMG_UINT32 i;

	*puiFWCodeAllocSize = 0;
	*puiFWDataAllocSize = 0;
	*puiFWCorememAllocSize = 0;

	/* Calculate how much memory the FW needs for its code and data segments */

	for(i = 0; i < RGXFW_NUM_CODE_SEGMENTS; i++) {
		*puiFWCodeAllocSize += asRGXFWCodeSegments[i].ui32SegAllocSize;
	}

	for(i = 0; i < RGXFW_NUM_DATA_SEGMENTS; i++) {
		*puiFWDataAllocSize += asRGXFWDataSegments[i].ui32SegAllocSize;
	}

#if defined(RGX_META_COREMEM_CODE)
	*puiFWCorememAllocSize = RGX_META_COREMEM_CODE_SIZE;
#endif

#if defined(RGX_FEATURE_MIPS)
	PVR_ASSERT((*puiFWCodeAllocSize % RGXMIPSFW_PAGE_SIZE) == 0);
	PVR_ASSERT((*puiFWDataAllocSize % RGXMIPSFW_PAGE_SIZE) == 0);
#endif
}


PVRSRV_ERROR RGXProcessFWImage(const void           *hPrivate,
                               const IMG_BYTE       *pbRGXFirmware,
                               void                 *pvFWCode,
                               void                 *pvFWData,
                               void                 *pvFWCorememCode,
                               IMG_DEV_VIRTADDR     *psFWCodeDevVAddrBase,
                               IMG_DEV_VIRTADDR     *psFWDataDevVAddrBase,
                               IMG_DEV_VIRTADDR     *psFWCorememDevVAddrBase,
                               RGXFWIF_DEV_VIRTADDR *psFWCorememFWAddr,
                               RGXFWIF_DEV_VIRTADDR *psRGXFwInit,
                               IMG_UINT32           ui32NumThreads,
                               IMG_UINT32           ui32MainThreadID)
{
	PVRSRV_ERROR eError = PVRSRV_OK;

#if defined(RGX_FEATURE_META)
	IMG_UINT32 *pui32BootConf = NULL;

	/* Skip bootloader configuration if a pointer to the FW code
	 * allocation is not available
	 */
	if (pvFWCode)
	{
		/* This variable points to the bootloader code which is mostly
		 * a sequence of <register address,register value> pairs
		 */
		pui32BootConf = ((IMG_UINT32*) pvFWCode) + RGXFW_BOOTLDR_CONF_OFFSET;
	
		/* Slave port and JTAG accesses are privileged */
		*pui32BootConf++ = META_CR_SYSC_JTAG_THREAD;
		*pui32BootConf++ = META_CR_SYSC_JTAG_THREAD_PRIV_EN;

		RGXFWConfigureSegMMU(hPrivate,
		                     psFWCodeDevVAddrBase,
		                     psFWDataDevVAddrBase,
		                     &pui32BootConf);
	}

	/* Process FW image data stream */
	eError = ProcessLDRCommandStream(hPrivate,
	                                 pbRGXFirmware,
	                                 pvFWCode,
	                                 pvFWData,
	                                 pvFWCorememCode,
	                                 &pui32BootConf);
	if (eError != PVRSRV_OK)
	{
		RGXErrorLogInit(hPrivate, "RGXProcessFWImage: Processing FW image failed (%d)", eError);
		return eError;
	}

	/* Skip bootloader configuration if a pointer to the FW code
	 * allocation is not available
	 */
	if (pvFWCode)
	{
		if ((ui32NumThreads == 0) || (ui32NumThreads > 2) || (ui32MainThreadID >= 2))
		{
			RGXErrorLogInit(hPrivate,
			                "ProcessFWImage: Wrong Meta threads configuration, using one thread only");

			ui32NumThreads = 1;
			ui32MainThreadID = 0;
		}

		RGXFWConfigureMetaCaches(hPrivate,
		                         ui32NumThreads,
		                         ui32MainThreadID,
		                         &pui32BootConf);

		/* Signal the end of the conf sequence */
		*pui32BootConf++ = 0x0;
		*pui32BootConf++ = 0x0;

		/* The FW main argv arguments start here */
		*pui32BootConf++ = psRGXFwInit->ui32Addr;

#if defined(RGX_META_COREMEM_CODE) || defined(RGX_META_COREMEM_DATA)
		*pui32BootConf++ = psFWCorememFWAddr->ui32Addr;
#else
		PVR_UNREFERENCED_PARAMETER(psFWCorememFWAddr);
#endif

#if defined(RGX_FEATURE_META_DMA)
		*pui32BootConf++ = (IMG_UINT32) (psFWCorememDevVAddrBase->uiAddr >> 32);
		*pui32BootConf++ = (IMG_UINT32) psFWCorememDevVAddrBase->uiAddr;
#else
		PVR_UNREFERENCED_PARAMETER(psFWCorememDevVAddrBase);
#endif
	}

#else /* defined(RGX_FEATURE_META) */
	/* Process FW image data stream */
	eError = ProcessELFCommandStream(hPrivate,
	                                 pbRGXFirmware,
	                                 pvFWCode);
	if (eError != PVRSRV_OK)
	{
		RGXErrorLogInit(hPrivate, "RGXProcessFWImage: Processing FW image failed (%d)", eError);
		return eError;
	}

	PVR_UNREFERENCED_PARAMETER(pvFWData); /* No need to touch the data segment in MIPS */
	PVR_UNREFERENCED_PARAMETER(pvFWCorememCode); /* Coremem N/A in MIPS */
	PVR_UNREFERENCED_PARAMETER(psFWCodeDevVAddrBase);
	PVR_UNREFERENCED_PARAMETER(psFWDataDevVAddrBase);
	PVR_UNREFERENCED_PARAMETER(psFWCorememDevVAddrBase);
	PVR_UNREFERENCED_PARAMETER(psFWCorememFWAddr);
	PVR_UNREFERENCED_PARAMETER(psRGXFwInit);
	PVR_UNREFERENCED_PARAMETER(ui32NumThreads);
	PVR_UNREFERENCED_PARAMETER(ui32MainThreadID);
#endif

	return eError;
}

