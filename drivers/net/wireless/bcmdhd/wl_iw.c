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
 * $Id: wl_iw.c,v 1.132.2.18 2011-02-05 01:44:47 Exp $
 */
#include <wlioctl.h>

#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>

#include <bcmutils.h>
#include <bcmendian.h>
#include <proto/ethernet.h>

#include <linux/if_arp.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

#include <dngl_stats.h>
#include <dhd.h>
#include <dhdioctl.h>

typedef void wlc_info_t;
typedef void wl_info_t;
typedef const struct si_pub  si_t;
#include <wlioctl.h>

#include <proto/ethernet.h>
#include <dngl_stats.h>
#include <dhd.h>
#define WL_ERROR(x) printf x
#define WL_TRACE(x)
#define WL_ASSOC(x)
#define WL_INFORM(x)
#define WL_WSEC(x)
#define WL_SCAN(x)

#include <wl_iw.h>



#define IW_WSEC_ENABLED(wsec)	((wsec) & (WEP_ENABLED | TKIP_ENABLED | AES_ENABLED))

#include <linux/rtnetlink.h>

#define WL_IW_USE_ISCAN  1
#define ENABLE_ACTIVE_PASSIVE_SCAN_SUPPRESS  1

#ifdef OEM_CHROMIUMOS
bool g_set_essid_before_scan = TRUE;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)) && 0
	struct mutex  g_wl_ss_scan_lock; /* lock/unlock for ISCAN cache settings */
#endif 


#define WL_IW_IOCTL_CALL(func_call) \
	do {				\
		func_call;		\
	} while (0)

#define RETURN_IF_EXTRA_NULL(extra) \
	if (!extra) { \
		WL_ERROR(("%s: error : extra is null pointer\n", __FUNCTION__)); \
		return -EINVAL; \
	}

static int		g_onoff = G_WLAN_SET_ON;
wl_iw_extra_params_t	g_wl_iw_params;


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)) && 0
/* 
 * wl_start_lock to replace MUTEX_LOCK[dhd.h]/dhd_pub_t::wl_start_stop_lock[dhd.h]
 * wl_cache_lock  to replace MUTEX_LOCK_WL_SCAN_SET[dhd.h]/g_wl_ss_scan_lock[wl_iw.c]
 */
static struct mutex	wl_start_lock;
static struct mutex	wl_cache_lock;
static struct mutex	wl_softap_lock;

#define DHD_OS_MUTEX_INIT(a) mutex_init(a)
#define DHD_OS_MUTEX_LOCK(a) mutex_lock(a)
#define DHD_OS_MUTEX_UNLOCK(a) mutex_unlock(a)

#else

#define DHD_OS_MUTEX_INIT(a)
#define DHD_OS_MUTEX_LOCK(a)
#define DHD_OS_MUTEX_UNLOCK(a)

#endif 


uint wl_msg_level = WL_ERROR_VAL;

#define MAX_WLIW_IOCTL_LEN 1024

/* IOCTL swapping mode for Big Endian host with Little Endian dongle.  Default to off */
#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i
#define htodchanspec(i) i
#define dtohchanspec(i) i

#ifdef CONFIG_WIRELESS_EXT

extern struct iw_statistics *dhd_get_wireless_stats(struct net_device *dev);
extern int dhd_wait_pend8021x(struct net_device *dev);
#endif /* CONFIG_WIRELESS_EXT */

#if WIRELESS_EXT < 19
#define IW_IOCTL_IDX(cmd)	((cmd) - SIOCIWFIRST)
#define IW_EVENT_IDX(cmd)	((cmd) - IWEVFIRST)
#endif /* WIRELESS_EXT < 19 */

static void *g_scan = NULL;
static volatile uint g_scan_specified_ssid;	/* current scan type flag, specific or broadcast */
static wlc_ssid_t g_specific_ssid;		/* cached specific ssid request */
/* caching current ssid */
static wlc_ssid_t g_ssid;



#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
#define DAEMONIZE(a) daemonize(a); \
	allow_signal(SIGKILL); \
	allow_signal(SIGTERM);
#else /* Linux 2.4 (w/o preemption patch) */
#define RAISE_RX_SOFTIRQ() \
	cpu_raise_softirq(smp_processor_id(), NET_RX_SOFTIRQ)
#define DAEMONIZE(a) daemonize(); \
	do { if (a) \
		strncpy(current->comm, a, MIN(sizeof(current->comm), (strlen(a) + 1))); \
	} while (0);
#endif /* LINUX_VERSION_CODE  */

#if defined(WL_IW_USE_ISCAN)
#define ISCAN_STATE_IDLE   0
#define ISCAN_STATE_SCANING 1

/* the buf lengh can be WLC_IOCTL_MAXLEN (8K) to reduce iteration */
#define WLC_IW_ISCAN_MAXLEN   2048
typedef struct iscan_buf {
	struct iscan_buf * next;
	char   iscan_buf[WLC_IW_ISCAN_MAXLEN];
} iscan_buf_t;

typedef struct iscan_info {
	struct net_device *dev;
	struct timer_list timer;
	uint32 timer_ms;
	uint32 timer_on;
	int    iscan_state;
	iscan_buf_t * list_hdr;
	iscan_buf_t * list_cur;

	/* Thread to work on iscan */
	long sysioc_pid;
	struct semaphore sysioc_sem;
	struct completion sysioc_exited;

#if defined CSCAN
	char ioctlbuf[WLC_IOCTL_MEDLEN];
#else
	char ioctlbuf[WLC_IOCTL_SMLEN];
#endif /* CSCAN */
	/* pointer to extra iscan params from wl_scan_params */
	wl_iscan_params_t *iscan_ex_params_p;
	int iscan_ex_param_size;
} iscan_info_t;

iscan_info_t *g_iscan = NULL;
void dhd_print_buf(void *pbuf, int len, int bytes_per_line);
static void wl_iw_timerfunc(ulong data);
static void wl_iw_set_event_mask(struct net_device *dev);
static int
wl_iw_iscan(iscan_info_t *iscan, wlc_ssid_t *ssid, uint16 action);
#endif /* WL_IW_USE_ISCAN */

static int
wl_iw_set_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
);

static int
wl_iw_get_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
);

static uint
wl_iw_get_scan_prep(
	wl_scan_results_t *list,
	struct iw_request_info *info,
	char *extra,
	short max_size
);


static void
swap_key_from_BE(
	wl_wsec_key_t *key
)
{
	key->index = htod32(key->index);
	key->len = htod32(key->len);
	key->algo = htod32(key->algo);
	key->flags = htod32(key->flags);
	key->rxiv.hi = htod32(key->rxiv.hi);
	key->rxiv.lo = htod16(key->rxiv.lo);
	key->iv_initialized = htod32(key->iv_initialized);
}

static void
swap_key_to_BE(
	wl_wsec_key_t *key
)
{
	key->index = dtoh32(key->index);
	key->len = dtoh32(key->len);
	key->algo = dtoh32(key->algo);
	key->flags = dtoh32(key->flags);
	key->rxiv.hi = dtoh32(key->rxiv.hi);
	key->rxiv.lo = dtoh16(key->rxiv.lo);
	key->iv_initialized = dtoh32(key->iv_initialized);
}

static int
dev_wlc_ioctl(
	struct net_device *dev,
	int cmd,
	void *arg,
	int len
)
{
	struct ifreq ifr;
	wl_ioctl_t ioc;
	mm_segment_t fs;
	int ret = -EINVAL;

	if (!dev) {
		WL_ERROR(("%s: dev is null\n", __FUNCTION__));
		return ret;
	}

	net_os_wake_lock(dev);

	WL_INFORM(("%s, PID:%x: send Local IOCTL -> dhd: cmd:0x%x, buf:%p, len:%d ,\n",
		__FUNCTION__, current->pid, cmd, arg, len));

	if (g_onoff == G_WLAN_SET_ON) {
		memset(&ioc, 0, sizeof(ioc));
		ioc.cmd = cmd;
		ioc.buf = arg;
		ioc.len = len;

		strcpy(ifr.ifr_name, dev->name);
		ifr.ifr_data = (caddr_t) &ioc;

		/* Must be up for virtually all useful ioctls */
		ret = dev_open(dev);
		if (ret) {
			WL_ERROR(("%s: Error dev_open: %d\n", __func__, ret));
			net_os_wake_unlock(dev);
			return ret;
		}

		fs = get_fs();
		set_fs(get_ds());
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 31)
		ret = dev->do_ioctl(dev, &ifr, SIOCDEVPRIVATE);
#else
		ret = dev->netdev_ops->ndo_do_ioctl(dev, &ifr, SIOCDEVPRIVATE);
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31) */
		set_fs(fs);
	}
	else {
		WL_TRACE(("%s: call after driver stop : ignored\n", __FUNCTION__));
	}

	net_os_wake_unlock(dev);

	return ret;
}


/*
set named driver variable to int value and return error indication
calling example: dev_wlc_intvar_set(dev, "arate", rate)
*/

static int
dev_wlc_intvar_set(
	struct net_device *dev,
	char *name,
	int val)
{
	char buf[WLC_IOCTL_SMLEN];
	uint len;

	val = htod32(val);
	len = bcm_mkiovar(name, (char *)(&val), sizeof(val), buf, sizeof(buf));
	ASSERT(len);

	return (dev_wlc_ioctl(dev, WLC_SET_VAR, buf, len));
}

#if defined(WL_IW_USE_ISCAN)
static int
dev_iw_iovar_setbuf(
	struct net_device *dev,
	char *iovar,
	void *param,
	int paramlen,
	void *bufptr,
	int buflen)
{
	int iolen;

	iolen = bcm_mkiovar(iovar, param, paramlen, bufptr, buflen);
	ASSERT(iolen);

	if (iolen == 0)
		return 0;

	return (dev_wlc_ioctl(dev, WLC_SET_VAR, bufptr, iolen));
}

static int
dev_iw_iovar_getbuf(
	struct net_device *dev,
	char *iovar,
	void *param,
	int paramlen,
	void *bufptr,
	int buflen)
{
	int iolen;

	iolen = bcm_mkiovar(iovar, param, paramlen, bufptr, buflen);
	ASSERT(iolen);

	return (dev_wlc_ioctl(dev, WLC_GET_VAR, bufptr, buflen));
}
#endif /* #ifdef BCMDONGLEHOST */


#if WIRELESS_EXT > 17
static int
dev_wlc_bufvar_set(
	struct net_device *dev,
	char *name,
	char *buf, int len)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31)
	char ioctlbuf[MAX_WLIW_IOCTL_LEN];
#else
	static char ioctlbuf[MAX_WLIW_IOCTL_LEN];
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31) */
	uint buflen;

	buflen = bcm_mkiovar(name, buf, len, ioctlbuf, sizeof(ioctlbuf));
	ASSERT(buflen);

	return (dev_wlc_ioctl(dev, WLC_SET_VAR, ioctlbuf, buflen));
}
#endif /* WIRELESS_EXT > 17 */
/*
get named driver variable to int value and return error indication
calling example: dev_wlc_intvar_get(dev, "arate", &rate)
*/

static int
dev_wlc_bufvar_get(
	struct net_device *dev,
	char *name,
	char *buf, int buflen)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31)
	char ioctlbuf[MAX_WLIW_IOCTL_LEN];
#else
	static char ioctlbuf[MAX_WLIW_IOCTL_LEN];
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31) */
	int error;
	uint len;

	len = bcm_mkiovar(name, NULL, 0, ioctlbuf, sizeof(ioctlbuf));
	ASSERT(len);
	error = dev_wlc_ioctl(dev, WLC_GET_VAR, (void *)ioctlbuf, MAX_WLIW_IOCTL_LEN);
	if (!error)
		bcopy(ioctlbuf, buf, buflen);

	return (error);
}

/*
get named driver variable to int value and return error indication
calling example: dev_wlc_intvar_get(dev, "arate", &rate)
*/

static int
dev_wlc_intvar_get(
	struct net_device *dev,
	char *name,
	int *retval)
{
	union {
		char buf[WLC_IOCTL_SMLEN];
		int val;
	} var;
	int error;

	uint len;
	uint data_null;

	len = bcm_mkiovar(name, (char *)(&data_null), 0, (char *)(&var), sizeof(var.buf));
	ASSERT(len);
	error = dev_wlc_ioctl(dev, WLC_GET_VAR, (void *)&var, len);

	*retval = dtoh32(var.val);

	return (error);
}

/* Maintain backward compatibility */
#if WIRELESS_EXT > 12
#endif /* WIRELESS_EXT > 12 */

#if WIRELESS_EXT < 13
struct iw_request_info
{
	__u16		cmd;		/* Wireless Extension command */
	__u16		flags;		/* More to come ;-) */
};

typedef int (*iw_handler)(struct net_device *dev,
                struct iw_request_info *info,
                void *wrqu,
                char *extra);
#endif /* WIRELESS_EXT < 13 */

