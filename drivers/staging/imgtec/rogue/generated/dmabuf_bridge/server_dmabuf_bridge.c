/*************************************************************************/ /*!
@File
@Title          Server bridge for dmabuf
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    Implements the server side of the bridge for dmabuf
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

#include <stddef.h>
#include <asm/uaccess.h>

#include "img_defs.h"

#include "physmem_dmabuf.h"
#include "pmr.h"


#include "common_dmabuf_bridge.h"

#include "allocmem.h"
#include "pvr_debug.h"
#include "connection_server.h"
#include "pvr_bridge.h"
#include "rgx_bridge.h"
#include "srvcore.h"
#include "handle.h"

#include <linux/slab.h>




/* ***************************************************************************
 * Server-side bridge entry points
 */
 
static IMG_INT
PVRSRVBridgePhysmemImportDmaBuf(IMG_UINT32 ui32DispatchTableEntry,
					  PVRSRV_BRIDGE_IN_PHYSMEMIMPORTDMABUF *psPhysmemImportDmaBufIN,
					  PVRSRV_BRIDGE_OUT_PHYSMEMIMPORTDMABUF *psPhysmemImportDmaBufOUT,
					 CONNECTION_DATA *psConnection)
{
	PMR * psPMRPtrInt = NULL;





	PMRLock();


	psPhysmemImportDmaBufOUT->eError =
		PhysmemImportDmaBuf(psConnection, OSGetDevData(psConnection),
					psPhysmemImportDmaBufIN->ifd,
					psPhysmemImportDmaBufIN->uiFlags,
					&psPMRPtrInt,
					&psPhysmemImportDmaBufOUT->uiSize,
					&psPhysmemImportDmaBufOUT->sAlign);
	/* Exit early if bridged call fails */
	if(psPhysmemImportDmaBufOUT->eError != PVRSRV_OK)
	{
		PMRUnlock();
		goto PhysmemImportDmaBuf_exit;
	}
	PMRUnlock();


	psPhysmemImportDmaBufOUT->eError = PVRSRVAllocHandle(psConnection->psHandleBase,
							&psPhysmemImportDmaBufOUT->hPMRPtr,
							(void *) psPMRPtrInt,
							PVRSRV_HANDLE_TYPE_PHYSMEM_PMR,
							PVRSRV_HANDLE_ALLOC_FLAG_SHARED
							,(PFN_HANDLE_RELEASE)&PMRUnrefPMR);
	if (psPhysmemImportDmaBufOUT->eError != PVRSRV_OK)
	{
		goto PhysmemImportDmaBuf_exit;
	}




PhysmemImportDmaBuf_exit:
	if (psPhysmemImportDmaBufOUT->eError != PVRSRV_OK)
	{
		if (psPMRPtrInt)
		{
			PMRUnrefPMR(psPMRPtrInt);
		}
	}


	return 0;
}

static IMG_INT
PVRSRVBridgePhysmemExportDmaBuf(IMG_UINT32 ui32DispatchTableEntry,
					  PVRSRV_BRIDGE_IN_PHYSMEMEXPORTDMABUF *psPhysmemExportDmaBufIN,
					  PVRSRV_BRIDGE_OUT_PHYSMEMEXPORTDMABUF *psPhysmemExportDmaBufOUT,
					 CONNECTION_DATA *psConnection)
{
	PMR * psPMRInt = NULL;





	PMRLock();


				{
					/* Look up the address from the handle */
					psPhysmemExportDmaBufOUT->eError =
						PVRSRVLookupHandle(psConnection->psHandleBase,
											(void **) &psPMRInt,
											psPhysmemExportDmaBufIN->hPMR,
											PVRSRV_HANDLE_TYPE_PHYSMEM_PMR);
					if(psPhysmemExportDmaBufOUT->eError != PVRSRV_OK)
					{
						PMRUnlock();
						goto PhysmemExportDmaBuf_exit;
					}
				}


	psPhysmemExportDmaBufOUT->eError =
		PhysmemExportDmaBuf(psConnection, OSGetDevData(psConnection),
					psPMRInt,
					&psPhysmemExportDmaBufOUT->iFd);
	PMRUnlock();




PhysmemExportDmaBuf_exit:

	return 0;
}



/* *************************************************************************** 
 * Server bridge dispatch related glue 
 */

static IMG_BOOL bUseLock = IMG_TRUE;

PVRSRV_ERROR InitDMABUFBridge(void);
PVRSRV_ERROR DeinitDMABUFBridge(void);

/*
 * Register all DMABUF functions with services
 */
PVRSRV_ERROR InitDMABUFBridge(void)
{

	SetDispatchTableEntry(PVRSRV_BRIDGE_DMABUF, PVRSRV_BRIDGE_DMABUF_PHYSMEMIMPORTDMABUF, PVRSRVBridgePhysmemImportDmaBuf,
					NULL, bUseLock);

	SetDispatchTableEntry(PVRSRV_BRIDGE_DMABUF, PVRSRV_BRIDGE_DMABUF_PHYSMEMEXPORTDMABUF, PVRSRVBridgePhysmemExportDmaBuf,
					NULL, bUseLock);


	return PVRSRV_OK;
}

/*
 * Unregister all dmabuf functions with services
 */
PVRSRV_ERROR DeinitDMABUFBridge(void)
{
	return PVRSRV_OK;
}

