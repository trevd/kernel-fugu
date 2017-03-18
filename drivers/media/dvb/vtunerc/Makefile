#
# Makefile for the nGene device driver
#

vtunerc-objs = vtunerc_main.o vtunerc_ctrldev.o vtunerc_proxyfe.o

obj-$(CONFIG_DVB_VTUNERC) += vtunerc.o

EXTRA_CFLAGS += -Idrivers/media/dvb/dvb-core
EXTRA_CFLAGS += -Idrivers/media/dvb/frontends
EXTRA_CFLAGS += -Idrivers/media/common/tuners
EXTRA_CFLAGS += -Iinclude/linux

EXTRA_CFLAGS += -DHAVE_DVB_API_VERSION=5