static int
wl_iw_config_commit(
	struct net_device *dev,
	struct iw_request_info *info,
	void *zwrq,
	char *extra
)
{
	wlc_ssid_t ssid;
	int error;
	struct sockaddr bssid;

	WL_TRACE(("%s: SIOCSIWCOMMIT\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_SSID, &ssid, sizeof(ssid))))
		return error;

	ssid.SSID_len = dtoh32(ssid.SSID_len);

	if (!ssid.SSID_len)
		return 0;

	bzero(&bssid, sizeof(struct sockaddr));
	if ((error = dev_wlc_ioctl(dev, WLC_REASSOC, &bssid, ETHER_ADDR_LEN))) {
		WL_ERROR(("%s: WLC_REASSOC to %s failed \n", __FUNCTION__, ssid.SSID));
		return error;
	}

	return 0;
}

static int
wl_iw_get_name(
	struct net_device *dev,
	struct iw_request_info *info,
	char *cwrq,
	char *extra
)
{
	WL_TRACE(("%s: SIOCGIWNAME\n", dev->name));

	strcpy(cwrq, "IEEE 802.11-DS");

	return 0;
}

static int
wl_iw_set_freq(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_freq *fwrq,
	char *extra
)
{
	int error, chan;
	uint sf = 0;

	WL_TRACE(("%s %s: SIOCSIWFREQ\n", __FUNCTION__, dev->name));
/*   scan is not allowed in AP mode  */

	/* Setting by channel number */
	if (fwrq->e == 0 && fwrq->m < MAXCHANNEL) {
		chan = fwrq->m;
	}
	/* Setting by frequency */
	else {
		/* Convert to MHz as best we can */
		if (fwrq->e >= 6) {
			fwrq->e -= 6;
			while (fwrq->e--)
				fwrq->m *= 10;
		} else if (fwrq->e < 6) {
			while (fwrq->e++ < 6)
				fwrq->m /= 10;
		}
		/* handle 4.9GHz frequencies as Japan 4 GHz based channelization */
		if (fwrq->m > 4000 && fwrq->m < 5000)
			sf = WF_CHAN_FACTOR_4_G; /* start factor for 4 GHz */

		chan = wf_mhz2channel(fwrq->m, sf);
	}

	chan = htod32(chan);

	if ((error = dev_wlc_ioctl(dev, WLC_SET_CHANNEL, &chan, sizeof(chan))))
		return error;

	g_wl_iw_params.target_channel = chan;

	/* -EINPROGRESS: Call commit handler */
	return -EINPROGRESS;
}

static int
wl_iw_get_freq(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_freq *fwrq,
	char *extra
)
{
	channel_info_t ci;
	int error;

	WL_TRACE(("%s: SIOCGIWFREQ\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_CHANNEL, &ci, sizeof(ci))))
		return error;

	/* Return radio channel in channel form */
	fwrq->m = dtoh32(ci.hw_channel);
	fwrq->e = dtoh32(0);
	return 0;
}

static int
wl_iw_set_mode(
	struct net_device *dev,
	struct iw_request_info *info,
	__u32 *uwrq,
	char *extra
)
{
	int infra = 0, ap = 0, error = 0;

	WL_TRACE(("%s: SIOCSIWMODE\n", dev->name));

	switch (*uwrq) {
	case IW_MODE_MASTER:
		infra = ap = 1;
		break;
	case IW_MODE_ADHOC:
	case IW_MODE_AUTO:
		break;
	case IW_MODE_INFRA:
		infra = 1;
		break;
	default:
		return -EINVAL;
	}
	infra = htod32(infra);
	ap = htod32(ap);

	if ((error = dev_wlc_ioctl(dev, WLC_SET_INFRA, &infra, sizeof(infra))) ||
	    (error = dev_wlc_ioctl(dev, WLC_SET_AP, &ap, sizeof(ap))))
		return error;

	/* -EINPROGRESS: Call commit handler */
	return -EINPROGRESS;
}

static int
wl_iw_get_mode(
	struct net_device *dev,
	struct iw_request_info *info,
	__u32 *uwrq,
	char *extra
)
{
	int error, infra = 0, ap = 0;

	WL_TRACE(("%s: SIOCGIWMODE\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_INFRA, &infra, sizeof(infra))) ||
	    (error = dev_wlc_ioctl(dev, WLC_GET_AP, &ap, sizeof(ap))))
		return error;

	infra = dtoh32(infra);
	ap = dtoh32(ap);
	*uwrq = infra ? ap ? IW_MODE_MASTER : IW_MODE_INFRA : IW_MODE_ADHOC;

	return 0;
}

static int
wl_iw_get_range(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	struct iw_range *range = (struct iw_range *) extra;
	wl_uint32_list_t *list;
	wl_rateset_t rateset;
	int8 *channels;
	int error, i, k;
	uint sf, ch;

	int phytype;
	int bw_cap = 0, sgi_tx = 0, nmode = 0;
	channel_info_t ci;
	uint8 nrate_list2copy = 0;
	uint16 nrate_list[4][8] = { {13, 26, 39, 52, 78, 104, 117, 130},
		{14, 29, 43, 58, 87, 116, 130, 144},
		{27, 54, 81, 108, 162, 216, 243, 270},
		{30, 60, 90, 120, 180, 240, 270, 300}};

	WL_TRACE(("%s: SIOCGIWRANGE\n", dev->name));

	if (!extra)
		return -EINVAL;

	channels = kmalloc((MAXCHANNEL+1)*4, GFP_KERNEL);
	if (!channels) {
		WL_ERROR(("Could not alloc channels\n"));
		return -ENOMEM;
	}
	list = (wl_uint32_list_t *)channels;

	dwrq->length = sizeof(struct iw_range);
	memset(range, 0, sizeof(range));

	/* We don't use nwids */
	range->min_nwid = range->max_nwid = 0;

	/* Set available channels/frequencies */
	list->count = htod32(MAXCHANNEL);
	if ((error = dev_wlc_ioctl(dev, WLC_GET_VALID_CHANNELS, channels, (MAXCHANNEL+1)*4))) {
		kfree(channels);
		return error;
	}
	for (i = 0; i < dtoh32(list->count) && i < IW_MAX_FREQUENCIES; i++) {
		range->freq[i].i = dtoh32(list->element[i]);

		ch = dtoh32(list->element[i]);
		if (ch <= CH_MAX_2G_CHANNEL)
			sf = WF_CHAN_FACTOR_2_4_G;
		else
			sf = WF_CHAN_FACTOR_5_G;

		range->freq[i].m = wf_channel2mhz(ch, sf);
		range->freq[i].e = 6;
	}
	range->num_frequency = range->num_channels = i;

	/* Link quality (use NDIS cutoffs) */
	range->max_qual.qual = 5;
	/* Signal level (use RSSI) */
	range->max_qual.level = 0x100 - 200;	/* -200 dBm */
	/* Noise level (use noise) */
	range->max_qual.noise = 0x100 - 200;	/* -200 dBm */
	/* Signal level threshold range (?) */
	range->sensitivity = 65535;

#if WIRELESS_EXT > 11
	/* Link quality (use NDIS cutoffs) */
	range->avg_qual.qual = 3;
	/* Signal level (use RSSI) */
	range->avg_qual.level = 0x100 + WL_IW_RSSI_GOOD;
	/* Noise level (use noise) */
	range->avg_qual.noise = 0x100 - 75;	/* -75 dBm */
#endif /* WIRELESS_EXT > 11 */

	/* Set available bitrates */
	if ((error = dev_wlc_ioctl(dev, WLC_GET_CURR_RATESET, &rateset, sizeof(rateset)))) {
		kfree(channels);
		return error;
	}
	rateset.count = dtoh32(rateset.count);
	range->num_bitrates = rateset.count;
	for (i = 0; i < rateset.count && i < IW_MAX_BITRATES; i++)
		range->bitrate[i] = (rateset.rates[i]& 0x7f) * 500000; /* convert to bps */
	dev_wlc_intvar_get(dev, "nmode", &nmode);
	dev_wlc_ioctl(dev, WLC_GET_PHYTYPE, &phytype, sizeof(phytype));

	if (nmode == 1 && phytype == WLC_PHY_TYPE_SSN) {
		dev_wlc_intvar_get(dev, "mimo_bw_cap", &bw_cap);
		dev_wlc_intvar_get(dev, "sgi_tx", &sgi_tx);
		dev_wlc_ioctl(dev, WLC_GET_CHANNEL, &ci, sizeof(channel_info_t));
		ci.hw_channel = dtoh32(ci.hw_channel);

		if (bw_cap == 0 ||
			(bw_cap == 2 && ci.hw_channel <= 14)) {
			if (sgi_tx == 0)
				nrate_list2copy = 0;
			else
				nrate_list2copy = 1;
		}
		if (bw_cap == 1 ||
			(bw_cap == 2 && ci.hw_channel >= 36)) {
			if (sgi_tx == 0)
				nrate_list2copy = 2;
			else
				nrate_list2copy = 3;
		}
		range->num_bitrates += 8;
		for (k = 0; i < range->num_bitrates; k++, i++) {
			/* convert to bps */
			range->bitrate[i] = (nrate_list[nrate_list2copy][k]) * 500000;
		}
	}

	/* Set an indication of the max TCP throughput
	 * in bit/s that we can expect using this interface.
	 * May be use for QoS stuff... Jean II
	 */
	if ((error = dev_wlc_ioctl(dev, WLC_GET_PHYTYPE, &i, sizeof(i)))) {
		kfree(channels);
		return error;
	}
	i = dtoh32(i);
	if (i == WLC_PHY_TYPE_A)
		range->throughput = 24000000;	/* 24 Mbits/s */
	else
		range->throughput = 1500000;	/* 1.5 Mbits/s */

	/* RTS and fragmentation thresholds */
	range->min_rts = 0;
	range->max_rts = 2347;
	range->min_frag = 256;
	range->max_frag = 2346;

	range->max_encoding_tokens = DOT11_MAX_DEFAULT_KEYS;
	range->num_encoding_sizes = 4;
	range->encoding_size[0] = WEP1_KEY_SIZE;
	range->encoding_size[1] = WEP128_KEY_SIZE;
#if WIRELESS_EXT > 17
	range->encoding_size[2] = TKIP_KEY_SIZE;
#else
	range->encoding_size[2] = 0;
#endif
	range->encoding_size[3] = AES_KEY_SIZE;

	/* Do not support power micro-management */
	range->min_pmp = 0;
	range->max_pmp = 0;
	range->min_pmt = 0;
	range->max_pmt = 0;
	range->pmp_flags = 0;
	range->pm_capa = 0;

	/* Transmit Power - values are in mW */
	range->num_txpower = 2;
	range->txpower[0] = 1;
	range->txpower[1] = 255;
	range->txpower_capa = IW_TXPOW_MWATT;

#if WIRELESS_EXT > 10
	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 19;

	/* Only support retry limits */
	range->retry_capa = IW_RETRY_LIMIT;
	range->retry_flags = IW_RETRY_LIMIT;
	range->r_time_flags = 0;
	/* SRL and LRL limits */
	range->min_retry = 1;
	range->max_retry = 255;
	/* Retry lifetime limits unsupported */
	range->min_r_time = 0;
	range->max_r_time = 0;
#endif /* WIRELESS_EXT > 10 */

#if WIRELESS_EXT > 17
	range->enc_capa = IW_ENC_CAPA_WPA;
	range->enc_capa |= IW_ENC_CAPA_CIPHER_TKIP;
	range->enc_capa |= IW_ENC_CAPA_CIPHER_CCMP;
	range->enc_capa |= IW_ENC_CAPA_WPA2;

	/* Event capability (kernel) */
	IW_EVENT_CAPA_SET_KERNEL(range->event_capa);
	/* Event capability (driver) */
	IW_EVENT_CAPA_SET(range->event_capa, SIOCGIWAP);
	IW_EVENT_CAPA_SET(range->event_capa, SIOCGIWSCAN);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVTXDROP);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVMICHAELMICFAILURE);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVPMKIDCAND);
#endif /* WIRELESS_EXT > 17 */

	kfree(channels);

	return 0;
}

static int
rssi_to_qual(int rssi)
{
	if (rssi <= WL_IW_RSSI_NO_SIGNAL)
		return 0;
	else if (rssi <= WL_IW_RSSI_VERY_LOW)
		return 1;
	else if (rssi <= WL_IW_RSSI_LOW)
		return 2;
	else if (rssi <= WL_IW_RSSI_GOOD)
		return 3;
	else if (rssi <= WL_IW_RSSI_VERY_GOOD)
		return 4;
	else
		return 5;
}

static int
wl_iw_set_spy(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = NETDEV_PRIV(dev);
	struct sockaddr *addr = (struct sockaddr *) extra;
	int i;

	WL_TRACE(("%s: SIOCSIWSPY\n", dev->name));

	if (!extra)
		return -EINVAL;

	iw->spy_num = MIN(ARRAYSIZE(iw->spy_addr), dwrq->length);
	for (i = 0; i < iw->spy_num; i++)
		memcpy(&iw->spy_addr[i], addr[i].sa_data, ETHER_ADDR_LEN);
	memset(iw->spy_qual, 0, sizeof(iw->spy_qual));

	return 0;
}

static int
wl_iw_get_spy(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = NETDEV_PRIV(dev);
	struct sockaddr *addr = (struct sockaddr *) extra;
	struct iw_quality *qual = (struct iw_quality *) &addr[iw->spy_num];
	int i;

	WL_TRACE(("%s: SIOCGIWSPY\n", dev->name));

	if (!extra)
		return -EINVAL;

	dwrq->length = iw->spy_num;
	for (i = 0; i < iw->spy_num; i++) {
		memcpy(addr[i].sa_data, &iw->spy_addr[i], ETHER_ADDR_LEN);
		addr[i].sa_family = AF_UNIX;
		memcpy(&qual[i], &iw->spy_qual[i], sizeof(struct iw_quality));
		iw->spy_qual[i].updated = 0;
	}

	return 0;
}

/*
 *  Convert channel number to chanspec structure
*/
static int
wl_iw_ch_to_chanspec(int ch, wl_join_params_t *join_params, int *join_params_size)
{
	chanspec_t chanspec = 0;

	if (ch != 0) {
		/* Pass target channel information to avoid extra scan */
		join_params->params.chanspec_num = 1;
		join_params->params.chanspec_list[0] = ch;

		if (join_params->params.chanspec_list[0])
			chanspec |= WL_CHANSPEC_BAND_2G;
		else
			chanspec |= WL_CHANSPEC_BAND_5G;

		chanspec |= WL_CHANSPEC_BW_20;
		chanspec |= WL_CHANSPEC_CTL_SB_NONE;

		/* account for additional bssid and chanlist parameters */
		*join_params_size += WL_ASSOC_PARAMS_FIXED_SIZE +
			join_params->params.chanspec_num * sizeof(chanspec_t);

		/* update chanspec entries with target channel */
		join_params->params.chanspec_list[0]  &= WL_CHANSPEC_CHAN_MASK;
		join_params->params.chanspec_list[0] |= chanspec;
		join_params->params.chanspec_list[0] =
		        htodchanspec(join_params->params.chanspec_list[0]);

		join_params->params.chanspec_num = htod32(join_params->params.chanspec_num);

		WL_TRACE(("%s  join_params->params.chanspec_list[0]= %X\n",
			__FUNCTION__, join_params->params.chanspec_list[0]));
	}
	return 1;
}

static int
wl_iw_set_wap(
	struct net_device *dev,
	struct iw_request_info *info,
	struct sockaddr *awrq,
	char *extra
)
{
	int error = -EINVAL;
	wl_join_params_t join_params;
	int join_params_size;

	WL_TRACE(("%s: SIOCSIWAP\n", dev->name));

	if (awrq->sa_family != ARPHRD_ETHER) {
		WL_ERROR(("Invalid Header...sa_family\n"));
		return -EINVAL;
	}

	/* Ignore "auto" or "off" */
	if (ETHER_ISBCAST(awrq->sa_data) || ETHER_ISNULLADDR(awrq->sa_data)) {
		scb_val_t scbval;
		/* WL_ASSOC(("disassociating \n")); */
		bzero(&scbval, sizeof(scb_val_t));
		/* Ignore error (may be down or disassociated) */
		(void) dev_wlc_ioctl(dev, WLC_DISASSOC, &scbval, sizeof(scb_val_t));
		return 0;
	}


	/* Join with specific BSSID and cached SSID
	 *  If SSID is zero join based on BSSID only
	 */
	memset(&join_params, 0, sizeof(join_params));
	join_params_size = sizeof(join_params.ssid);

	memcpy(join_params.ssid.SSID, g_ssid.SSID, g_ssid.SSID_len);
	join_params.ssid.SSID_len = htod32(g_ssid.SSID_len);
	memcpy(&join_params.params.bssid, awrq->sa_data, ETHER_ADDR_LEN);

	/* join  to specific channel , use 0 for all  channels */
	/* g_wl_iw_params.target_channel = 0; */
	WL_TRACE(("%s  target_channel=%d\n", __FUNCTION__, g_wl_iw_params.target_channel));
	wl_iw_ch_to_chanspec(g_wl_iw_params.target_channel, &join_params, &join_params_size);

	if ((error = dev_wlc_ioctl(dev, WLC_SET_SSID, &join_params, join_params_size))) {
		WL_ERROR(("%s Invalid ioctl data=%d\n", __FUNCTION__, error));
		return error;
	}

	if (g_ssid.SSID_len) {
		WL_TRACE(("%s: join SSID=%s BSSID="MACSTR" ch=%d\n", __FUNCTION__,
			g_ssid.SSID, MAC2STR((u8 *)awrq->sa_data),
			g_wl_iw_params.target_channel));
	}

	/* Clean up cached SSID */
	memset(&g_ssid, 0, sizeof(g_ssid));
	return 0;
}

static int
wl_iw_get_wap(
	struct net_device *dev,
	struct iw_request_info *info,
	struct sockaddr *awrq,
	char *extra
)
{
	WL_TRACE(("%s: SIOCGIWAP\n", dev->name));

	awrq->sa_family = ARPHRD_ETHER;
	memset(awrq->sa_data, 0, ETHER_ADDR_LEN);

	/* Ignore error (may be down or disassociated) */
	(void) dev_wlc_ioctl(dev, WLC_GET_BSSID, awrq->sa_data, ETHER_ADDR_LEN);

	return 0;
}

#if WIRELESS_EXT > 17
static int
wl_iw_mlme(
	struct net_device *dev,
	struct iw_request_info *info,
	struct sockaddr *awrq,
	char *extra
)
{
	struct iw_mlme *mlme;
	scb_val_t scbval;
	int error  = -EINVAL;

	WL_TRACE(("%s: SIOCSIWMLME DISASSOC/DEAUTH\n", dev->name));

	mlme = (struct iw_mlme *)extra;
	if (mlme == NULL) {
		WL_ERROR(("Invalid ioctl data.\n"));
		return error;
	}

	scbval.val = mlme->reason_code;
	bcopy(&mlme->addr.sa_data, &scbval.ea, ETHER_ADDR_LEN);

	if (mlme->cmd == IW_MLME_DISASSOC) {
		scbval.val = htod32(scbval.val);
		error = dev_wlc_ioctl(dev, WLC_DISASSOC, &scbval, sizeof(scb_val_t));
	}
	else if (mlme->cmd == IW_MLME_DEAUTH) {
		scbval.val = htod32(scbval.val);
		error = dev_wlc_ioctl(dev, WLC_SCB_DEAUTHENTICATE_FOR_REASON, &scbval,
			sizeof(scb_val_t));
	}
	else {
		WL_ERROR(("Invalid ioctl data.\n"));
		return error;
	}

	return error;
}
#endif /* WIRELESS_EXT > 17 */

#ifndef WL_IW_USE_ISCAN
static int
wl_iw_get_aplist(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_scan_results_t *list;
	struct sockaddr *addr = (struct sockaddr *) extra;
	struct iw_quality qual[IW_MAX_AP];
	wl_bss_info_t *bi = NULL;
	int error, i;
	uint buflen = dwrq->length;

	WL_TRACE(("%s: SIOCGIWAPLIST\n", dev->name));

	if (!extra)
		return -EINVAL;

	/* Get scan results (too large to put on the stack) */
	list = kmalloc(buflen, GFP_KERNEL);
	if (!list)
		return -ENOMEM;
	memset(list, 0, buflen);
	list->buflen = htod32(buflen);
	if ((error = dev_wlc_ioctl(dev, WLC_SCAN_RESULTS, list, buflen))) {
		WL_ERROR(("%d: Scan results error %d\n", __LINE__, error));
		kfree(list);
		return error;
	}
	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);
	if (list->version != WL_BSS_INFO_VERSION) {
		WL_ERROR(("%s : list->version %d != WL_BSS_INFO_VERSION\n",
		          __FUNCTION__, list->version));
		kfree(list);
		return -EINVAL;
	}

	for (i = 0, dwrq->length = 0; i < list->count && dwrq->length < IW_MAX_AP; i++) {
		bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) : list->bss_info;
		ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list +
			buflen));

		/* Infrastructure only */
		if (!(dtoh16(bi->capability) & DOT11_CAP_ESS))
			continue;

		/* BSSID */
		memcpy(addr[dwrq->length].sa_data, &bi->BSSID, ETHER_ADDR_LEN);
		addr[dwrq->length].sa_family = ARPHRD_ETHER;
		qual[dwrq->length].qual = rssi_to_qual(dtoh16(bi->RSSI));
		qual[dwrq->length].level = 0x100 + dtoh16(bi->RSSI);
		qual[dwrq->length].noise = 0x100 + bi->phy_noise;

		/* Updated qual, level, and noise */
#if WIRELESS_EXT > 18
		qual[dwrq->length].updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
#else
		qual[dwrq->length].updated = 7;
#endif /* WIRELESS_EXT > 18 */

		dwrq->length++;
	}

	kfree(list);

	if (dwrq->length) {
		memcpy(&addr[dwrq->length], qual, sizeof(struct iw_quality) * dwrq->length);
		/* Provided qual */
		dwrq->flags = 1;
	}

	return 0;
}
#endif /* WL_IW_USE_ISCAN */

