ifeq ($(CONFIG_BCM_DHD_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG -DDHD_DEBUG -DSDTEST -DBCMDBG -DSRCBASE=\"$(obj)\"
endif

ifeq ($(CONFIG_BCM_DHD_STATIC_BUF),y)
EXTRA_CFLAGS += -DDHD_USE_STATIC_BUF
endif

ifneq ("$(wildcard $($(objtree)/.config))","")
    include $(objtree)/.config
endif

DFLAGS += -DSDIO_ISR_THREAD
DFLAGS += -DDHDTHREAD
DFLAGS += -DOEM_CHROMIUMOS
DFLAGS += -DCONFIG_DHD_MUTEXES=1 -DWITH_OBSOLETE_CODE=0
DFLAGS += -DLINUX -DBCMDRIVER -DBCMDONGLEHOST
DFLAGS += -DUNRELEASEDCHIP -DBCMDMA32
DFLAGS += -DBCMFILEIMAGE
DFLAGS += -DCONFIG_WIRELESS_EXT
DFLAGS += -DWIFI_ACT_FRAME
DFLAGS += -DDHD_GPL -DDHD_SCHED
DFLAGS += -DBDC -DTOE
DFLAGS += -DDHD_BCMEVENTS -DSHOW_EVENTS
DFLAGS += -DBCMSDIO -DBCMLXSDMMC -DBCMPLATFORM_BUS

ifeq ($(CONFIG_MMC_MSM7X00A),y)
DFLAGS += -Dlinux
DFLAGS += -DDHD_SDALIGN=64 -DMAX_HDR_READ=64 -DDHD_FIRSTREAD=64
endif

obj-$(CONFIG_BCM_DHD) += dhd.o

dhd-objs := dhd_common.o \
	    dhd_cdc.o \
	    dhd_custom_gpio.o \
	    dhd_linux.o \
	    dhd_linux_sched.o \
	    dhd_sdio.o \
	    aiutils.o \
	    bcmevent.o \
	    bcmsdh.o \
	    bcmsdh_linux.o \
	    bcmsdh_sdmmc.o \
	    bcmsdh_sdmmc_linux.o \
	    bcmutils.o \
	    bcmwifi.o \
	    hndpmu.o \
	    linux_osl.o \
	    sbutils.o \
	    siutils.o \
	    wl_iw.o



ifeq ($(subst m,y,$(CONFIG_CFG80211)),y)
ifeq ($(CPTCFG_BACKPORT_KERNEL_3_99),y)
ifeq ($(CPTCFG_KOBO_BCM_DHD_DEVELOPING),y)
DFLAGS += -DBACKPORT_KERNEL_DRIVER
DFLAGS += -DWL_CFG80211
dhd-objs += wl_cfg80211.o
DFLAGS += -DWLP2P
dhd-objs += wl_cfgp2p.o
endif
else
## wl_cfg80211.c will not compile on this kernel
## due to missing interface/older headers to the nl80211 stuff
## you will need the backport-kernel-driver environment
DFLAGS += -DWL_CFG80211
dhd-objs += wl_cfg80211.o
ifeq ($(CONFIG_BCM_DHD_WIFI_DIRECT),y)
# try what we can do with this, it says Wifi-direct support,
# seems to be useable for repeaters
DFLAGS += -DWLP2P
dhd-objs += wl_cfgp2p.o
endif

endif
endif


ccflags-y = -I$(obj)/include -I$(obj)/. $(DFLAGS)
