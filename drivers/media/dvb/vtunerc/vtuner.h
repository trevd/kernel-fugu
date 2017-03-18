/*
 * vtunerc: /dev/vtunerc API
 *
 * Copyright (C) 2010-11 Honza Petrous <jpetr...@smartimp.cz>
 * [based on dreamtuner userland code by Roland Mieslinger]
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _VTUNER_H_
#define _VTUNER_H_

#ifndef HAVE_DVB_API_VERSION
#define HAVE_DVB_API_VERSION 5
#endif

#if HAVE_DVB_API_VERSION < 3
  #include <ost/frontend.h>
  #include <ost/dmx.h>
  #include <ost/sec.h>
#else
  #include <linux/dvb/version.h>
  #include <linux/dvb/frontend.h>
  #include <linux/dvb/dmx.h>
#endif

#define VT_S   0x01
#define VT_C   0x02
#define VT_T   0x04
#define VT_S2  0x08

#define MSG_SET_FRONTEND               1
#define MSG_GET_FRONTEND               2
#define MSG_READ_STATUS                        3
#define MSG_READ_BER                   4
#define MSG_READ_SIGNAL_STRENGTH       5
#define MSG_READ_SNR                   6
#define MSG_READ_UCBLOCKS              7
#define MSG_SET_TONE                   8
#define MSG_SET_VOLTAGE                        9
#define MSG_ENABLE_HIGH_VOLTAGE                10
#define MSG_SEND_DISEQC_MSG            11
#define MSG_SEND_DISEQC_BURST          13
#define MSG_PIDLIST                    14
#define MSG_TYPE_CHANGED               15
#define MSG_SET_PROPERTY               16
#define MSG_GET_PROPERTY               17

#define MSG_NULL                       1024
#define MSG_DISCOVER                   1025
#define MSG_UPDATE                     1026

struct diseqc_master_cmd {
       __u8 msg[6];
       __u8 msg_len;
};

#if DVB_API_VERSION < 5
struct dtv_property {
       __u32 cmd;
       __u32 reserved[3];
       union {
               __u32 data;
               struct {
                       __u8 data[32];
                       __u32 len;
                       __u32 reserved1[3];
                       void *reserved2;
               } buffer;
       } u;
       int result;
} __attribute__ ((packed));

#define DTV_UNDEFINED          0
#define DTV_TUNE               1
#define DTV_CLEAR              2
#define DTV_FREQUENCY          3
#define DTV_MODULATION         4
#define DTV_BANDWIDTH_HZ       5
#define DTV_INVERSION          6
#define DTV_DISEQC_MASTER      7
#define DTV_SYMBOL_RATE                8
#define DTV_INNER_FEC          9
#define DTV_VOLTAGE            10
#define DTV_TONE               11
#define DTV_PILOT              12
#define DTV_ROLLOFF            13
#define DTV_DISEQC_SLAVE_REPLY 14
#define DTV_FE_CAPABILITY_COUNT        15
#define DTV_FE_CAPABILITY      16
#define DTV_DELIVERY_SYSTEM    17

#define DTV_IOCTL_MAX_MSGS     64

#endif

struct vtuner_message {
       __s32 type;
       union {
               struct {
                       __u32   frequency;
                       __u8    inversion;
                       union {
                               struct {
                                       __u32   symbol_rate;
                                       __u32   fec_inner;
                               } qpsk;
                               struct {
                                       __u32   symbol_rate;
                                       __u32   fec_inner;
                                       __u32   modulation;
                               } qam;
                               struct {
                                       __u32   bandwidth;
                                       __u32   code_rate_HP;
                                       __u32   code_rate_LP;
                                       __u32   constellation;
                                       __u32   transmission_mode;
                                       __u32   guard_interval;
                                       __u32   hierarchy_information;
                               } ofdm;
                               struct {
                                       __u32   modulation;
                               } vsb;
                       } u;
               } fe_params;
               struct dtv_property prop;
               __u32 status;
               __u32 ber;
               __u16 ss;
               __u16 snr;
               __u32 ucb;
               __u8 tone;
               __u8 voltage;
               struct diseqc_master_cmd diseqc_master_cmd;
               __u8 burst;
               __u16 pidlist[30];
               __u8  pad[72];
               __u32 type_changed;
       } body;
};

#define VTUNER_MAJOR           226

/*#define PVR_FLUSH_BUFFER     _IO(VTUNER_MAJOR, 0)*/
#define VTUNER_GET_MESSAGE     _IOR(VTUNER_MAJOR, 1, struct vtuner_message *)
#define VTUNER_SET_RESPONSE    _IOW(VTUNER_MAJOR, 2, struct vtuner_message *)
#define VTUNER_SET_NAME                _IOW(VTUNER_MAJOR, 3, char *)
#define VTUNER_SET_TYPE                _IOW(VTUNER_MAJOR, 4, char *)
#define VTUNER_SET_HAS_OUTPUTS _IOW(VTUNER_MAJOR, 5, char *)
#define VTUNER_SET_FE_INFO     _IOW(VTUNER_MAJOR, 6, struct dvb_frontend_info *)
#define VTUNER_SET_NUM_MODES   _IOW(VTUNER_MAJOR, 7, int)
#define VTUNER_SET_MODES       _IOW(VTUNER_MAJOR, 8, char *)

#endif