#ifdef WL_IW_USE_ISCAN
static int
wl_iw_iscan_get_aplist(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_scan_results_t *list;
	iscan_buf_t * buf;
	iscan_info_t *iscan = g_iscan;

	struct sockaddr *addr = (struct sockaddr *) extra;
	struct iw_quality qual[IW_MAX_AP];
	wl_bss_info_t *bi = NULL;
	int i;

	WL_TRACE(("%s: SIOCGIWAPLIST\n", dev->name));

	if (!extra)
		return -EINVAL;

	if ((!iscan) || (iscan->sysioc_pid < 0)) {
		WL_ERROR(("%s error\n", __FUNCTION__));
		return 0;
	}

	buf = iscan->list_hdr;
	/* Get scan results (too large to put on the stack) */
	while (buf) {
		list = &((wl_iscan_results_t*)buf->iscan_buf)->results;
		if (list->version != WL_BSS_INFO_VERSION) {
			WL_ERROR(("%s : list->version %d != WL_BSS_INFO_VERSION\n",
				__FUNCTION__, list->version));
			return -EINVAL;
		}

		bi = NULL;
		for (i = 0, dwrq->length = 0; i < list->count && dwrq->length < IW_MAX_AP; i++) {
			bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length))
			          : list->bss_info;
			ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list +
				WLC_IW_ISCAN_MAXLEN));

			/* Infrastructure only */
			if (!(dtoh16(bi->capability) & DOT11_CAP_ESS))
				continue;

			/* BSSID */
			memcpy(addr[dwrq->length].sa_data, &bi->BSSID, ETHER_ADDR_LEN);
			addr[dwrq->length].sa_family = ARPHRD_ETHER;
			qual[dwrq->length].qual = rssi_to_qual(dtoh16(bi->RSSI));
			qual[dwrq->length].level = 0x100 + dtoh16(bi->RSSI);
			qual[dwrq->length].noise = 0x100 + bi->phy_noise;

			/* Updated qual, level, and noise */
#if WIRELESS_EXT > 18
			qual[dwrq->length].updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
#else
			qual[dwrq->length].updated = 7;
#endif /* WIRELESS_EXT > 18 */

			dwrq->length++;
		}
		buf = buf->next;
	}
	if (dwrq->length) {
		memcpy(&addr[dwrq->length], qual, sizeof(struct iw_quality) * dwrq->length);
		/* Provided qual */
		dwrq->flags = 1;
	}

	return 0;
}

static int
wl_iw_iscan_prep(wl_scan_params_t *params, wlc_ssid_t *ssid)
{
	int err = 0;

	memcpy(&params->bssid, &ether_bcast, ETHER_ADDR_LEN);
	params->bss_type = DOT11_BSSTYPE_ANY;
	params->scan_type = 0;
	params->nprobes = -1;
	params->active_time = -1;
	params->passive_time = -1;
	params->home_time = -1;
	params->channel_num = 0;

	params->nprobes = htod32(params->nprobes);
	params->active_time = htod32(params->active_time);
	params->passive_time = htod32(params->passive_time);
	params->home_time = htod32(params->home_time);
	if (ssid && ssid->SSID_len)
		memcpy(&params->ssid, ssid, sizeof(wlc_ssid_t));

	return err;
}

static int
wl_iw_iscan(iscan_info_t *iscan, wlc_ssid_t *ssid, uint16 action)
{
	int err = 0;

	iscan->iscan_ex_params_p->version = htod32(ISCAN_REQ_VERSION);
	iscan->iscan_ex_params_p->action = htod16(action);
	iscan->iscan_ex_params_p->scan_duration = htod16(0);

	WL_SCAN(("%s : nprobes=%d\n", __FUNCTION__, iscan->iscan_ex_params_p->params.nprobes));
	WL_SCAN(("active_time=%d\n", iscan->iscan_ex_params_p->params.active_time));
	WL_SCAN(("passive_time=%d\n", iscan->iscan_ex_params_p->params.passive_time));
	WL_SCAN(("home_time=%d\n", iscan->iscan_ex_params_p->params.home_time));
	WL_SCAN(("scan_type=%d\n", iscan->iscan_ex_params_p->params.scan_type));
	WL_SCAN(("bss_type=%d\n", iscan->iscan_ex_params_p->params.bss_type));

	if ((dev_iw_iovar_setbuf(iscan->dev, "iscan", iscan->iscan_ex_params_p,
		iscan->iscan_ex_param_size, iscan->ioctlbuf, sizeof(iscan->ioctlbuf)))) {
			WL_ERROR(("Set ISCAN for %s failed with %d\n", __FUNCTION__, err));
			err = -1;
	}

	return err;
}

static void
wl_iw_timerfunc(ulong data)
{
	iscan_info_t *iscan = (iscan_info_t *)data;
	if (iscan) {
		iscan->timer_on = 0;
		if (iscan->iscan_state != ISCAN_STATE_IDLE) {
			WL_TRACE(("timer trigger\n"));
			up(&iscan->sysioc_sem);
		}
	}
}

static void
wl_iw_set_event_mask(struct net_device *dev)
{
	char eventmask[WL_EVENTING_MASK_LEN];
	char iovbuf[WL_EVENTING_MASK_LEN + 12];	/* Room for "event_msgs" + '\0' + bitvec */

	dev_iw_iovar_getbuf(dev, "event_msgs", "", 0, iovbuf, sizeof(iovbuf));
	bcopy(iovbuf, eventmask, WL_EVENTING_MASK_LEN);
	setbit(eventmask, WLC_E_SCAN_COMPLETE);
	dev_iw_iovar_setbuf(dev, "event_msgs", eventmask, WL_EVENTING_MASK_LEN,
		iovbuf, sizeof(iovbuf));
}

static uint32
wl_iw_iscan_get(iscan_info_t *iscan)
{
	iscan_buf_t * buf;
	iscan_buf_t * ptr;
	wl_iscan_results_t * list_buf;
	wl_iscan_results_t list;
	wl_scan_results_t *results;
	uint32 status;
	int res = 0;

	DHD_OS_MUTEX_LOCK(&wl_cache_lock);
	if (iscan->list_cur) {
		buf = iscan->list_cur;
		iscan->list_cur = buf->next;
	}
	else {
		buf = kmalloc(sizeof(iscan_buf_t), GFP_KERNEL);
		if (!buf) {
			WL_ERROR(("%s can't alloc iscan_buf_t : going to abort currect iscan\n",
			          __FUNCTION__));
			DHD_OS_MUTEX_UNLOCK(&wl_cache_lock);
			return WL_SCAN_RESULTS_NO_MEM;
		}
		buf->next = NULL;
		if (!iscan->list_hdr)
			iscan->list_hdr = buf;
		else {
			ptr = iscan->list_hdr;
			while (ptr->next) {
				ptr = ptr->next;
			}
			ptr->next = buf;
		}
	}
	memset(buf->iscan_buf, 0, WLC_IW_ISCAN_MAXLEN);
	list_buf = (wl_iscan_results_t*)buf->iscan_buf;
	results = &list_buf->results;
	results->buflen = WL_ISCAN_RESULTS_FIXED_SIZE;
	results->version = 0;
	results->count = 0;

	memset(&list, 0, sizeof(list));
	list.results.buflen = htod32(WLC_IW_ISCAN_MAXLEN);
	res = dev_iw_iovar_getbuf(
		iscan->dev,
		"iscanresults",
		&list,
		WL_ISCAN_RESULTS_FIXED_SIZE,
		buf->iscan_buf,
		WLC_IW_ISCAN_MAXLEN);
	if (res == 0) {
		results->buflen = dtoh32(results->buflen);
		results->version = dtoh32(results->version);
		results->count = dtoh32(results->count);
		WL_TRACE(("results->count = %d\n", results->count));
		WL_TRACE(("results->buflen = %d\n", results->buflen));
		status = dtoh32(list_buf->status);
	} else {
		WL_ERROR(("%s returns error %d\n", __FUNCTION__, res));
		/* TODO Add ISCAN abort call */
		status = WL_SCAN_RESULTS_NO_MEM;
	}
	DHD_OS_MUTEX_UNLOCK(&wl_cache_lock);
	return status;
}

static void
wl_iw_force_specific_scan(iscan_info_t *iscan)
{
	WL_TRACE(("%s force Specific SCAN for %s\n", __FUNCTION__, g_specific_ssid.SSID));
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	rtnl_lock();
#endif

	(void) dev_wlc_ioctl(iscan->dev, WLC_SCAN, &g_specific_ssid, sizeof(g_specific_ssid));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	rtnl_unlock();
#endif
}

static void
wl_iw_send_scan_complete(iscan_info_t *iscan)
{
#ifndef SANDGATE2G
	union iwreq_data wrqu;

	memset(&wrqu, 0, sizeof(wrqu));

	/* wext expects to get no data for SIOCGIWSCAN Event  */
	wireless_send_event(iscan->dev, SIOCGIWSCAN, &wrqu, NULL);
		WL_TRACE(("Send Event ISCAN complete\n"));
#endif /* SANDGATE2G */
}

static int
_iscan_sysioc_thread(void *data)
{
	uint32 status;
	iscan_info_t *iscan = (iscan_info_t *)data;
	static bool iscan_pass_abort = FALSE;

	DAEMONIZE("iscan_sysioc");

	status = WL_SCAN_RESULTS_PARTIAL;
	while (down_interruptible(&iscan->sysioc_sem) == 0) {

	net_os_wake_lock(iscan->dev);


		if (iscan->timer_on) {
			/* to prevent the timer from being deleted twice (concurrent case)
			 * as all code checks timer_on before deleting it, maybe a spin
			 * lock is needed.
			 */
			iscan->timer_on = 0;
			del_timer_sync(&iscan->timer);
		}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
		rtnl_lock();
#endif
		status = wl_iw_iscan_get(iscan);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
		rtnl_unlock();
#endif

	if  (g_scan_specified_ssid && (iscan_pass_abort == TRUE)) {
		WL_TRACE(("%s Get results from specific scan status=%d\n", __FUNCTION__, status));
			wl_iw_send_scan_complete(iscan);
			iscan_pass_abort = FALSE;
			status  = -1;
		}

		switch (status) {
			case WL_SCAN_RESULTS_PARTIAL:
				WL_TRACE(("iscanresults incomplete\n"));
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
				rtnl_lock();
#endif
				/* make sure our buffer size is enough before going next round */
				wl_iw_iscan(iscan, NULL, WL_SCAN_ACTION_CONTINUE);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
				rtnl_unlock();
#endif
				/* Reschedule the timer */
				mod_timer(&iscan->timer, jiffies + iscan->timer_ms*HZ/1000);
				iscan->timer_on = 1;
				break;
			case WL_SCAN_RESULTS_SUCCESS:
				WL_TRACE(("iscanresults complete\n"));
				iscan->iscan_state = ISCAN_STATE_IDLE;
				wl_iw_send_scan_complete(iscan);
				break;
			case WL_SCAN_RESULTS_PENDING:
				WL_TRACE(("iscanresults pending\n"));
				/* Reschedule the timer */
				mod_timer(&iscan->timer, jiffies + iscan->timer_ms*HZ/1000);
				iscan->timer_on = 1;
				break;
			case WL_SCAN_RESULTS_ABORTED:
				WL_TRACE(("iscanresults aborted\n"));
				iscan->iscan_state = ISCAN_STATE_IDLE;
				if (g_scan_specified_ssid == 0)
					wl_iw_send_scan_complete(iscan);
				else {
					iscan_pass_abort = TRUE;
					wl_iw_force_specific_scan(iscan);
				}
				break;
			case WL_SCAN_RESULTS_NO_MEM:
				WL_TRACE(("iscanresults can't alloc memory: skip\n"));
				iscan->iscan_state = ISCAN_STATE_IDLE;
				break;
			default:
				WL_TRACE(("iscanresults returned unknown status %d\n", status));
				break;
		 }

		net_os_wake_unlock(iscan->dev);
	}

	if (iscan->timer_on) {
		iscan->timer_on = 0;
		del_timer_sync(&iscan->timer);
	}

	complete_and_exit(&iscan->sysioc_exited, 0);
}
#endif /* #ifndef WL_IW_USE_ISCAN */


static int
wl_iw_set_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
)
{
	int error;
	WL_TRACE(("\n:%s dev:%s: SIOCSIWSCAN : SCAN\n", __FUNCTION__, dev->name));

#ifdef OEM_CHROMIUMOS
	g_set_essid_before_scan = FALSE;
#endif

#if defined(CSCAN)
		WL_ERROR(("%s: Scan from SIOCGIWSCAN not supported\n", __FUNCTION__));
		return -EINVAL;
#endif /* defined(CSCAN) */


	/* Android may send scan request even before START if finished
	     Ignoring scan request when chip is still off
	*/
	if (g_onoff == G_WLAN_SET_OFF)
		return 0;

	/* Broadcast scan */
	memset(&g_specific_ssid, 0, sizeof(g_specific_ssid));
#ifndef WL_IW_USE_ISCAN
	/* with ISCAN scan used only for specific scan */
	g_scan_specified_ssid = 0;
#endif /* WL_IW_USE_ISCAN */

#if WIRELESS_EXT > 17
	/* check for given essid */
	if (wrqu->data.length == sizeof(struct iw_scan_req)) {
		if (wrqu->data.flags & IW_SCAN_THIS_ESSID) {
			struct iw_scan_req *req = (struct iw_scan_req *)extra;
			if (g_scan_specified_ssid) {
				WL_TRACE(("%s Specific SCAN is not done ignore scan for = %s \n",
					__FUNCTION__, req->essid));
				/* TODO : queue SP scan and restart specific scan later */
				return -EBUSY;
			}
			else {
				g_specific_ssid.SSID_len = MIN(sizeof(g_specific_ssid.SSID),
				                               req->essid_len);
				memcpy(g_specific_ssid.SSID, req->essid, g_specific_ssid.SSID_len);
				g_specific_ssid.SSID_len = htod32(g_specific_ssid.SSID_len);
				g_scan_specified_ssid = 1;
				WL_TRACE(("### Specific scan ssid=%s len=%d\n",
				          g_specific_ssid.SSID, g_specific_ssid.SSID_len));
			}
		}
	}
#endif /* WIRELESS_EXT > 17 */
	/* Ignore error (most likely scan in progress) */
	if ((error = dev_wlc_ioctl(dev, WLC_SCAN, &g_specific_ssid, sizeof(g_specific_ssid)))) {
		WL_TRACE(("#### Set SCAN for %s failed with %d\n", g_specific_ssid.SSID, error));
		/* if scan failed clean g_scan_specified_ssid flag */
		g_scan_specified_ssid = 0;
		return -EBUSY;
	}

	return 0;
}

#ifdef WL_IW_USE_ISCAN
int
wl_iw_iscan_set_scan_broadcast_prep(struct net_device *dev, uint flag)
{
	wlc_ssid_t ssid;
	iscan_info_t *iscan = g_iscan;


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	if (flag)
		rtnl_lock();
#endif

	wl_iw_set_event_mask(dev);

	WL_TRACE(("+++: Set Broadcast ISCAN\n"));
	/* default Broadcast scan */
	memset(&ssid, 0, sizeof(ssid));

	iscan->list_cur = iscan->list_hdr;
	iscan->iscan_state = ISCAN_STATE_SCANING;

	memset(&iscan->iscan_ex_params_p->params, 0, iscan->iscan_ex_param_size);
	wl_iw_iscan_prep(&iscan->iscan_ex_params_p->params, &ssid);
	wl_iw_iscan(iscan, &ssid, WL_SCAN_ACTION_START);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	if (flag)
		rtnl_unlock();
#endif

	mod_timer(&iscan->timer, jiffies + iscan->timer_ms*HZ/1000);

	iscan->timer_on = 1;

	return 0;
}

static int
wl_iw_iscan_set_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
)
{
	wlc_ssid_t ssid;
	iscan_info_t *iscan = g_iscan;
	int ret = 0; /* Lin - for releasing wake lock properly */

	WL_TRACE(("%s: SIOCSIWSCAN : ISCAN\n", dev->name));

#if defined(CSCAN)
		WL_ERROR(("%s: Scan from SIOCGIWSCAN not supported\n", __FUNCTION__));
		return -EINVAL;
#endif /* defined(CSCAN) */

	net_os_wake_lock(dev);

	/* IN SOFTAP mode , scan must never be performed  */
	/* Android may send scan request even before START if finished
	     Ignoring scan request when chip is still off
	*/
	if (g_onoff == G_WLAN_SET_OFF) {
		WL_TRACE(("%s: driver is not up yet after START\n", __FUNCTION__));
		goto set_scan_end;
	}

#ifdef PNO_SUPPORT
	/* maybe better ignore scan request when PNO still active */
	if  (dhd_dev_get_pno_status(dev)) {
		WL_ERROR(("%s: Scan called when PNO is active\n", __FUNCTION__));
	}
#endif /* PNO_SUPPORT */

	/* use backup if our thread is not successful */
	if ((!iscan) || (iscan->sysioc_pid < 0)) {
		ret = wl_iw_set_scan(dev, info, wrqu, extra);
		goto set_scan_end;
	}

	if (g_scan_specified_ssid) {
		WL_TRACE(("%s Specific SCAN already running ignoring BC scan\n",
		          __FUNCTION__));
		ret = EBUSY;
		goto set_scan_end;
	}

	/* clean Broadcast scan */
	memset(&ssid, 0, sizeof(ssid));

#if WIRELESS_EXT > 17
	/* check for given essid */
	if (wrqu->data.length == sizeof(struct iw_scan_req)) {
		if (wrqu->data.flags & IW_SCAN_THIS_ESSID) {
			struct iw_scan_req *req = (struct iw_scan_req *)extra;
			/* moved to the end, see down below */
			ssid.SSID_len = MIN(sizeof(ssid.SSID), req->essid_len);
			memcpy(ssid.SSID, req->essid, ssid.SSID_len);
			ssid.SSID_len = htod32(ssid.SSID_len);
		}
		else {
			g_scan_specified_ssid = 0;

			if (iscan->iscan_state == ISCAN_STATE_SCANING) {
				WL_TRACE(("%s ISCAN already in progress \n", __FUNCTION__));
				goto set_scan_end;
			}
		}
	}
#endif /* WIRELESS_EXT > 17 */


	wl_iw_iscan_set_scan_broadcast_prep(dev, 0);

set_scan_end:
	net_os_wake_unlock(dev);
	return ret;
}
#endif /* WL_IW_USE_ISCAN */

