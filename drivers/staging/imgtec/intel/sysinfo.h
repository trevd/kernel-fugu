/*************************************************************************/ /*!
@File           
@Title          System Description Header
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    This header provides system-specific declarations and macros
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

#if !defined(__SYSINFO_H__)
#define __SYSINFO_H__

/*! Sleep time (1h) for Devices Watchdog thread when GPU is in power off state */
#define DEVICES_WATCHDOG_POWER_OFF_SLEEP_TIMEOUT 60 * 60 * 1000
#define DEVICES_WATCHDOG_POWER_ON_SLEEP_TIMEOUT  (10000)

/*!< System specific poll/timeout details */
#define MAX_HW_TIME_US                 (500000)
#define FATAL_ERROR_DETECTION_POLL_MS  (10000)
#define WAIT_TRY_COUNT                 (10000)

#define SYS_DEVICE_COUNT 3 /* RGX, DISPLAY (external), BUFFER (external) */

#if defined(SUPPORT_TRUSTED_DEVICE)
#define SYS_PHYS_HEAP_COUNT		2
#else
#define SYS_PHYS_HEAP_COUNT		1
#endif

#define SYS_RGX_DEV_VENDOR_ID		0x8086
#define SYS_RGX_DEV_DEVICE_ID		0x1180

/*!
* Active Power Latency default in ms
*/
#define RGX_APM_LATENCY_DEFAULT			(500)

/*!
* Core Clock Speed in Hz
*/
#define RGX_CORE_CLOCK_SPEED_DEFAULT	(400000000)

#if defined(__linux__)
#define SYS_RGX_DEV_NAME    "Merrifield"
#if defined(SUPPORT_DRM)
/*
 * Use the static bus ID for the platform DRM device.
 */
#if defined(PVR_DRM_DEV_BUS_ID)
#define	SYS_RGX_DEV_DRM_BUS_ID	PVR_DRM_DEV_BUS_ID
#else
#define SYS_RGX_DEV_DRM_BUS_ID	"platform:Merrifield"
#endif	/* defined(PVR_DRM_DEV_BUS_ID) */
#endif	/* defined(SUPPORT_DRM) */
#endif

#endif	/* !defined(__SYSINFO_H__) */
