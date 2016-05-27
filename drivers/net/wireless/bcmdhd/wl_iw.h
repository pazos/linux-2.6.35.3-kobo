/*
 * Linux Wireless Extensions support
 *
 * Copyright (C) 2011, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: wl_iw.h,v 1.15.80.6 2010-12-23 01:13:23 Exp $
 */

#ifndef _wl_iw_h_
#define _wl_iw_h_

#include <linux/wireless.h>

#include <typedefs.h>
#include <proto/ethernet.h>
#include <wlioctl.h>

#define WL_SCAN_PARAMS_SSID_MAX 	10
#define GET_SSID			"SSID="
#define GET_CHANNEL			"CH="
#define GET_NPROBE 			"NPROBE="
#define GET_ACTIVE_ASSOC_DWELL  	"ACTIVE="
#define GET_PASSIVE_ASSOC_DWELL  	"PASSIVE="
#define GET_HOME_DWELL  		"HOME="
#define GET_SCAN_TYPE			"TYPE="

/* Lin - Has Android changed these strings ? */
#define BAND_GET_CMD				"GETBAND"
#define BAND_SET_CMD				"SETBAND"
#define DTIM_SKIP_GET_CMD			"DTIMSKIPGET"
#define DTIM_SKIP_SET_CMD			"DTIMSKIPSET"
#define SETSUSPEND_CMD				"SETSUSPENDOPT"
#define PNOSSIDCLR_SET_CMD			"PNOSSIDCLR"
/* Lin - Is the extra space needed? */
#define PNOSETUP_SET_CMD			"PNOSETUP " /* TLV command has extra end space */
#define PNOENABLE_SET_CMD			"PNOFORCE"
#define PNODEBUG_SET_CMD			"PNODEBUG"

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

/* Structure to keep global parameters */
typedef struct wl_iw_extra_params {
	int 	target_channel; /* target channel */
} wl_iw_extra_params_t;

/* ============================================== */
/* Defines from wlc_pub.h */
#define	WL_IW_RSSI_MINVAL		-200	/* Low value, e.g. for forcing roam */
#define	WL_IW_RSSI_NO_SIGNAL	-91	/* NDIS RSSI link quality cutoffs */
#define	WL_IW_RSSI_VERY_LOW	-80	/* Very low quality cutoffs */
#define	WL_IW_RSSI_LOW		-70	/* Low quality cutoffs */
#define	WL_IW_RSSI_GOOD		-68	/* Good quality cutoffs */
#define	WL_IW_RSSI_VERY_GOOD	-58	/* Very good quality cutoffs */
#define	WL_IW_RSSI_EXCELLENT	-57	/* Excellent quality cutoffs */
#define	WL_IW_RSSI_INVALID	 0	/* invalid RSSI value */
#define MAX_WX_STRING 80
#define isprint(c) bcm_isprint(c)
#define WL_IW_SET_ACTIVE_SCAN	(SIOCIWFIRSTPRIV+1)
#define WL_IW_GET_RSSI			(SIOCIWFIRSTPRIV+3)
#define WL_IW_SET_PASSIVE_SCAN	(SIOCIWFIRSTPRIV+5)
#define WL_IW_GET_LINK_SPEED	(SIOCIWFIRSTPRIV+7)
#define WL_IW_GET_CURR_MACADDR	(SIOCIWFIRSTPRIV+9)
#define WL_IW_SET_STOP				(SIOCIWFIRSTPRIV+11)
#define WL_IW_SET_START			(SIOCIWFIRSTPRIV+13)

/* IWPRIV IOCTLS for SOFTAP */
#define WL_SET_AP_CFG           (SIOCIWFIRSTPRIV+15)
#define WL_AP_STA_LIST          (SIOCIWFIRSTPRIV+17)
#define WL_AP_MAC_FLTR	        (SIOCIWFIRSTPRIV+19)
#define WL_AP_BSS_START         (SIOCIWFIRSTPRIV+21)
#define AP_LPB_CMD              (SIOCIWFIRSTPRIV+23)
#define WL_AP_STOP              (SIOCIWFIRSTPRIV+25)
#define WL_FW_RELOAD            (SIOCIWFIRSTPRIV+27)
#define WL_COMBO_SCAN            (SIOCIWFIRSTPRIV+29)
#define WL_AP_SPARE3            (SIOCIWFIRSTPRIV+31)
#define 		G_SCAN_RESULTS 8*1024
#define 		WE_ADD_EVENT_FIX	0x80
#define          G_WLAN_SET_ON	0
#define          G_WLAN_SET_OFF	1