#if WIRELESS_EXT > 17
static bool
ie_is_wpa_ie(uint8 **wpaie, uint8 **tlvs, int *tlvs_len)
{
/* Is this body of this tlvs entry a WPA entry? If */
/* not update the tlvs buffer pointer/length */
	uint8 *ie = *wpaie;

	/* If the contents match the WPA_OUI and type=1 */
	if ((ie[1] >= 6) &&
		!bcmp((const void *)&ie[2], (const void *)(WPA_OUI "\x01"), 4)) {
		return TRUE;
	}

	/* point to the next ie */
	ie += ie[1] + 2;
	/* calculate the length of the rest of the buffer */
	*tlvs_len -= (int)(ie - *tlvs);
	/* update the pointer to the start of the buffer */
	*tlvs = ie;
	return FALSE;
}

static bool
ie_is_wps_ie(uint8 **wpsie, uint8 **tlvs, int *tlvs_len)
{
/* Is this body of this tlvs entry a WPS entry? If */
/* not update the tlvs buffer pointer/length */
	uint8 *ie = *wpsie;

	/* If the contents match the WPA_OUI and type=4 */
	if ((ie[1] >= 4) &&
		!bcmp((const void *)&ie[2], (const void *)(WPA_OUI "\x04"), 4)) {
		return TRUE;
	}

	/* point to the next ie */
	ie += ie[1] + 2;
	/* calculate the length of the rest of the buffer */
	*tlvs_len -= (int)(ie - *tlvs);
	/* update the pointer to the start of the buffer */
	*tlvs = ie;
	return FALSE;
}
#endif /* WIRELESS_EXT > 17 */


static int
wl_iw_handle_scanresults_ies(char **event_p, char *end,
	struct iw_request_info *info, wl_bss_info_t *bi)
{
#if WIRELESS_EXT > 17
	struct iw_event	iwe;
	char *event;

	event = *event_p;
	if (bi->ie_length) {
		/* look for wpa/rsn ies in the ie list... */
		bcm_tlv_t *ie;
		uint8 *ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		int ptr_len = bi->ie_length;

		if ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_RSN_ID))) {
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = ie->len + 2;
			event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)ie);
		}
		ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);

		while ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_WPA_ID))) {
			/* look for WPS IE */
			if (ie_is_wps_ie(((uint8 **)&ie), &ptr, &ptr_len)) {
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = ie->len + 2;
				event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)ie);
				break;
			}
		}

		ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		ptr_len = bi->ie_length;
		while ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_WPA_ID))) {
			if (ie_is_wpa_ie(((uint8 **)&ie), &ptr, &ptr_len)) {
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = ie->len + 2;
				event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)ie);
				break;
			}
		}

	*event_p = event;
	}
#endif /* WIRELESS_EXT > 17 */

	return 0;
}

static uint
wl_iw_get_scan_prep(
	wl_scan_results_t *list,
	struct iw_request_info *info,
	char *extra,
	short max_size)
{
	int  i, j;
	struct iw_event  iwe;
	wl_bss_info_t *bi = NULL;
	char *event = extra, *end = extra + max_size - WE_ADD_EVENT_FIX, *value;
	int	ret = 0;

	ASSERT(list);

	/* End pointer (*end) for any IWE_STREAM_ADD_EVENT should be
	 * corrected by WE_ADD_EVENT_FIX because kernel iw_handler is
	 * checking end round up to specific event_len fields only
	 */

	for (i = 0; i < list->count && i < IW_MAX_AP; i++) {
		if (list->version != WL_BSS_INFO_VERSION) {
			WL_ERROR(("%s : list->version %d != WL_BSS_INFO_VERSION\n",
			          __FUNCTION__, list->version));
			return ret;
		}

		bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) : list->bss_info;

		WL_TRACE(("%s : %s\n", __FUNCTION__, bi->SSID));

		/* First entry must be the BSSID */
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
		memcpy(iwe.u.ap_addr.sa_data, &bi->BSSID, ETHER_ADDR_LEN);
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_ADDR_LEN);
		/* SSID */
		iwe.u.data.length = dtoh32(bi->SSID_len);
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.flags = 1;
		event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, bi->SSID);

		/* Mode */
		if (dtoh16(bi->capability) & (DOT11_CAP_ESS | DOT11_CAP_IBSS)) {
			iwe.cmd = SIOCGIWMODE;
			if (dtoh16(bi->capability) & DOT11_CAP_ESS)
				iwe.u.mode = IW_MODE_INFRA;
			else
				iwe.u.mode = IW_MODE_ADHOC;
			event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_UINT_LEN);
		}

		/* Channel */
		iwe.cmd = SIOCGIWFREQ;
		iwe.u.freq.m = wf_channel2mhz(CHSPEC_CHANNEL(bi->chanspec),
			CHSPEC_CHANNEL(bi->chanspec) <= CH_MAX_2G_CHANNEL ?
			WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
		iwe.u.freq.e = 6;
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_FREQ_LEN);

		/* Channel quality */
		iwe.cmd = IWEVQUAL;
		iwe.u.qual.qual = rssi_to_qual(dtoh16(bi->RSSI));
		iwe.u.qual.level = 0x100 + dtoh16(bi->RSSI);
		iwe.u.qual.noise = 0x100 + bi->phy_noise;
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_QUAL_LEN);

		/* WPA, WPA2, WPS, WAPI IEs */
		 wl_iw_handle_scanresults_ies(&event, end, info, bi);

		/* Encryption */
		iwe.cmd = SIOCGIWENCODE;
		if (dtoh16(bi->capability) & DOT11_CAP_PRIVACY)
			iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		else
			iwe.u.data.flags = IW_ENCODE_DISABLED;
		iwe.u.data.length = 0;
		event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)event);

		/* Rates */
		if (bi->rateset.count) {
			if (((event -extra) + IW_EV_LCP_LEN) <= (uintptr)end) {
				value = event + IW_EV_LCP_LEN;
				iwe.cmd = SIOCGIWRATE;
				/* Those two flags are ignored... */
				iwe.u.bitrate.fixed = iwe.u.bitrate.disabled = 0;
				for (j = 0; j < bi->rateset.count && j < IW_MAX_BITRATES; j++) {
					iwe.u.bitrate.value =
						(bi->rateset.rates[j] & 0x7f) * 500000;
					value = IWE_STREAM_ADD_VALUE(info, event, value, end, &iwe,
						IW_EV_PARAM_LEN);
				}
				event = value;
			}
		}
	}

	if ((ret = (event - extra)) < 0) {
		WL_ERROR(("==> Wrong size\n"));
		ret = 0;
	}

	WL_TRACE(("%s: size=%d bytes prepared \n", __FUNCTION__, (unsigned int)(event - extra)));
	return (uint)ret;
}

static int
wl_iw_get_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	channel_info_t ci;
	wl_scan_results_t *list_merge;
	wl_scan_results_t *list = (wl_scan_results_t *) g_scan;
	int error;
	uint buflen_from_user = dwrq->length;
	uint len =  G_SCAN_RESULTS;
	__u16 len_ret = 0;
#if defined(WL_IW_USE_ISCAN)
	iscan_info_t *iscan = g_iscan;
	iscan_buf_t * p_buf;
#endif /* (WL_IW_USE_ISCAN) */

	WL_TRACE(("%s: buflen_from_user %d: \n", dev->name, buflen_from_user));

	if (!extra) {
		WL_TRACE(("%s: wl_iw_get_scan return -EINVAL\n", dev->name));
		return -EINVAL;
	}

	/* Check for scan in progress */
	if ((error = dev_wlc_ioctl(dev, WLC_GET_CHANNEL, &ci, sizeof(ci))))
		return error;
	ci.scan_channel = dtoh32(ci.scan_channel);
	if (ci.scan_channel)
		return -EAGAIN;



	/* For specific SSID allocate local buffer */
	if (g_scan_specified_ssid) {
		/* Overwrite list as pointer for local buf instead of global g_scan */
		list = kmalloc(len, GFP_KERNEL);
		if (!list) {
			WL_TRACE(("%s: wl_iw_get_scan return -ENOMEM\n", dev->name));
			g_scan_specified_ssid = 0;
			return -ENOMEM;
		}
	}

	memset(list, 0, len);
	list->buflen = htod32(len);
	if ((error = dev_wlc_ioctl(dev, WLC_SCAN_RESULTS, list, len))) {
		WL_ERROR(("%s: %s : Scan_results ERROR %d\n", dev->name, __FUNCTION__, error));
		dwrq->length = len;
		if (g_scan_specified_ssid) {
			g_scan_specified_ssid = 0;
			kfree(list);
		}
		return 0;
	}
	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);

	/* check if scan results are valid and exit if not */
	if (list->version != WL_BSS_INFO_VERSION) {
		WL_ERROR(("%s : list->version %d != WL_BSS_INFO_VERSION\n",
		          __FUNCTION__, list->version));
		if (g_scan_specified_ssid) {
			g_scan_specified_ssid = 0;
			kfree(list);
		}
		return -EINVAL;
	}



	/* Merge results from specific SSID with results from Broadcast SSID */
	if (g_scan_specified_ssid) {
		WL_TRACE(("%s: Specified scan APs in the list =%d\n", __FUNCTION__, list->count));
		len_ret = (__u16) wl_iw_get_scan_prep(list, info, extra, buflen_from_user);
		kfree(list);

#if defined(WL_IW_USE_ISCAN)
		p_buf = iscan->list_hdr;
		/* Get scan results */
		while (p_buf != iscan->list_cur) {
			list_merge = &((wl_iscan_results_t*)p_buf->iscan_buf)->results;
			WL_TRACE(("%s: Bcast APs list=%d\n", __FUNCTION__, list_merge->count));
			if (list_merge->count > 0)
				len_ret += (__u16) wl_iw_get_scan_prep(list_merge, info,
				    extra+len_ret, buflen_from_user -len_ret);
			p_buf = p_buf->next;
		}
#else
		list_merge = (wl_scan_results_t *) g_scan;
		WL_TRACE(("%s: Bcast APs list=%d\n", __FUNCTION__, list_merge->count));
		if (list_merge->count > 0)
			len_ret += (__u16) wl_iw_get_scan_prep(list_merge, info, extra+len_ret,
				buflen_from_user -len_ret);
#endif /* WL_IW_USE_ISCAN */
	}
	else {
		list = (wl_scan_results_t *) g_scan;
		len_ret = (__u16) wl_iw_get_scan_prep(list, info, extra, buflen_from_user);
	}

#if defined(WL_IW_USE_ISCAN)
	/* Clean up when specific scan results retrived */
	g_scan_specified_ssid = 0;
#endif /* WL_IW_USE_ISCAN */
	/* Force WPA_SUPPLICANT to make another scan request if
	 */
	if ((len_ret + WE_ADD_EVENT_FIX) < buflen_from_user)
		len = len_ret;

	dwrq->length = len;
	dwrq->flags = 0;	/* todo */

	WL_TRACE(("%s return to WE %d bytes APs=%d\n", __FUNCTION__, dwrq->length, list->count));
	return 0;
}

#if defined(WL_IW_USE_ISCAN)
static int
wl_iw_iscan_get_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_scan_results_t *list;
	struct iw_event	iwe;
	wl_bss_info_t *bi = NULL;
	int ii, j;
	int apcnt;
	char *event = extra, *end = extra + dwrq->length, *value;
	iscan_info_t *iscan = g_iscan;
	iscan_buf_t * p_buf;
	uint32  counter = 0;
	uint8   channel;

	WL_TRACE(("%s %s buflen_from_user %d:\n", dev->name, __FUNCTION__, dwrq->length));


	if (!extra) {
		WL_TRACE(("%s: INVALID SIOCGIWSCAN GET bad parameter\n", dev->name));
		return -EINVAL;
	}

	/* use backup if our thread is not successful */
	if ((!iscan) || (iscan->sysioc_pid < 0)) {
		WL_ERROR(("%ssysioc_pid\n", __FUNCTION__));
		return wl_iw_get_scan(dev, info, dwrq, extra);
	}

	/* ANDROID wpa_supplicant has a bug in that it only try to get the result twice
	   and then give up, disregard of our return -EAGAIN. What's worse, it usually
	   set the scan to passive, so only make things worse
	*/
	/* Check for scan in progress */
	if (iscan->iscan_state == ISCAN_STATE_SCANING) {
		WL_TRACE(("%s: SIOCGIWSCAN GET still scanning\n", dev->name));
		return -EAGAIN;
	}


	WL_TRACE(("%s: SIOCGIWSCAN GET broadcast results\n", dev->name));
	apcnt = 0;
	p_buf = iscan->list_hdr;
	/* Get scan results */
	while (p_buf != iscan->list_cur) {
		list = &((wl_iscan_results_t*)p_buf->iscan_buf)->results;

		counter += list->count;

		if (list->version != WL_BSS_INFO_VERSION) {
			WL_ERROR(("%s : list->version %d != WL_BSS_INFO_VERSION\n",
			          __FUNCTION__, list->version));
			return -EINVAL;
		}

		bi = NULL;
		for (ii = 0; ii < list->count && apcnt < IW_MAX_AP; apcnt++, ii++) {
			bi = (bi ?
			      (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) :
			      list->bss_info);
			ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list +
			                                              WLC_IW_ISCAN_MAXLEN));

			/* overflow check cover fields before wpa IEs */
			if (event + ETHER_ADDR_LEN + bi->SSID_len +
			    IW_EV_UINT_LEN + IW_EV_FREQ_LEN + IW_EV_QUAL_LEN >= end)
				return -E2BIG;
			/* First entry must be the BSSID */
			iwe.cmd = SIOCGIWAP;
			iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
			memcpy(iwe.u.ap_addr.sa_data, &bi->BSSID, ETHER_ADDR_LEN);
			event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_ADDR_LEN);

			/* SSID */
			iwe.u.data.length = dtoh32(bi->SSID_len);
			iwe.cmd = SIOCGIWESSID;
			iwe.u.data.flags = 1;
			event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, bi->SSID);

			/* Mode */
			if (dtoh16(bi->capability) & (DOT11_CAP_ESS | DOT11_CAP_IBSS)) {
				iwe.cmd = SIOCGIWMODE;
				if (dtoh16(bi->capability) & DOT11_CAP_ESS)
					iwe.u.mode = IW_MODE_INFRA;
				else
					iwe.u.mode = IW_MODE_ADHOC;
				event = IWE_STREAM_ADD_EVENT(info, event, end,
				                             &iwe, IW_EV_UINT_LEN);
			}

			/* Channel */
			iwe.cmd = SIOCGIWFREQ;
			channel = (bi->ctl_ch == 0) ? CHSPEC_CHANNEL(bi->chanspec) : bi->ctl_ch;
			iwe.u.freq.m = wf_channel2mhz(channel,
			                              channel <= CH_MAX_2G_CHANNEL ?
			                              WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
			iwe.u.freq.e = 6;
			event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_FREQ_LEN);

			/* Channel quality */
			iwe.cmd = IWEVQUAL;
			iwe.u.qual.qual = rssi_to_qual(dtoh16(bi->RSSI));
			iwe.u.qual.level = 0x100 + dtoh16(bi->RSSI);
			iwe.u.qual.noise = 0x100 + bi->phy_noise;
			event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_QUAL_LEN);

			/* WPA, WPA2, WPS, WAPI IEs */
			wl_iw_handle_scanresults_ies(&event, end, info, bi);

			/* Encryption */
			iwe.cmd = SIOCGIWENCODE;
			if (dtoh16(bi->capability) & DOT11_CAP_PRIVACY)
				iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
			else
				iwe.u.data.flags = IW_ENCODE_DISABLED;
			iwe.u.data.length = 0;
			event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)event);

			/* Rates */
			if (bi->rateset.count) {
				if (event + IW_MAX_BITRATES*IW_EV_PARAM_LEN >= end)
					return -E2BIG;

				value = event + IW_EV_LCP_LEN;
				iwe.cmd = SIOCGIWRATE;
				/* Those two flags are ignored... */
				iwe.u.bitrate.fixed = iwe.u.bitrate.disabled = 0;
				for (j = 0; j < bi->rateset.count && j < IW_MAX_BITRATES; j++) {
					iwe.u.bitrate.value =
					        (bi->rateset.rates[j] & 0x7f) * 500000;
					value = IWE_STREAM_ADD_VALUE(info, event, value, end, &iwe,
					                             IW_EV_PARAM_LEN);
				}
				event = value;
			}
		}
		p_buf = p_buf->next;
	} /* while (p_buf) */

	dwrq->length = event - extra;
	dwrq->flags = 0;	/* todo */


	WL_TRACE(("%s return to WE %d bytes APs=%d\n", __FUNCTION__, dwrq->length, counter));

	/* if get scan was called too fast (aka iwlist) results maybe empty hence return again */
	if (!dwrq->length)
		return -EAGAIN;

	return 0;
}
#endif /* WIRELESS_EXT > 13 */

