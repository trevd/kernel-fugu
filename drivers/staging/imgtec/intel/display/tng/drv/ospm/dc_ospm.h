/**************************************************************************
 * Copyright (c) 2012, Intel Corporation.
 * All Rights Reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Hitesh K. Patel <hitesh.k.patel@intel.com>
 */

#ifndef _TNG_DISPLAY_OSPM_H_
#define _TNG_DISPLAY_OSPM_H_

#include "pwr_mgmt.h"


#define PMU_DISP_A		0x1
#define PMU_DISP_B		0x2
#define PMU_DISP_C		0x4
#define PMU_MIO			0x1
#define PMU_HDMI		0x1

void ospm_disp_a_init(struct drm_device *dev,
			struct ospm_power_island *p_island);

void ospm_disp_b_init(struct drm_device *dev,
			struct ospm_power_island *p_island);

void ospm_disp_c_init(struct drm_device *dev,
			struct ospm_power_island *p_island);

void ospm_mio_init(struct drm_device *dev,
			struct ospm_power_island *p_island);

void ospm_hdmi_init(struct drm_device *dev,
			struct ospm_power_island *p_island);


#endif	/* _TNG_DISPLAY_OSPM_H_ */
