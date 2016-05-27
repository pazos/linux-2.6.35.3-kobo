/*
 * Console support for hndrte.
 *
 * Copyright (C) 2011, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: hndrte_cons.h,v 13.4.10.4 2011-02-05 00:08:20 Exp $
 */

#ifndef	_HNDRTE_CONS_H
#define	_HNDRTE_CONS_H

#include <typedefs.h>

#if defined(RWL_DONGLE) || defined(UART_REFLECTOR)
/* For Dongle uart tranport max cmd len is 256 bytes + header length (16 bytes)
 *  In case of ASD commands we are not sure about how much is the command size
 *  To be on the safe side, input buf len CBUF_LEN is increased to max (512) bytes.
 */
#define RWL_MAX_DATA_LEN 	(512 + 8)	/* allow some extra bytes for '/n' termination */
#define CBUF_LEN	(RWL_MAX_DATA_LEN + 64)  /* allow 64 bytes for header ("rwl...") */
#else
#define CBUF_LEN	(128)
#endif /* RWL_DONGLE || UART_REFLECTOR */

#if defined(WLLMAC)
#define LOG_BUF_LEN	2048
#else
#define LOG_BUF_LEN	1024
#endif

typedef struct {
	uint32		buf;		/* Can't be pointer on (64-bit) hosts */
	uint		buf_size;
	uint		idx;
	char		*_buf_compat;	/* redundant pointer for backward compat. */
} hndrte_log_t;

typedef struct {
	/* Virtual UART
	 *   When there is no UART (e.g. Quickturn), the host should write a complete
	 *   input line directly into cbuf and then write the length into vcons_in.
	 *   This may also be used when there is a real UART (at risk of conflicting with
	 *   the real UART).  vcons_out is currently unused.
	 */
	volatile uint	vcons_in;
	volatile uint	vcons_out;

	/* Output (logging) buffer
	 *   Console output is written to a ring buffer log_buf at index log_idx.
	 *   The host may read the output when it sees log_idx advance.
	 *   Output will be lost if the output wraps around faster than the host polls.
	 */
	hndrte_log_t	log;

	/* Console input line buffer
	 *   Characters are read one at a time into cbuf until <CR> is received, then
	 *   the buffer is processed as a command line.  Also used for virtual UART.
	 */
	uint		cbuf_idx;
	char		cbuf[CBUF_LEN];
} hndrte_cons_t;

#endif /* _HNDRTE_CONS_H */