#define WL_JOIN_PARAMS_MAX 1600
#ifdef CONFIG_PRESCANNED
static int
check_prescan(wl_join_params_t *join_params, int *join_params_size)
{
	int cnt = 0;
	int indx = 0;
	wl_iw_ss_cache_t *node = NULL;
	wl_bss_info_t *bi = NULL;
	iscan_info_t *iscan = g_iscan;
	iscan_buf_t * buf;
	wl_scan_results_t *list;
	char *destbuf;

	buf = iscan->list_hdr;

	while (buf) {
		list = &((wl_iscan_results_t*)buf->iscan_buf)->results;
		bi = NULL;
		for (indx = 0;  indx < list->count; indx++) {
			bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length))
				: list->bss_info;
			if (!(dtoh16(bi->capability) & DOT11_CAP_ESS))
				continue;
			if ((dtoh32(bi->SSID_len) != join_params->ssid.SSID_len) ||
				memcmp(bi->SSID, join_params->ssid.SSID,
				join_params->ssid.SSID_len))
				continue;
			memcpy(&join_params->params.chanspec_list[cnt],
				&bi->chanspec, sizeof(chanspec_t));
			WL_ERROR(("iscan : chanspec :%d, count %d \n", bi->chanspec, cnt));
			cnt++;
		}
		buf = buf->next;
	}

	if (!cnt) {
		MUTEX_LOCK_WL_SCAN_SET();
		node = g_ss_cache_ctrl.m_cache_head;
		for (; node; ) {
			if (!memcmp(&node->bss_info->SSID, join_params->ssid.SSID,
				join_params->ssid.SSID_len)) {
				memcpy(&join_params->params.chanspec_list[cnt],
					&node->bss_info->chanspec, sizeof(chanspec_t));
				WL_ERROR(("cache_scan : chanspec :%d, count %d \n",
				(int)node->bss_info->chanspec, cnt));
				cnt++;
			}
			node = node->next;
		}
		MUTEX_UNLOCK_WL_SCAN_SET();
	}

	if (!cnt) {
		return 0;
	}

	destbuf = (char *)&join_params->params.chanspec_list[cnt];
	*join_params_size = destbuf - (char*)join_params;
	join_params->ssid.SSID_len = htod32(g_ssid.SSID_len);
	memcpy(&(join_params->params.bssid), &ether_bcast, ETHER_ADDR_LEN);
	join_params->params.chanspec_num = htod32(cnt);

	if ((*join_params_size) > WL_JOIN_PARAMS_MAX) {
		WL_ERROR(("can't fit bssids for all %d APs found\n", cnt));
			kfree(join_params);
		return 0;
	}

	WL_ERROR(("Passing %d channel/bssid pairs.\n", cnt));
	return cnt;
}
#endif /* CONFIG_PRESCANNED */

static int
wl_iw_set_essid(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	int error;
	wl_join_params_t *join_params;
	int join_params_size;

	WL_TRACE(("%s: SIOCSIWESSID\n", dev->name));

	RETURN_IF_EXTRA_NULL(extra);

#ifdef OEM_CHROMIUMOS
	if (g_set_essid_before_scan)
		return -EAGAIN;
#endif
	if (!(join_params = kmalloc(WL_JOIN_PARAMS_MAX, GFP_KERNEL))) {
		WL_ERROR(("allocation failed for join_params size is %d\n", WL_JOIN_PARAMS_MAX));
		return -ENOMEM;
	}

	memset(join_params, 0, WL_JOIN_PARAMS_MAX);

	/* clean cached ssid value */
	memset(&g_ssid, 0, sizeof(g_ssid));

	if (dwrq->length && extra) {
#if WIRELESS_EXT > 20
		g_ssid.SSID_len = MIN(sizeof(g_ssid.SSID), dwrq->length);
#else
		g_ssid.SSID_len = MIN(sizeof(g_ssid.SSID), dwrq->length-1);
#endif
		memcpy(g_ssid.SSID, extra, g_ssid.SSID_len);

#ifdef CONFIG_PRESCANNED
		memcpy(join_params->ssid.SSID, g_ssid.SSID, g_ssid.SSID_len);
		join_params->ssid.SSID_len = g_ssid.SSID_len;

		if (check_prescan(join_params, &join_params_size)) {
			if ((error = dev_wlc_ioctl(dev, WLC_SET_SSID,
				join_params, join_params_size))) {
				WL_ERROR(("Invalid ioctl data=%d\n", error));
				kfree(join_params);
				return error;
			}
			kfree(join_params);
			return 0;
		} else {
			WL_ERROR(("No matched found\n Trying to join to specific channel\n"));
		}
#endif /* CONFIG_PRESCANNED */
	} else {
		/* Broadcast SSID */
		g_ssid.SSID_len = 0;
	}
	g_ssid.SSID_len = htod32(g_ssid.SSID_len);

	/* join parameters starts with the ssid */
	memset(join_params, 0, sizeof(join_params));
	join_params_size = sizeof(join_params->ssid);

	memcpy(join_params->ssid.SSID, g_ssid.SSID, g_ssid.SSID_len);
	join_params->ssid.SSID_len = htod32(g_ssid.SSID_len);
	memcpy(&(join_params->params.bssid), &ether_bcast, ETHER_ADDR_LEN);

	/* join  to specific channel , use 0 for all  channels */
	/* g_wl_iw_params.target_channel = 0; */
	wl_iw_ch_to_chanspec(g_wl_iw_params.target_channel, join_params, &join_params_size);

	if ((error = dev_wlc_ioctl(dev, WLC_SET_SSID, join_params, join_params_size))) {
		WL_ERROR(("Invalid ioctl data=%d\n", error));
		return error;
	}

	if (g_ssid.SSID_len) {
		WL_ERROR(("%s: join SSID=%s ch=%d\n", __FUNCTION__,
			g_ssid.SSID,  g_wl_iw_params.target_channel));
	}
	kfree(join_params);
	return 0;
}

static int
wl_iw_get_essid(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wlc_ssid_t ssid;
	int error;

	WL_TRACE(("%s: SIOCGIWESSID\n", dev->name));

	if (!extra)
		return -EINVAL;

	if ((error = dev_wlc_ioctl(dev, WLC_GET_SSID, &ssid, sizeof(ssid)))) {
		WL_ERROR(("Error getting the SSID\n"));
		return error;
	}

	ssid.SSID_len = dtoh32(ssid.SSID_len);

	/* Get the current SSID */
	memcpy(extra, ssid.SSID, ssid.SSID_len);

	dwrq->length = ssid.SSID_len;

	dwrq->flags = 1; /* active */

	return 0;
}

static int
wl_iw_set_nick(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = NETDEV_PRIV(dev);

	WL_TRACE(("%s: SIOCSIWNICKN\n", dev->name));

	if (!extra)
		return -EINVAL;

	/* Check the size of the string */
	if (dwrq->length > sizeof(iw->nickname))
		return -E2BIG;

	memcpy(iw->nickname, extra, dwrq->length);
	iw->nickname[dwrq->length - 1] = '\0';

	return 0;
}

static int
wl_iw_get_nick(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = NETDEV_PRIV(dev);

	WL_TRACE(("%s: SIOCGIWNICKN\n", dev->name));

	if (!extra)
		return -EINVAL;

	strcpy(extra, iw->nickname);
	dwrq->length = strlen(extra) + 1;

	return 0;
}

static int
wl_iw_set_rate(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	wl_rateset_t rateset;
	int error, rate, i, error_bg, error_a;

	WL_TRACE(("%s: SIOCSIWRATE\n", dev->name));

	/* Get current rateset */
	if ((error = dev_wlc_ioctl(dev, WLC_GET_CURR_RATESET, &rateset, sizeof(rateset))))
		return error;

	rateset.count = dtoh32(rateset.count);

	if (vwrq->value < 0) {
		/* Select maximum rate */
		rate = rateset.rates[rateset.count - 1] & 0x7f;
	} else if (vwrq->value < rateset.count) {
		/* Select rate by rateset index */
		rate = rateset.rates[vwrq->value] & 0x7f;
	} else {
		/* Specified rate in bps */
		rate = vwrq->value / 500000;
	}

	if (vwrq->fixed) {
		/*
			Set rate override,
			Since the is a/b/g-blind, both a/bg_rate are enforced.
		*/
		error_bg = dev_wlc_intvar_set(dev, "bg_rate", rate);
		error_a = dev_wlc_intvar_set(dev, "a_rate", rate);

		if (error_bg && error_a)
			return (error_bg | error_a);
	} else {
		/*
			clear rate override
			Since the is a/b/g-blind, both a/bg_rate are enforced.
		*/
		/* 0 is for clearing rate override */
		error_bg = dev_wlc_intvar_set(dev, "bg_rate", 0);
		/* 0 is for clearing rate override */
		error_a = dev_wlc_intvar_set(dev, "a_rate", 0);

		if (error_bg && error_a)
			return (error_bg | error_a);

		/* Remove rates above selected rate */
		for (i = 0; i < rateset.count; i++)
			if ((rateset.rates[i] & 0x7f) > rate)
				break;
		rateset.count = htod32(i);

		/* Set current rateset */
		if ((error = dev_wlc_ioctl(dev, WLC_SET_RATESET, &rateset, sizeof(rateset))))
			return error;
	}

	return 0;
}

static int
wl_iw_get_rate(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, rate;

	WL_TRACE(("%s: SIOCGIWRATE\n", dev->name));

	/* Report the current tx rate */
	if ((error = dev_wlc_ioctl(dev, WLC_GET_RATE, &rate, sizeof(rate))))
		return error;
	rate = dtoh32(rate);
	vwrq->value = rate * 500000;

	return 0;
}

static int
wl_iw_set_rts(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, rts;

	WL_TRACE(("%s: SIOCSIWRTS\n", dev->name));

	if (vwrq->disabled)
		rts = DOT11_DEFAULT_RTS_LEN;
	else if (vwrq->value < 0 || vwrq->value > DOT11_DEFAULT_RTS_LEN)
		return -EINVAL;
	else
		rts = vwrq->value;

	if ((error = dev_wlc_intvar_set(dev, "rtsthresh", rts)))
		return error;

	return 0;
}

static int
wl_iw_get_rts(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, rts;

	WL_TRACE(("%s: SIOCGIWRTS\n", dev->name));

	if ((error = dev_wlc_intvar_get(dev, "rtsthresh", &rts)))
		return error;

	vwrq->value = rts;
	vwrq->disabled = (rts >= DOT11_DEFAULT_RTS_LEN);
	vwrq->fixed = 1;

	return 0;
}

static int
wl_iw_set_frag(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, frag;

	WL_TRACE(("%s: SIOCSIWFRAG\n", dev->name));

	if (vwrq->disabled)
		frag = DOT11_DEFAULT_FRAG_LEN;
	else if (vwrq->value < 0 || vwrq->value > DOT11_DEFAULT_FRAG_LEN)
		return -EINVAL;
	else
		frag = vwrq->value;

	if ((error = dev_wlc_intvar_set(dev, "fragthresh", frag)))
		return error;

	return 0;
}

static int
wl_iw_get_frag(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, fragthreshold;

	WL_TRACE(("%s: SIOCGIWFRAG\n", dev->name));

	if ((error = dev_wlc_intvar_get(dev, "fragthresh", &fragthreshold)))
		return error;

	vwrq->value = fragthreshold;
	vwrq->disabled = (fragthreshold >= DOT11_DEFAULT_FRAG_LEN);
	vwrq->fixed = 1;

	return 0;
}

static int
wl_iw_set_txpow(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, disable;
	uint16 txpwrmw;
	WL_TRACE(("%s: SIOCSIWTXPOW\n", dev->name));

	/* Make sure radio is off or on as far as software is concerned */
	disable = vwrq->disabled ? WL_RADIO_SW_DISABLE : 0;
	disable += WL_RADIO_SW_DISABLE << 16;

	disable = htod32(disable);
	if ((error = dev_wlc_ioctl(dev, WLC_SET_RADIO, &disable, sizeof(disable))))
		return error;

	/* If Radio is off, nothing more to do */
	if (disable & WL_RADIO_SW_DISABLE)
		return 0;

	/* Only handle mW */
	if (!(vwrq->flags & IW_TXPOW_MWATT))
		return -EINVAL;

	/* Value < 0 means just "on" or "off" */
	if (vwrq->value < 0)
		return 0;

	if (vwrq->value > 0xffff) txpwrmw = 0xffff;
	else txpwrmw = (uint16)vwrq->value;


	error = dev_wlc_intvar_set(dev, "qtxpower", (int)(bcm_mw_to_qdbm(txpwrmw)));
	return error;
}