typedef struct wl_iw {
	char nickname[IW_ESSID_MAX_SIZE];

	struct iw_statistics wstats;

	int spy_num;
	int wpaversion;			/* wpaversion setting */
	int pcipher;			/* pairwise cipher setting */
	int gcipher;			/* group cipher setting  */
	int privacy_invoked; 		/* IW_AUTH_PRIVACY_INVOKED setting */

	struct ether_addr spy_addr[IW_MAX_SPY];
	struct iw_quality spy_qual[IW_MAX_SPY];
	void  *wlinfo;
	dhd_pub_t * pub;
} wl_iw_t;


#if WIRELESS_EXT > 12
#include <net/iw_handler.h>
extern const struct iw_handler_def wl_iw_handler_def;
#endif /* WIRELESS_EXT > 12 */

extern int wl_iw_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
extern void wl_iw_event(struct net_device *dev, wl_event_msg_t *e, void* data);
extern int wl_iw_get_wireless_stats(struct net_device *dev, struct iw_statistics *wstats);
int wl_iw_attach(struct net_device *dev, void * dhdp);
void wl_iw_detach(void);

extern int net_os_wake_lock(struct net_device *dev);
extern int net_os_wake_unlock(struct net_device *dev);
extern int net_os_wake_lock_timeout(struct net_device *dev);
extern int net_os_wake_lock_timeout_enable(struct net_device *dev);
extern int net_os_set_suspend_disable(struct net_device *dev, int val);
extern int net_os_set_suspend(struct net_device *dev, int val);
extern int net_os_set_dtim_skip(struct net_device *dev, int val);
extern int net_os_set_packet_filter(struct net_device *dev, int val);
extern int net_os_send_hang_message(struct net_device *dev);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#define IWE_STREAM_ADD_EVENT(info, stream, ends, iwe, extra) \
	iwe_stream_add_event(info, stream, ends, iwe, extra)
#define IWE_STREAM_ADD_VALUE(info, event, value, ends, iwe, event_len) \
	iwe_stream_add_value(info, event, value, ends, iwe, event_len)
#define IWE_STREAM_ADD_POINT(info, stream, ends, iwe, extra) \
	iwe_stream_add_point(info, stream, ends, iwe, extra)
#else
#define IWE_STREAM_ADD_EVENT(info, stream, ends, iwe, extra) \
	iwe_stream_add_event(stream, ends, iwe, extra)
#define IWE_STREAM_ADD_VALUE(info, event, value, ends, iwe, event_len) \
	iwe_stream_add_value(event, value, ends, iwe, event_len)
#define IWE_STREAM_ADD_POINT(info, stream, ends, iwe, extra) \
	iwe_stream_add_point(stream, ends, iwe, extra)
#endif

extern int dhd_pno_enable(dhd_pub_t *dhd, int pfn_enabled);
extern int dhd_pno_clean(dhd_pub_t *dhd);
extern int dhd_pno_set(dhd_pub_t *dhd, wlc_ssid_t* ssids_local, int nssid, uchar  scan_fr);
extern int dhd_pno_get_status(dhd_pub_t *dhd);
extern int dhd_dev_pno_reset(struct net_device *dev);
extern int dhd_dev_pno_set(struct net_device *dev, wlc_ssid_t* ssids_local,
	int nssid, uchar  scan_fr);
extern int dhd_dev_pno_enable(struct net_device *dev,  int pfn_enabled);
extern int dhd_dev_get_pno_status(struct net_device *dev);

#define PNO_TLV_PREFIX			'S'
#define PNO_TLV_VERSION			1
#define PNO_TLV_SUBVERSION 		0
#define PNO_TLV_RESERVED		0
#define PNO_TLV_TYPE_SSID_IE		'S'
#define PNO_TLV_TYPE_TIME		'T'
#define  PNO_EVENT_UP			"PNO_EVENT"

typedef struct cmd_tlv {
	char prefix;
	char version;
	char subver;
	char reserved;
} cmd_tlv_t;



#define NETDEV_PRIV(dev)	(*(wl_iw_t **)netdev_priv(dev))

#endif /* _wl_iw_h_ */