static int
wl_iw_get_txpow(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, disable, txpwrdbm;
	uint8 result;

	WL_TRACE(("%s: SIOCGIWTXPOW\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_RADIO, &disable, sizeof(disable))) ||
	    (error = dev_wlc_intvar_get(dev, "qtxpower", &txpwrdbm)))
		return error;

	disable = dtoh32(disable);
	result = (uint8)(txpwrdbm & ~WL_TXPWR_OVERRIDE);
	vwrq->value = (int32)bcm_qdbm_to_mw(result);
	vwrq->fixed = 0;
	vwrq->disabled = (disable & (WL_RADIO_SW_DISABLE | WL_RADIO_HW_DISABLE)) ? 1 : 0;
	vwrq->flags = IW_TXPOW_MWATT;

	return 0;
}

#if WIRELESS_EXT > 10
static int
wl_iw_set_retry(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, lrl, srl;

	WL_TRACE(("%s: SIOCSIWRETRY\n", dev->name));

	/* Do not handle "off" or "lifetime" */
	if (vwrq->disabled || (vwrq->flags & IW_RETRY_LIFETIME))
		return -EINVAL;

	/* Handle "[min|max] limit" */
	if (vwrq->flags & IW_RETRY_LIMIT) {

		/* "max limit" or just "limit" */
#if WIRELESS_EXT > 20
	if ((vwrq->flags & IW_RETRY_LONG) ||(vwrq->flags & IW_RETRY_MAX) ||
		!((vwrq->flags & IW_RETRY_SHORT) || (vwrq->flags & IW_RETRY_MIN))) {
#else
	if ((vwrq->flags & IW_RETRY_MAX) || !(vwrq->flags & IW_RETRY_MIN)) {
#endif /* WIRELESS_EXT > 20 */
			lrl = htod32(vwrq->value);
			if ((error = dev_wlc_ioctl(dev, WLC_SET_LRL, &lrl, sizeof(lrl))))
				return error;
		}

		/* "min limit" or just "limit" */
#if WIRELESS_EXT > 20
	if ((vwrq->flags & IW_RETRY_SHORT) ||(vwrq->flags & IW_RETRY_MIN) ||
		!((vwrq->flags & IW_RETRY_LONG) || (vwrq->flags & IW_RETRY_MAX))) {
#else
		if ((vwrq->flags & IW_RETRY_MIN) || !(vwrq->flags & IW_RETRY_MAX)) {
#endif /* WIRELESS_EXT > 20 */
			srl = htod32(vwrq->value);
			if ((error = dev_wlc_ioctl(dev, WLC_SET_SRL, &srl, sizeof(srl))))
				return error;
		}
	}
	return 0;
}

static int
wl_iw_get_retry(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, lrl, srl;

	WL_TRACE(("%s: SIOCGIWRETRY\n", dev->name));

	vwrq->disabled = 0;      /* Can't be disabled */

	/* Do not handle lifetime queries */
	if ((vwrq->flags & IW_RETRY_TYPE) == IW_RETRY_LIFETIME)
		return -EINVAL;

	/* Get retry limits */
	if ((error = dev_wlc_ioctl(dev, WLC_GET_LRL, &lrl, sizeof(lrl))) ||
	    (error = dev_wlc_ioctl(dev, WLC_GET_SRL, &srl, sizeof(srl))))
		return error;

	lrl = dtoh32(lrl);
	srl = dtoh32(srl);

	/* Note : by default, display the min retry number */
	if (vwrq->flags & IW_RETRY_MAX) {
		vwrq->flags = IW_RETRY_LIMIT | IW_RETRY_MAX;
		vwrq->value = lrl;
	} else {
		vwrq->flags = IW_RETRY_LIMIT;
		vwrq->value = srl;
		if (srl != lrl)
			vwrq->flags |= IW_RETRY_MIN;
	}

	return 0;
}
#endif /* WIRELESS_EXT > 10 */

static int
wl_iw_set_encode(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_wsec_key_t key;
	int error, val, wsec;

	WL_TRACE(("%s: SIOCSIWENCODE index %d, len %d, flags %04x (%s%s%s%s%s)\n",
		dev->name, dwrq->flags & IW_ENCODE_INDEX, dwrq->length, dwrq->flags,
		dwrq->flags & IW_ENCODE_NOKEY ? "NOKEY" : "",
		dwrq->flags & IW_ENCODE_DISABLED ? " DISABLED" : "",
		dwrq->flags & IW_ENCODE_RESTRICTED ? " RESTRICTED" : "",
		dwrq->flags & IW_ENCODE_OPEN ? " OPEN" : "",
		dwrq->flags & IW_ENCODE_TEMP ? " TEMP" : ""));

	memset(&key, 0, sizeof(key));

	if ((dwrq->flags & IW_ENCODE_INDEX) == 0) {
		/* Find the current key */
		for (key.index = 0; key.index < DOT11_MAX_DEFAULT_KEYS; key.index++) {
			val = htod32(key.index);
			if ((error = dev_wlc_ioctl(dev, WLC_GET_KEY_PRIMARY, &val, sizeof(val))))
				return error;
			val = dtoh32(val);
			if (val)
				break;
		}
		/* Default to 0 */
		if (key.index == DOT11_MAX_DEFAULT_KEYS)
			key.index = 0;
	} else {
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;
		if (key.index >= DOT11_MAX_DEFAULT_KEYS)
			return -EINVAL;
	}

	/* Old API used to pass a NULL pointer instead of IW_ENCODE_NOKEY */
	if (!extra || !dwrq->length || (dwrq->flags & IW_ENCODE_NOKEY)) {
		/* Just select a new current key */
		val = htod32(key.index);
		if ((error = dev_wlc_ioctl(dev, WLC_SET_KEY_PRIMARY, &val, sizeof(val))))
			return error;
	} else {
		key.len = dwrq->length;

		if (dwrq->length > sizeof(key.data))
			return -EINVAL;

		memcpy(key.data, extra, dwrq->length);

		key.flags = WL_PRIMARY_KEY;
		switch (key.len) {
		case WEP1_KEY_SIZE:
			key.algo = CRYPTO_ALGO_WEP1;
			break;
		case WEP128_KEY_SIZE:
			key.algo = CRYPTO_ALGO_WEP128;
			break;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14)
		case TKIP_KEY_SIZE:
			key.algo = CRYPTO_ALGO_TKIP;
			break;
#endif
		case AES_KEY_SIZE:
			key.algo = CRYPTO_ALGO_AES_CCM;
			break;
		default:
			return -EINVAL;
		}

		/* Set the new key/index */
		swap_key_from_BE(&key);
		if ((error = dev_wlc_ioctl(dev, WLC_SET_KEY, &key, sizeof(key))))
			return error;
	}

	/* Interpret "off" to mean no encryption */
	val = (dwrq->flags & IW_ENCODE_DISABLED) ? 0 : WEP_ENABLED;

	if ((error = dev_wlc_intvar_get(dev, "wsec", &wsec)))
		return error;

	wsec  &= ~(WEP_ENABLED);
	wsec |= val;

	if ((error = dev_wlc_intvar_set(dev, "wsec", wsec)))
		return error;

	/* Interpret "restricted" to mean shared key authentication */
	val = (dwrq->flags & IW_ENCODE_RESTRICTED) ? 1 : 0;
	val = htod32(val);
	if ((error = dev_wlc_ioctl(dev, WLC_SET_AUTH, &val, sizeof(val))))
		return error;

	return 0;
}

static int
wl_iw_get_encode(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_wsec_key_t key;
	int error, val, wsec, auth;

	WL_TRACE(("%s: SIOCGIWENCODE\n", dev->name));

	/* assure default values of zero for things we don't touch */
	bzero(&key, sizeof(wl_wsec_key_t));

	if ((dwrq->flags & IW_ENCODE_INDEX) == 0) {
		/* Find the current key */
		for (key.index = 0; key.index < DOT11_MAX_DEFAULT_KEYS; key.index++) {
			val = key.index;
			if ((error = dev_wlc_ioctl(dev, WLC_GET_KEY_PRIMARY, &val, sizeof(val))))
				return error;
			val = dtoh32(val);
			if (val)
				break;
		}
	} else
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;

	if (key.index >= DOT11_MAX_DEFAULT_KEYS)
		key.index = 0;

	/* Get info */

	if ((error = dev_wlc_ioctl(dev, WLC_GET_WSEC, &wsec, sizeof(wsec))) ||
	    (error = dev_wlc_ioctl(dev, WLC_GET_AUTH, &auth, sizeof(auth))))
		return error;

	swap_key_to_BE(&key);

	wsec = dtoh32(wsec);
	auth = dtoh32(auth);
	/* Get key length */
	dwrq->length = MIN(DOT11_MAX_KEY_SIZE, key.len);

	/* Get flags */
	dwrq->flags = key.index + 1;
	if (!(wsec & (WEP_ENABLED | TKIP_ENABLED | AES_ENABLED))) {
		/* Interpret "off" to mean no encryption */
		dwrq->flags |= IW_ENCODE_DISABLED;
	}
	if (auth) {
		/* Interpret "restricted" to mean shared key authentication */
		dwrq->flags |= IW_ENCODE_RESTRICTED;
	}

	/* Get key */
	if (dwrq->length && extra)
		memcpy(extra, key.data, dwrq->length);

	return 0;
}

static int
wl_iw_set_power(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, pm;

	WL_TRACE(("%s: SIOCSIWPOWER\n", dev->name));

	pm = vwrq->disabled ? PM_OFF : PM_MAX;

	pm = htod32(pm);
	if ((error = dev_wlc_ioctl(dev, WLC_SET_PM, &pm, sizeof(pm))))
		return error;

	return 0;
}

static int
wl_iw_get_power(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, pm;

	WL_TRACE(("%s: SIOCGIWPOWER\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_PM, &pm, sizeof(pm))))
		return error;

	pm = dtoh32(pm);
	vwrq->disabled = pm ? 0 : 1;
	vwrq->flags = IW_POWER_ALL_R;

	return 0;
}

#if WIRELESS_EXT > 17
static int
wl_iw_set_wpaie(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *iwp,
	char *extra
)
{

	WL_TRACE(("%s: SIOCSIWGENIE\n", dev->name));

	RETURN_IF_EXTRA_NULL(extra);

#ifdef DHD_DEBUG
	{
		int i;

		for (i = 0; i < iwp->length; i++)
			WL_TRACE(("%02X ", extra[i]));
		WL_TRACE(("\n"));
	}
#endif

		dev_wlc_bufvar_set(dev, "wpaie", extra, iwp->length);

	return 0;
}

static int
wl_iw_get_wpaie(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *iwp,
	char *extra
)
{
	WL_TRACE(("%s: SIOCGIWGENIE\n", dev->name));
	iwp->length = 64;
	dev_wlc_bufvar_get(dev, "wpaie", extra, iwp->length);
	return 0;
}

static int
wl_iw_set_encodeext(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_wsec_key_t key;
	int error;
	struct iw_encode_ext *iwe;

	WL_TRACE(("%s: SIOCSIWENCODEEXT\n", dev->name));

	RETURN_IF_EXTRA_NULL(extra);

	memset(&key, 0, sizeof(key));
	iwe = (struct iw_encode_ext *)extra;

	/* disable encryption completely  */
	if (dwrq->flags & IW_ENCODE_DISABLED) {

	}

	/* get the key index */
	key.index = 0;
	if (dwrq->flags & IW_ENCODE_INDEX)
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;

	key.len = iwe->key_len;

	/* Instead of bcast for ea address for default wep keys, driver needs it to be Null */
	if (!ETHER_ISMULTI(iwe->addr.sa_data))
		bcopy((void *)&iwe->addr.sa_data, (char *)&key.ea, ETHER_ADDR_LEN);

	/* check for key index change */
	if (key.len == 0) {
		if (iwe->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
			WL_WSEC(("Changing the the primary Key to %d\n", key.index));
			/* change the key index .... */
			key.index = htod32(key.index);
			error = dev_wlc_ioctl(dev, WLC_SET_KEY_PRIMARY,
				&key.index, sizeof(key.index));
			if (error)
				return error;
		}
		/* key delete */
		else {
			swap_key_from_BE(&key);
			dev_wlc_ioctl(dev, WLC_SET_KEY, &key, sizeof(key));
		}
	}
	else {
		if (iwe->key_len > sizeof(key.data))
			return -EINVAL;

		WL_WSEC(("Setting the key index %d\n", key.index));
		if (iwe->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
			WL_WSEC(("key is a Primary Key\n"));
			key.flags = WL_PRIMARY_KEY;
		}

		bcopy((void *)iwe->key, key.data, iwe->key_len);

		if (iwe->alg == IW_ENCODE_ALG_TKIP) {
			uint8 keybuf[8];
			bcopy(&key.data[24], keybuf, sizeof(keybuf));
			bcopy(&key.data[16], &key.data[24], sizeof(keybuf));
			bcopy(keybuf, &key.data[16], sizeof(keybuf));
		}

		/* rx iv */
		if (iwe->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID) {
			uchar *ivptr;
			ivptr = (uchar *)iwe->rx_seq;
			key.rxiv.hi = (ivptr[5] << 24) | (ivptr[4] << 16) |
				(ivptr[3] << 8) | ivptr[2];
			key.rxiv.lo = (ivptr[1] << 8) | ivptr[0];
			key.iv_initialized = TRUE;
		}

		switch (iwe->alg) {
			case IW_ENCODE_ALG_NONE:
				key.algo = CRYPTO_ALGO_OFF;
				break;
			case IW_ENCODE_ALG_WEP:
				if (iwe->key_len == WEP1_KEY_SIZE)
					key.algo = CRYPTO_ALGO_WEP1;
				else
					key.algo = CRYPTO_ALGO_WEP128;
				break;
			case IW_ENCODE_ALG_TKIP:
				key.algo = CRYPTO_ALGO_TKIP;
				break;
			case IW_ENCODE_ALG_CCMP:
				key.algo = CRYPTO_ALGO_AES_CCM;
				break;
			default:
				break;
		}
		swap_key_from_BE(&key);

		dhd_wait_pend8021x(dev);

		error = dev_wlc_ioctl(dev, WLC_SET_KEY, &key, sizeof(key));
		if (error)
			return error;
	}
	return 0;
}

#if WIRELESS_EXT > 17
struct {
	pmkid_list_t pmkids;
	pmkid_t foo[MAXPMKID-1];
} pmkid_list;

static int
wl_iw_set_pmksa(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	struct iw_pmksa *iwpmksa;
	uint i;
	int ret = 0;
	char eabuf[ETHER_ADDR_STR_LEN];

	WL_WSEC(("%s: SIOCSIWPMKSA\n", dev->name));

	RETURN_IF_EXTRA_NULL(extra);

	iwpmksa = (struct iw_pmksa *)extra;
	bzero((char *)eabuf, ETHER_ADDR_STR_LEN);

	if (iwpmksa->cmd == IW_PMKSA_FLUSH) {
		WL_WSEC(("wl_iw_set_pmksa - IW_PMKSA_FLUSH\n"));
		bzero((char *)&pmkid_list, sizeof(pmkid_list));
	}

	else if (iwpmksa->cmd == IW_PMKSA_REMOVE) {
		{
			pmkid_list_t pmkid, *pmkidptr;
			uint j;
			pmkidptr = &pmkid;

			bcopy(&iwpmksa->bssid.sa_data[0], &pmkidptr->pmkid[0].BSSID,
				ETHER_ADDR_LEN);
			bcopy(&iwpmksa->pmkid[0], &pmkidptr->pmkid[0].PMKID, WPA2_PMKID_LEN);

			WL_WSEC(("wl_iw_set_pmksa,IW_PMKSA_REMOVE - PMKID: %s = ",
				bcm_ether_ntoa(&pmkidptr->pmkid[0].BSSID,
				eabuf)));
			for (j = 0; j < WPA2_PMKID_LEN; j++)
				WL_WSEC(("%02x ", pmkidptr->pmkid[0].PMKID[j]));
			WL_WSEC(("\n"));
		}

		for (i = 0; i < pmkid_list.pmkids.npmkid; i++)
			if (!bcmp(&iwpmksa->bssid.sa_data[0], &pmkid_list.pmkids.pmkid[i].BSSID,
				ETHER_ADDR_LEN))
				break;

		if ((pmkid_list.pmkids.npmkid > 0) && (i < pmkid_list.pmkids.npmkid)) {
			bzero(&pmkid_list.pmkids.pmkid[i], sizeof(pmkid_t));
			for (; i < (pmkid_list.pmkids.npmkid - 1); i++) {
				bcopy(&pmkid_list.pmkids.pmkid[i+1].BSSID,
					&pmkid_list.pmkids.pmkid[i].BSSID,
					ETHER_ADDR_LEN);
				bcopy(&pmkid_list.pmkids.pmkid[i+1].PMKID,
					&pmkid_list.pmkids.pmkid[i].PMKID,
					WPA2_PMKID_LEN);
			}
			pmkid_list.pmkids.npmkid--;
		}
		else
			ret = -EINVAL;
	}

	else if (iwpmksa->cmd == IW_PMKSA_ADD) {
		for (i = 0; i < pmkid_list.pmkids.npmkid; i++)
			if (!bcmp(&iwpmksa->bssid.sa_data[0], &pmkid_list.pmkids.pmkid[i].BSSID,
				ETHER_ADDR_LEN))
				break;
		if (i < MAXPMKID) {
			bcopy(&iwpmksa->bssid.sa_data[0],
				&pmkid_list.pmkids.pmkid[i].BSSID,
				ETHER_ADDR_LEN);
			bcopy(&iwpmksa->pmkid[0], &pmkid_list.pmkids.pmkid[i].PMKID,
				WPA2_PMKID_LEN);
			if (i == pmkid_list.pmkids.npmkid)
				pmkid_list.pmkids.npmkid++;
		}
		else
			ret = -EINVAL;

		{
			uint j;
			uint k;
			k = pmkid_list.pmkids.npmkid;
			WL_WSEC(("wl_iw_set_pmksa,IW_PMKSA_ADD - PMKID: %s = ",
				bcm_ether_ntoa(&pmkid_list.pmkids.pmkid[k].BSSID,
				eabuf)));
			for (j = 0; j < WPA2_PMKID_LEN; j++)
				WL_WSEC(("%02x ", pmkid_list.pmkids.pmkid[k].PMKID[j]));
			WL_WSEC(("\n"));
		}
	}
	WL_WSEC(("PRINTING pmkid LIST - No of elements %d", pmkid_list.pmkids.npmkid));
	for (i = 0; i < pmkid_list.pmkids.npmkid; i++) {
		uint j;
		WL_WSEC(("\nPMKID[%d]: %s = ", i,
			bcm_ether_ntoa(&pmkid_list.pmkids.pmkid[i].BSSID,
			eabuf)));
		for (j = 0; j < WPA2_PMKID_LEN; j++)
			WL_WSEC(("%02x ", pmkid_list.pmkids.pmkid[i].PMKID[j]));
	}
	WL_WSEC(("\n"));

	if (!ret)
		ret = dev_wlc_bufvar_set(dev, "pmkid_info", (char *)&pmkid_list,
			sizeof(pmkid_list));
	return ret;
}
#endif /* WIRELESS_EXT > 17 */

static int
wl_iw_get_encodeext(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	WL_TRACE(("%s: SIOCGIWENCODEEXT\n", dev->name));
	return 0;
}

/* Create wsec for wpaauth from saved privacy invoked, pairwise and group ciphers */
static uint32
wl_iw_create_wpaauth_wsec(struct net_device *dev)
{
	wl_iw_t *iw = NETDEV_PRIV(dev);
	uint32 wsec;

	/* Create initial wsec from pairwise cipher */
	if (iw->pcipher & (IW_AUTH_CIPHER_WEP40 | IW_AUTH_CIPHER_WEP104))
		wsec = WEP_ENABLED;
	else if (iw->pcipher & IW_AUTH_CIPHER_TKIP)
		wsec = TKIP_ENABLED;
	else if (iw->pcipher & IW_AUTH_CIPHER_CCMP)
		wsec = AES_ENABLED;
	else
		wsec = 0;

	/* Add into wsec the group cipher */
	if (iw->gcipher & (IW_AUTH_CIPHER_WEP40 | IW_AUTH_CIPHER_WEP104))
		wsec |= WEP_ENABLED;
	else if (iw->gcipher & IW_AUTH_CIPHER_TKIP)
		wsec |= TKIP_ENABLED;
	else if (iw->gcipher & IW_AUTH_CIPHER_CCMP)
		wsec |= AES_ENABLED;

	/* If the pairwise and group cipher did not result in a (non zero) wsec
	   value then if privacy invoked has been set then we set WSEC to
	   WEP.  This is the wlc API to create an association with the
	   privacy capability bit set.
	 */
	if (wsec == 0 && iw->privacy_invoked)
		wsec = WEP_ENABLED;

	WL_INFORM(("%s: returning wsec of %d\n", __FUNCTION__, wsec));

	return wsec;
}

static int
wl_iw_set_wpaauth(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error = 0;
	int paramid;
	int paramval;
	int val = 0;
	wl_iw_t *iw = NETDEV_PRIV(dev);

	paramid = vwrq->flags & IW_AUTH_INDEX;
	paramval = vwrq->value;

	WL_TRACE(("%s: SIOCSIWAUTH, %s(%d), paramval = 0x%0x\n",
		dev->name,
		paramid == IW_AUTH_WPA_VERSION ? "IW_AUTH_WPA_VERSION" :
		paramid == IW_AUTH_CIPHER_PAIRWISE ? "IW_AUTH_CIPHER_PAIRWISE" :
		paramid == IW_AUTH_CIPHER_GROUP ? "IW_AUTH_CIPHER_GROUP" :
		paramid == IW_AUTH_KEY_MGMT ? "IW_AUTH_KEY_MGMT" :
		paramid == IW_AUTH_TKIP_COUNTERMEASURES ? "IW_AUTH_TKIP_COUNTERMEASURES" :
		paramid == IW_AUTH_DROP_UNENCRYPTED ? "IW_AUTH_DROP_UNENCRYPTED" :
		paramid == IW_AUTH_80211_AUTH_ALG ? "IW_AUTH_80211_AUTH_ALG" :
		paramid == IW_AUTH_WPA_ENABLED ? "IW_AUTH_WPA_ENABLED" :
		paramid == IW_AUTH_RX_UNENCRYPTED_EAPOL ? "IW_AUTH_RX_UNENCRYPTED_EAPOL" :
		paramid == IW_AUTH_ROAMING_CONTROL ? "IW_AUTH_ROAMING_CONTROL" :
		paramid == IW_AUTH_PRIVACY_INVOKED ? "IW_AUTH_PRIVACY_INVOKED" :
		"UNKNOWN",
		paramid, paramval));


	switch (paramid) {
	case IW_AUTH_WPA_VERSION:
		/* supported wpa version disabled or wpa or wpa2 */
		iw->wpaversion = paramval;
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
		iw->pcipher = paramval;
		val = wl_iw_create_wpaauth_wsec(dev);
		if ((error = dev_wlc_intvar_set(dev, "wsec", val)))
			return error;
		break;

	case IW_AUTH_CIPHER_GROUP:
		iw->gcipher = paramval;
		val = wl_iw_create_wpaauth_wsec(dev);
		if ((error = dev_wlc_intvar_set(dev, "wsec", val)))
			return error;
		break;

	case IW_AUTH_KEY_MGMT:
		if (paramval & IW_AUTH_KEY_MGMT_PSK) {
			if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA)
				val = WPA_AUTH_PSK;
			else if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA2)
				val = WPA2_AUTH_PSK;
			else /* IW_AUTH_WPA_VERSION_DISABLED */
				val = WPA_AUTH_DISABLED;
		} else if (paramval & IW_AUTH_KEY_MGMT_802_1X) {
			if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA)
				val = WPA_AUTH_UNSPECIFIED;
			else if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA2)
				val = WPA2_AUTH_UNSPECIFIED;
			else /* IW_AUTH_WPA_VERSION_DISABLED */
				val = WPA_AUTH_DISABLED;
		}
		else
			val = WPA_AUTH_DISABLED;

		WL_INFORM(("%s: %d: setting wpa_auth to %d\n", __FUNCTION__, __LINE__, val));
		if ((error = dev_wlc_intvar_set(dev, "wpa_auth", val)))
			return error;
		break;

	case IW_AUTH_TKIP_COUNTERMEASURES:
		dev_wlc_bufvar_set(dev, "tkip_countermeasures", (char *)&paramval, 1);
		break;

	case IW_AUTH_80211_AUTH_ALG:
		/* open shared */
		WL_INFORM(("Setting the D11auth %d\n", paramval));
		if (paramval == IW_AUTH_ALG_OPEN_SYSTEM)
			val = 0;
		else if (paramval == IW_AUTH_ALG_SHARED_KEY)
			val = 1;
		else if (paramval == (IW_AUTH_ALG_OPEN_SYSTEM | IW_AUTH_ALG_SHARED_KEY))
			val = 2;
		else
			error = 1;
		if (!error && (error = dev_wlc_intvar_set(dev, "auth", val)))
			return error;
		break;

	case IW_AUTH_WPA_ENABLED:
		if (paramval == 0) {
			iw->privacy_invoked = 0; /* FALSE */
			iw->pcipher = 0;
			iw->gcipher = 0;
			val = wl_iw_create_wpaauth_wsec(dev);
			if ((error = dev_wlc_intvar_set(dev, "wsec", val)))
				return error;
			WL_INFORM(("%s: %d: setting wpa_auth to %d, wsec to %d\n",
				__FUNCTION__, __LINE__, paramval, val));
			dev_wlc_intvar_set(dev, "wpa_auth", paramval);
			return error;
		}

		/* nothing really needs to be done if wpa_auth enabled */
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		if ((error = dev_wlc_intvar_set(dev, "wsec_restrict", paramval)))
			return error;
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		dev_wlc_bufvar_set(dev, "rx_unencrypted_eapol", (char *)&paramval, 1);
		break;

#if WIRELESS_EXT > 17
	case IW_AUTH_ROAMING_CONTROL:
		WL_INFORM(("%s: IW_AUTH_ROAMING_CONTROL\n", __FUNCTION__));
		/* driver control or user space app control */
		break;

	case IW_AUTH_PRIVACY_INVOKED:
		iw->privacy_invoked = paramval;
		val = wl_iw_create_wpaauth_wsec(dev);
		if ((error = dev_wlc_intvar_set(dev, "wsec", val)))
			return error;
		break;

#endif /* WIRELESS_EXT > 17 */
	default:
		break;
	}
	return 0;
}
#define VAL_PSK(_val) (((_val) & WPA_AUTH_PSK) || ((_val) & WPA2_AUTH_PSK))

static int
wl_iw_get_wpaauth(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error;
	int paramid;
	int paramval = 0;
	int val;
	wl_iw_t *iw = NETDEV_PRIV(dev);

	WL_TRACE(("%s: SIOCGIWAUTH\n", dev->name));

	paramid = vwrq->flags & IW_AUTH_INDEX;

	switch (paramid) {
	case IW_AUTH_WPA_VERSION:
		paramval = iw->wpaversion;
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
		paramval = iw->pcipher;
		break;

	case IW_AUTH_CIPHER_GROUP:
		paramval = iw->gcipher;
		break;

	case IW_AUTH_KEY_MGMT:
		/* psk, 1x */
		if ((error = dev_wlc_intvar_get(dev, "wpa_auth", &val)))
			return error;
		if (VAL_PSK(val))
			paramval = IW_AUTH_KEY_MGMT_PSK;
		else
			paramval = IW_AUTH_KEY_MGMT_802_1X;

		break;

	case IW_AUTH_TKIP_COUNTERMEASURES:
		dev_wlc_bufvar_get(dev, "tkip_countermeasures", (char *)&paramval, 1);
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		dev_wlc_intvar_get(dev, "wsec_restrict", &paramval);
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		dev_wlc_bufvar_get(dev, "rx_unencrypted_eapol", (char *)&paramval, 1);
		break;

	case IW_AUTH_80211_AUTH_ALG:
		/* open, shared, leap */
		if ((error = dev_wlc_intvar_get(dev, "auth", &val)))
			return error;
		if (!val)
			paramval = IW_AUTH_ALG_OPEN_SYSTEM;
		else
			paramval = IW_AUTH_ALG_SHARED_KEY;
		break;
	case IW_AUTH_WPA_ENABLED:
		if ((error = dev_wlc_intvar_get(dev, "wpa_auth", &val)))
			return error;
		if (val)
			paramval = TRUE;
		else
			paramval = FALSE;
		break;
#if WIRELESS_EXT > 17
	case IW_AUTH_ROAMING_CONTROL:
		WL_ERROR(("%s: IW_AUTH_ROAMING_CONTROL\n", __FUNCTION__));
		/* driver control or user space app control */
		break;
	case IW_AUTH_PRIVACY_INVOKED:
		paramval = iw->privacy_invoked;
		break;

#endif /* WIRELESS_EXT > 17 */
	}
	vwrq->value = paramval;
	return 0;
}
#endif /* WIRELESS_EXT > 17 */


static const iw_handler wl_iw_handler[] =
{
	(iw_handler) wl_iw_config_commit,	/* SIOCSIWCOMMIT */
	(iw_handler) wl_iw_get_name,		/* SIOCGIWNAME */
	(iw_handler) NULL,			/* SIOCSIWNWID */
	(iw_handler) NULL,			/* SIOCGIWNWID */
	(iw_handler) wl_iw_set_freq,		/* SIOCSIWFREQ */
	(iw_handler) wl_iw_get_freq,		/* SIOCGIWFREQ */
	(iw_handler) wl_iw_set_mode,		/* SIOCSIWMODE */
	(iw_handler) wl_iw_get_mode,		/* SIOCGIWMODE */
	(iw_handler) NULL,			/* SIOCSIWSENS */
	(iw_handler) NULL,			/* SIOCGIWSENS */
	(iw_handler) NULL,			/* SIOCSIWRANGE */
	(iw_handler) wl_iw_get_range,		/* SIOCGIWRANGE */
	(iw_handler) NULL,			/* SIOCSIWPRIV */
	(iw_handler) NULL,			/* SIOCGIWPRIV */
	(iw_handler) NULL,			/* SIOCSIWSTATS */
	(iw_handler) NULL,			/* SIOCGIWSTATS */
	(iw_handler) wl_iw_set_spy,		/* SIOCSIWSPY */
	(iw_handler) wl_iw_get_spy,		/* SIOCGIWSPY */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) wl_iw_set_wap,		/* SIOCSIWAP */
	(iw_handler) wl_iw_get_wap,		/* SIOCGIWAP */
#if WIRELESS_EXT > 17
	(iw_handler) wl_iw_mlme,		/* SIOCSIWMLME */
#else
	(iw_handler) NULL,			/* -- hole -- */
#endif
#if defined(WL_IW_USE_ISCAN)
	(iw_handler) wl_iw_iscan_get_aplist,	/* SIOCGIWAPLIST */
#else
	(iw_handler) wl_iw_get_aplist,		/* SIOCGIWAPLIST */
#endif 
#if WIRELESS_EXT > 13
#if defined(WL_IW_USE_ISCAN)
	(iw_handler) wl_iw_iscan_set_scan,	/* SIOCSIWSCAN */
	(iw_handler) wl_iw_iscan_get_scan,	/* SIOCGIWSCAN */
#else
	(iw_handler) wl_iw_set_scan,		/* SIOCSIWSCAN */
	(iw_handler) wl_iw_get_scan,		/* SIOCGIWSCAN */
#endif
#else	/* WIRELESS_EXT > 13 */
	(iw_handler) NULL,			/* SIOCSIWSCAN */
	(iw_handler) NULL,			/* SIOCGIWSCAN */
#endif	/* WIRELESS_EXT > 13 */
	(iw_handler) wl_iw_set_essid,		/* SIOCSIWESSID */
	(iw_handler) wl_iw_get_essid,		/* SIOCGIWESSID */
	(iw_handler) wl_iw_set_nick,		/* SIOCSIWNICKN */
	(iw_handler) wl_iw_get_nick,		/* SIOCGIWNICKN */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) wl_iw_set_rate,		/* SIOCSIWRATE */
	(iw_handler) wl_iw_get_rate,		/* SIOCGIWRATE */
	(iw_handler) wl_iw_set_rts,		/* SIOCSIWRTS */
	(iw_handler) wl_iw_get_rts,		/* SIOCGIWRTS */
	(iw_handler) wl_iw_set_frag,		/* SIOCSIWFRAG */
	(iw_handler) wl_iw_get_frag,		/* SIOCGIWFRAG */
	(iw_handler) wl_iw_set_txpow,		/* SIOCSIWTXPOW */
	(iw_handler) wl_iw_get_txpow,		/* SIOCGIWTXPOW */
#if WIRELESS_EXT > 10
	(iw_handler) wl_iw_set_retry,		/* SIOCSIWRETRY */
	(iw_handler) wl_iw_get_retry,		/* SIOCGIWRETRY */
#endif /* WIRELESS_EXT > 10 */
	(iw_handler) wl_iw_set_encode,		/* SIOCSIWENCODE */
	(iw_handler) wl_iw_get_encode,		/* SIOCGIWENCODE */
	(iw_handler) wl_iw_set_power,		/* SIOCSIWPOWER */
	(iw_handler) wl_iw_get_power,		/* SIOCGIWPOWER */
#if WIRELESS_EXT > 17
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) wl_iw_set_wpaie,		/* SIOCSIWGENIE */
	(iw_handler) wl_iw_get_wpaie,		/* SIOCGIWGENIE */
	(iw_handler) wl_iw_set_wpaauth,		/* SIOCSIWAUTH */
	(iw_handler) wl_iw_get_wpaauth,		/* SIOCGIWAUTH */
	(iw_handler) wl_iw_set_encodeext,	/* SIOCSIWENCODEEXT */
	(iw_handler) wl_iw_get_encodeext,	/* SIOCGIWENCODEEXT */
	(iw_handler) wl_iw_set_pmksa,			/* SIOCSIWPMKSA */
#endif /* WIRELESS_EXT > 17 */
};

#if WIRELESS_EXT > 12

const struct iw_handler_def wl_iw_handler_def =
{
	.num_standard = ARRAYSIZE(wl_iw_handler),
	.standard = (iw_handler *) wl_iw_handler,
	.num_private = 0,
	.num_private_args = 0,
	.private = 0,
	.private_args = 0,

#if WIRELESS_EXT >= 19
	get_wireless_stats: dhd_get_wireless_stats,
#endif /* WIRELESS_EXT >= 19 */
	};
#endif /* WIRELESS_EXT > 12 */


/*
 *  DHD driver IOCTLS in (cmd >= SIOCIWFIRST) && (cmd <= SIOCIWLAST)) range land in here
 *
 */
int
wl_iw_ioctl(
	struct net_device *dev,
	struct ifreq *rq,
	int cmd
)
{
	struct iwreq *wrq = (struct iwreq *) rq;
	struct iw_request_info info;
	iw_handler handler;
	char *extra = NULL;
	size_t token_size = 1;
	int max_tokens = 0, ret = 0;

	net_os_wake_lock(dev);

	WL_TRACE(("\n%s, cmd:%x called via dhd->do_ioctl()entry point\n", __FUNCTION__, cmd));
	if (cmd < SIOCIWFIRST ||
		IW_IOCTL_IDX(cmd) >= ARRAYSIZE(wl_iw_handler) ||
		!(handler = wl_iw_handler[IW_IOCTL_IDX(cmd)])) {
			WL_ERROR(("%s: error in cmd=%x : not supported\n", __FUNCTION__, cmd));
			net_os_wake_unlock(dev);
			return -EOPNOTSUPP;
	}

	switch (cmd) {

	case SIOCSIWESSID:
	case SIOCGIWESSID:
	case SIOCSIWNICKN:
	case SIOCGIWNICKN:
		max_tokens = IW_ESSID_MAX_SIZE + 1;
		break;

	case SIOCSIWENCODE:
	case SIOCGIWENCODE:
#if WIRELESS_EXT > 17
	case SIOCSIWENCODEEXT:
	case SIOCGIWENCODEEXT:
#endif
		max_tokens = wrq->u.data.length;
		break;

	case SIOCGIWRANGE:
		/*  GG Adding stange 500 bytes to match the same extra number of bytes
			from wpa_supplicant code, function wpa_driver_wext_get_range
			From 2.6.31 it will be called directly and not from the handle so
			we have to make adjustment to pass checking below
		*/
		max_tokens = sizeof(struct iw_range) + 500;
		break;

	case SIOCGIWAPLIST:
		token_size = sizeof(struct sockaddr) + sizeof(struct iw_quality);
		max_tokens = IW_MAX_AP;
		break;

#if WIRELESS_EXT > 13
	case SIOCGIWSCAN:
#if defined(WL_IW_USE_ISCAN)
	if (g_iscan)
		max_tokens = wrq->u.data.length;
	else
#endif
		max_tokens = IW_SCAN_MAX_DATA;
		break;
#endif /* WIRELESS_EXT > 13 */

	case SIOCSIWSPY:
		token_size = sizeof(struct sockaddr);
		max_tokens = IW_MAX_SPY;
		break;

	case SIOCGIWSPY:
		token_size = sizeof(struct sockaddr) + sizeof(struct iw_quality);
		max_tokens = IW_MAX_SPY;
		break;

#if WIRELESS_EXT > 17
	case SIOCSIWPMKSA:
	case SIOCSIWGENIE:
#endif /* WIRELESS_EXT > 17 */
	case SIOCSIWPRIV:
		max_tokens = wrq->u.data.length;
		break;
	}

	if (max_tokens && wrq->u.data.pointer) {
		if (wrq->u.data.length > max_tokens) {
			WL_ERROR(("%s: error in cmd=%x wrq->u.data.length=%d  > max_tokens=%d\n",
				__FUNCTION__, cmd, wrq->u.data.length, max_tokens));
			ret = -E2BIG;
			goto wl_iw_ioctl_done;
		}
		if (!(extra = kmalloc(max_tokens * token_size, GFP_KERNEL))) {
			ret = -ENOMEM;
			goto wl_iw_ioctl_done;
		}

		if (copy_from_user(extra, wrq->u.data.pointer, wrq->u.data.length * token_size)) {
			kfree(extra);
			ret = -EFAULT;
			goto wl_iw_ioctl_done;
		}
	}

	info.cmd = cmd;
	info.flags = 0;

	ret = handler(dev, &info, &wrq->u, extra);

	if (extra) {
		if (copy_to_user(wrq->u.data.pointer, extra, wrq->u.data.length * token_size)) {
			kfree(extra);
			ret = -EFAULT;
			goto wl_iw_ioctl_done;
		}

		kfree(extra);
	}

wl_iw_ioctl_done:

	net_os_wake_unlock(dev);

	return ret;
}

/* Convert a connection status event into a connection status string.
 * Returns TRUE if a matching connection status string was found.
 */
static bool
wl_iw_conn_status_str(uint32 event_type, uint32 status, uint32 reason,
	char* stringBuf, uint buflen)
{
	typedef struct conn_fail_event_map_t {
		uint32 inEvent;			/* input: event type to match */
		uint32 inStatus;		/* input: event status code to match */
		uint32 inReason;		/* input: event reason code to match */
		const char* outName;	/* output: failure type */
		const char* outCause;	/* output: failure cause */
	} conn_fail_event_map_t;

	/* Map of WLC_E events to connection failure strings */
#define WL_IW_DONT_CARE	9999
	const conn_fail_event_map_t event_map [] = {
		/* inEvent           inStatus                inReason         */
		/* outName outCause                                           */
		{WLC_E_SET_SSID,     WLC_E_STATUS_SUCCESS,   WL_IW_DONT_CARE,
		"Conn", "Success"},
		{WLC_E_SET_SSID,     WLC_E_STATUS_NO_NETWORKS, WL_IW_DONT_CARE,
		"Conn", "NoNetworks"},
		{WLC_E_SET_SSID,     WLC_E_STATUS_FAIL,      WL_IW_DONT_CARE,
		"Conn", "ConfigMismatch"},
		{WLC_E_PRUNE,        WL_IW_DONT_CARE,        WLC_E_PRUNE_ENCR_MISMATCH,
		"Conn", "EncrypMismatch"},
		{WLC_E_PRUNE,        WL_IW_DONT_CARE,        WLC_E_RSN_MISMATCH,
		"Conn", "RsnMismatch"},
		{WLC_E_AUTH,         WLC_E_STATUS_TIMEOUT,   WL_IW_DONT_CARE,
		"Conn", "AuthTimeout"},
		{WLC_E_AUTH,         WLC_E_STATUS_FAIL,      WL_IW_DONT_CARE,
		"Conn", "AuthFail"},
		{WLC_E_AUTH,         WLC_E_STATUS_NO_ACK,    WL_IW_DONT_CARE,
		"Conn", "AuthNoAck"},
		{WLC_E_REASSOC,      WLC_E_STATUS_FAIL,      WL_IW_DONT_CARE,
		"Conn", "ReassocFail"},
		{WLC_E_REASSOC,      WLC_E_STATUS_TIMEOUT,   WL_IW_DONT_CARE,
		"Conn", "ReassocTimeout"},
		{WLC_E_REASSOC,      WLC_E_STATUS_ABORT,     WL_IW_DONT_CARE,
		"Conn", "ReassocAbort"},
		{WLC_E_PSK_SUP,      WLC_SUP_KEYED,          WL_IW_DONT_CARE,
		"Sup", "ConnSuccess"},
		{WLC_E_PSK_SUP,      WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Sup", "WpaHandshakeFail"},
		{WLC_E_DEAUTH_IND,   WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Conn", "Deauth"},
		{WLC_E_DISASSOC_IND, WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Conn", "DisassocInd"},
		{WLC_E_DISASSOC,     WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Conn", "Disassoc"}
	};

	const char* name = "";
	const char* cause = NULL;
	int i;

	/* Search the event map table for a matching event */
	for (i = 0;  i < sizeof(event_map)/sizeof(event_map[0]);  i++) {
		const conn_fail_event_map_t* row = &event_map[i];
		if (row->inEvent == event_type &&
		    (row->inStatus == status || row->inStatus == WL_IW_DONT_CARE) &&
		    (row->inReason == reason || row->inReason == WL_IW_DONT_CARE)) {
			name = row->outName;
			cause = row->outCause;
			break;
		}
	}

	/* If found, generate a connection failure string and return TRUE */
	if (cause) {
		memset(stringBuf, 0, buflen);
		snprintf(stringBuf, buflen, "%s %s %02d %02d",
			name, cause, status, reason);
		WL_INFORM(("Connection status: %s\n", stringBuf));
		return TRUE;
	} else {
		return FALSE;
	}
}

#if WIRELESS_EXT > 14
/* Check if we have received an event that indicates connection failure
 * If so, generate a connection failure report string.
 * The caller supplies a buffer to hold the generated string.
 */
static bool
wl_iw_check_conn_fail(wl_event_msg_t *e, char* stringBuf, uint buflen)
{
	uint32 event = ntoh32(e->event_type);
	uint32 status =  ntoh32(e->status);
	uint32 reason =  ntoh32(e->reason);

	if (wl_iw_conn_status_str(event, status, reason, stringBuf, buflen)) {
		return TRUE;
	}
	else
		return FALSE;
}
#endif /* WIRELESS_EXT > 14 */

#ifndef IW_CUSTOM_MAX
#define IW_CUSTOM_MAX 256 /* size of extra buffer used for translation of events */
#endif /* IW_CUSTOM_MAX */

void
wl_iw_event(struct net_device *dev, wl_event_msg_t *e, void* data)
{
#if WIRELESS_EXT > 13
	union iwreq_data wrqu;
	char extra[IW_CUSTOM_MAX + 1];
	int cmd = 0;
	uint32 event_type = ntoh32(e->event_type);
	uint16 flags =  ntoh16(e->flags);
	uint32 datalen = ntoh32(e->datalen);
	uint32 status =  ntoh32(e->status);
	uint32 toto;
	memset(&wrqu, 0, sizeof(wrqu));
	memset(extra, 0, sizeof(extra));

	if (!dev) {
		WL_ERROR(("%s: dev is null\n", __FUNCTION__));
		return;
	}

	net_os_wake_lock(dev);

	WL_TRACE(("%s: dev=%s event=%d \n", __FUNCTION__, dev->name, event_type));

	/* Different Events may pointed to the different structures.
	  * See union (!) iwreq_data declaration for IOCTL REQUEST in WPA_SUPPLICANT code
	  * Refer to desciptions and header_type of each Event in WPA_SUPPLICAN code
	  * and your kernel wext.c file before adding any new case after switch
	  */
	switch (event_type) {
	case WLC_E_TXFAIL:
		cmd = IWEVTXDROP;
		memcpy(wrqu.addr.sa_data, &e->addr, ETHER_ADDR_LEN);
		wrqu.addr.sa_family = ARPHRD_ETHER;
		break;
#if WIRELESS_EXT > 14
	case WLC_E_JOIN:
	case WLC_E_ASSOC_IND:
	case WLC_E_REASSOC_IND:
		memcpy(wrqu.addr.sa_data, &e->addr, ETHER_ADDR_LEN);
		wrqu.addr.sa_family = ARPHRD_ETHER;
		cmd = IWEVREGISTERED;
		break;
	case WLC_E_DEAUTH_IND:
	case WLC_E_DISASSOC_IND:
		cmd = SIOCGIWAP;
		bzero(wrqu.addr.sa_data, ETHER_ADDR_LEN);
		wrqu.addr.sa_family = ARPHRD_ETHER;
		bzero(&extra, ETHER_ADDR_LEN);
		break;
	case WLC_E_LINK:
	case WLC_E_NDIS_LINK:
		cmd = SIOCGIWAP;
		if (!(flags & WLC_EVENT_MSG_LINK)) {
			/* link down , return bssid zero */

			bzero(wrqu.addr.sa_data, ETHER_ADDR_LEN);
			bzero(&extra, ETHER_ADDR_LEN);
		}
		else {
			/* link up with AP's bssid */
			memcpy(wrqu.addr.sa_data, &e->addr, ETHER_ADDR_LEN);
			WL_TRACE(("Link UP\n"));

		}
		net_os_wake_lock_timeout_enable(dev);
		wrqu.addr.sa_family = ARPHRD_ETHER;
		break;
	case WLC_E_ACTION_FRAME:
		cmd = IWEVCUSTOM;
		if (datalen + 1 <= sizeof(extra)) {
			wrqu.data.length = datalen + 1;
			extra[0] = WLC_E_ACTION_FRAME;
			memcpy(&extra[1], data, datalen);
			WL_TRACE(("WLC_E_ACTION_FRAME len %d \n", wrqu.data.length));
		}
		break;

	case WLC_E_ACTION_FRAME_COMPLETE:
		cmd = IWEVCUSTOM;
		memcpy(&toto, data, 4);
		if (sizeof(status) + 1 <= sizeof(extra)) {
			wrqu.data.length = sizeof(status) + 1;
			extra[0] = WLC_E_ACTION_FRAME_COMPLETE;
			memcpy(&extra[1], &status, sizeof(status));
			printf("wl_iw_event status %d PacketId %d \n", status, toto);
			printf("WLC_E_ACTION_FRAME_COMPLETE len %d \n", wrqu.data.length);
		}
		break;
#endif /* WIRELESS_EXT > 14 */
#if WIRELESS_EXT > 17
	case WLC_E_MIC_ERROR: {
		struct	iw_michaelmicfailure  *micerrevt = (struct  iw_michaelmicfailure  *)&extra;
		cmd = IWEVMICHAELMICFAILURE;
		wrqu.data.length = sizeof(struct iw_michaelmicfailure);
		if (flags & WLC_EVENT_MSG_GROUP)
			micerrevt->flags |= IW_MICFAILURE_GROUP;
		else
			micerrevt->flags |= IW_MICFAILURE_PAIRWISE;
		memcpy(micerrevt->src_addr.sa_data, &e->addr, ETHER_ADDR_LEN);
		micerrevt->src_addr.sa_family = ARPHRD_ETHER;

		break;
	}
	case WLC_E_PMKID_CACHE: {
		if (data)
		{
			struct iw_pmkid_cand *iwpmkidcand = (struct iw_pmkid_cand *)&extra;
			pmkid_cand_list_t *pmkcandlist;
			pmkid_cand_t	*pmkidcand;
			int count;

			cmd = IWEVPMKIDCAND;
			pmkcandlist = data;
			count = ntoh32_ua((uint8 *)&pmkcandlist->npmkid_cand);
			ASSERT(count >= 0);
			wrqu.data.length = sizeof(struct iw_pmkid_cand);
			pmkidcand = pmkcandlist->pmkid_cand;
			while (count) {
				bzero(iwpmkidcand, sizeof(struct iw_pmkid_cand));
				if (pmkidcand->preauth)
					iwpmkidcand->flags |= IW_PMKID_CAND_PREAUTH;
				bcopy(&pmkidcand->BSSID, &iwpmkidcand->bssid.sa_data,
					ETHER_ADDR_LEN);
#ifndef SANDGATE2G
				wireless_send_event(dev, cmd, &wrqu, extra);
#endif
				pmkidcand++;
				count--;
			}
		}
		goto wl_iw_event_end;
	}
#endif /* WIRELESS_EXT > 17 */

	case WLC_E_SCAN_COMPLETE:
#if defined(WL_IW_USE_ISCAN)
		if ((g_iscan) && (g_iscan->sysioc_pid > 0) &&
			(g_iscan->iscan_state != ISCAN_STATE_IDLE))
		{
			up(&g_iscan->sysioc_sem);
		} else {
			cmd = SIOCGIWSCAN;
			wrqu.data.length = strlen(extra);
			WL_TRACE(("Event WLC_E_SCAN_COMPLETE from specific scan %d\n",
				g_iscan->iscan_state));
		}
#else
		cmd = SIOCGIWSCAN;
		wrqu.data.length = strlen(extra);
		WL_TRACE(("Event WLC_E_SCAN_COMPLETE\n"));
#endif /* defined((WL_IW_USE_ISCAN) */
	break;

	/* PNO Event */
	case WLC_E_PFN_NET_FOUND:
	{
		wlc_ssid_t	* ssid;
		ssid = (wlc_ssid_t *)data;
		WL_ERROR(("%s Event WLC_E_PFN_NET_FOUND, send %s up : find %s len=%d\n",
			__FUNCTION__, PNO_EVENT_UP, ssid->SSID, ssid->SSID_len));
		net_os_wake_lock_timeout_enable(dev);
		cmd = IWEVCUSTOM;
		memset(&wrqu, 0, sizeof(wrqu));
		strcpy(extra, PNO_EVENT_UP);
		wrqu.data.length = strlen(extra);
	}
	break;

	default:
		/* Cannot translate event */
		WL_TRACE(("Unknown Event %d: ignoring\n", event_type));
		break;
	}
#ifndef SANDGATE2G
		if (cmd) {
			if (cmd == SIOCGIWSCAN)
				wireless_send_event(dev, cmd, &wrqu, NULL);
			else
				wireless_send_event(dev, cmd, &wrqu, extra);
		}
#endif

#if WIRELESS_EXT > 14
	/* Look for WLC events that indicate a connection failure.
	 * If found, generate an IWEVCUSTOM event.
	 */
	memset(extra, 0, sizeof(extra));
	if (wl_iw_check_conn_fail(e, extra, sizeof(extra))) {
		cmd = IWEVCUSTOM;
		wrqu.data.length = strlen(extra);
#ifndef SANDGATE2G
		wireless_send_event(dev, cmd, &wrqu, extra);
#endif
	}
	goto wl_iw_event_end;	/* Avoid warning "label defined but not used" */
wl_iw_event_end:
	net_os_wake_unlock(dev);
#endif /* WIRELESS_EXT > 14 */
#endif /* WIRELESS_EXT > 13 */
}

int
wl_iw_get_wireless_stats(struct net_device *dev, struct iw_statistics *wstats)
{
	int res = 0;
	wl_cnt_t cnt;
	int phy_noise;
	int rssi;
	scb_val_t scb_val;

	phy_noise = 0;
	if ((res = dev_wlc_ioctl(dev, WLC_GET_PHY_NOISE, &phy_noise, sizeof(phy_noise))))
		goto done;

	phy_noise = dtoh32(phy_noise);
	WL_TRACE(("wl_iw_get_wireless_stats phy noise=%d\n", phy_noise));

	bzero(&scb_val, sizeof(scb_val_t));
	if ((res = dev_wlc_ioctl(dev, WLC_GET_RSSI, &scb_val, sizeof(scb_val_t))))
		goto done;

	rssi = dtoh32(scb_val.val);
	WL_TRACE(("wl_iw_get_wireless_stats rssi=%d\n", rssi));
	if (rssi <= WL_IW_RSSI_NO_SIGNAL)
		wstats->qual.qual = 0;
	else if (rssi <= WL_IW_RSSI_VERY_LOW)
		wstats->qual.qual = 1;
	else if (rssi <= WL_IW_RSSI_LOW)
		wstats->qual.qual = 2;
	else if (rssi <= WL_IW_RSSI_GOOD)
		wstats->qual.qual = 3;
	else if (rssi <= WL_IW_RSSI_VERY_GOOD)
		wstats->qual.qual = 4;
	else
		wstats->qual.qual = 5;

	/* Wraps to 0 if RSSI is 0 */
	wstats->qual.level = 0x100 + rssi;
	wstats->qual.noise = 0x100 + phy_noise;
#if WIRELESS_EXT > 18
	wstats->qual.updated |= (IW_QUAL_ALL_UPDATED | IW_QUAL_DBM);
#else
	wstats->qual.updated |= 7;
#endif /* WIRELESS_EXT > 18 */

#if WIRELESS_EXT > 11
	WL_TRACE(("wl_iw_get_wireless_stats counters=%d\n", (int)sizeof(wl_cnt_t)));

	memset(&cnt, 0, sizeof(wl_cnt_t));
	res = dev_wlc_bufvar_get(dev, "counters", (char *)&cnt, sizeof(wl_cnt_t));
	if (res)
	{
		WL_ERROR(("wl_iw_get_wireless_stats counters failed error=%d\n", res));
		goto done;
	}

	cnt.version = dtoh16(cnt.version);
	if (cnt.version != WL_CNT_T_VERSION) {
		WL_TRACE(("\tIncorrect version of counters struct: expected %d; got %d\n",
			WL_CNT_T_VERSION, cnt.version));
		goto done;
	}

	wstats->discard.nwid = 0;
	wstats->discard.code = dtoh32(cnt.rxundec);
	wstats->discard.fragment = dtoh32(cnt.rxfragerr);
	wstats->discard.retries = dtoh32(cnt.txfail);
	wstats->discard.misc = dtoh32(cnt.rxrunt) + dtoh32(cnt.rxgiant);
	wstats->miss.beacon = 0;

	WL_TRACE(("wl_iw_get_wireless_stats counters txframe=%d txbyte=%d\n",
		dtoh32(cnt.txframe), dtoh32(cnt.txbyte)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxfrmtoolong=%d\n", dtoh32(cnt.rxfrmtoolong)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxbadplcp=%d\n", dtoh32(cnt.rxbadplcp)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxundec=%d\n", dtoh32(cnt.rxundec)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxfragerr=%d\n", dtoh32(cnt.rxfragerr)));
	WL_TRACE(("wl_iw_get_wireless_stats counters txfail=%d\n", dtoh32(cnt.txfail)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxrunt=%d\n", dtoh32(cnt.rxrunt)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxgiant=%d\n", dtoh32(cnt.rxgiant)));

#endif /* WIRELESS_EXT > 11 */

done:
	return res;
}


int
wl_iw_attach(struct net_device *dev, void * dhdp)
{
#if defined(WL_IW_USE_ISCAN)
	int params_size = 0;
#endif /* defined(WL_IW_USE_ISCAN) */
	wl_iw_t *iw;
#if defined(WL_IW_USE_ISCAN)
	iscan_info_t *iscan = NULL;
#endif

	DHD_OS_MUTEX_INIT(&wl_cache_lock);
	DHD_OS_MUTEX_INIT(&wl_start_lock);
	DHD_OS_MUTEX_INIT(&wl_softap_lock);

#if defined(WL_IW_USE_ISCAN)
	if (!dev)
		return 0;

	/* clean all global vars */
	memset(&g_wl_iw_params, 0, sizeof(wl_iw_extra_params_t));

	/* prealloc buffer for max iscan extra params size */
#ifdef CSCAN
	params_size = (WL_SCAN_PARAMS_FIXED_SIZE + OFFSETOF(wl_iscan_params_t, params)) +
	    (WL_NUMCHANNELS * sizeof(uint16)) + WL_SCAN_PARAMS_SSID_MAX * sizeof(wlc_ssid_t);
#else
	params_size = (WL_SCAN_PARAMS_FIXED_SIZE + OFFSETOF(wl_iscan_params_t, params));
#endif /* CSCAN */
	iscan = kmalloc(sizeof(iscan_info_t), GFP_KERNEL);
	if (!iscan)
		return -ENOMEM;
	memset(iscan, 0, sizeof(iscan_info_t));

	/* Allocate memory for iscan extra params */
	iscan->iscan_ex_params_p = (wl_iscan_params_t*)kmalloc(params_size, GFP_KERNEL);
	if (!iscan->iscan_ex_params_p)
		return -ENOMEM;
	iscan->iscan_ex_param_size = params_size;
	iscan->sysioc_pid = -1;
	/* we only care about main interface so save a global here */
	g_iscan = iscan;
	iscan->dev = dev;
	iscan->iscan_state = ISCAN_STATE_IDLE;


	/* Set up the timer */
	iscan->timer_ms    = 3000;
	init_timer(&iscan->timer);
	iscan->timer.data = (ulong)iscan;
	iscan->timer.function = wl_iw_timerfunc;

	sema_init(&iscan->sysioc_sem, 0);
	init_completion(&iscan->sysioc_exited);
	iscan->sysioc_pid = kernel_thread(_iscan_sysioc_thread, iscan, 0);
	if (iscan->sysioc_pid < 0)
		return -ENOMEM;
#endif /* WL_IW_USE_ISCAN */

	iw = *(wl_iw_t **)netdev_priv(dev);
	iw->pub = (dhd_pub_t *)dhdp;
#ifdef SOFTAP
	priv_dev = dev;
#endif /* SOFTAP */
	g_scan = NULL;

	/* Get scan results : allocate 16K for now */
	g_scan = (void *)kmalloc(G_SCAN_RESULTS, GFP_KERNEL);
	if (!g_scan)
		return -ENOMEM;

	memset(g_scan, 0, G_SCAN_RESULTS);
	g_scan_specified_ssid = 0;


	return 0;
}

void
wl_iw_detach(void)
{
#if defined(WL_IW_USE_ISCAN)
	iscan_buf_t  *buf;
	iscan_info_t *iscan = g_iscan;

	if (!iscan)
		return;
	if (iscan->sysioc_pid >= 0) {
		KILL_PROC(iscan->sysioc_pid, SIGTERM);
		wait_for_completion(&iscan->sysioc_exited);
	}
	DHD_OS_MUTEX_LOCK(&wl_cache_lock);
	while (iscan->list_hdr) {
		buf = iscan->list_hdr->next;
		kfree(iscan->list_hdr);
		iscan->list_hdr = buf;
	}
	kfree(iscan->iscan_ex_params_p);
	kfree(iscan);
	g_iscan = NULL;
	DHD_OS_MUTEX_UNLOCK(&wl_cache_lock);
#endif /* BCMDONGLEHOST */

	if (g_scan)
		kfree(g_scan);

	g_scan = NULL;
}
