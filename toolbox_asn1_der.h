/* $Gateweaver: toolbox_asn1_der.h,v 1.6 2006/02/10 20:06:02 cmaxwell Exp $ */
/* $KTH: der.h,v 1.28 2005/05/29 14:23:00 lha Exp $ */
/*
 * Copyright (c) 2005 Christopher Maxwell.  All rights reserved.
 */
/*
 * Copyright (c) 1997 - 2001 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#ifndef __DER_H__
#define __DER_H__

#include <time.h>

typedef enum {
    ASN1_C_UNIV = 0,
    ASN1_C_APPL = 1,
    ASN1_C_CONTEXT = 2 ,
    ASN1_C_PRIVATE = 3
} Der_class;

typedef enum {PRIM = 0, CONS = 1} Der_type;

/* Universal tags */

enum {
     UT_Boolean			= 1,
     UT_Integer			= 2,
     UT_BitString		= 3,
     UT_OctetString		= 4,
     UT_Null			= 5,
     UT_OID				= 6,
     UT_Enumerated		= 10,
     UT_UTF8String		= 12,
     UT_Sequence		= 16,
     UT_Set				= 17,
     UT_PrintableString	= 19,
     UT_IA5String		= 22,
     UT_UTCTime			= 23,
     UT_GeneralizedTime	= 24,
     UT_VisibleString	= 26,
     UT_GeneralString	= 27
};

#define ASN1_INDEFINITE 0xdce0deed

int time2generaltime(time_t, tb_octet_string *);

int tb_der_get_int32(const unsigned char *, size_t, int32_t *, size_t *);
int tb_der_get_int64(const unsigned char *, size_t, int64_t *, size_t *);
int tb_der_get_len(const unsigned char *, size_t,
		size_t *, size_t *);
int tb_der_get_bool(const unsigned char *, size_t,
		int *, size_t *);
int tb_der_get_str(const unsigned char *, size_t, 
		tb_general_string *, size_t *);
int tb_der_get_bytes(const unsigned char *, size_t, 
		tb_octet_string *, size_t *);
int tb_der_get_oid(const unsigned char *, size_t,
		tb_oid *, size_t *);
int tb_der_get_tag(const unsigned char *, size_t, 
		Der_class *, Der_type *,
		int *, size_t *);

int tb_der_match_tag(const unsigned char *, size_t, 
		Der_class, Der_type,
		int, size_t *);
int tb_der_match_tag_and_len(const unsigned char *, size_t,
		Der_class, Der_type, int,
		size_t *, size_t *);

int dec_bool(const unsigned char *, size_t, int *, size_t *);
int dec_int32(const unsigned char *, size_t, int32_t *, size_t *);
int dec_int64(const unsigned char *, size_t, int64_t *, size_t *);
int dec_uint32(const unsigned char *, size_t, uint32_t *, size_t *);
int dec_uint64(const unsigned char *, size_t, uint64_t *, size_t *);
int dec_enum(const unsigned char *, size_t, unsigned int *, size_t *);
int dec_str(const unsigned char *, size_t,
		tb_general_string *, size_t *);
int dec_oid(const unsigned char *, size_t, 
		tb_oid *, size_t *);
int dec_bytes(const unsigned char *, size_t, 
		tb_octet_string *, size_t *);
int dec_time(const unsigned char *, size_t, time_t *, size_t *);
int dec_null(const unsigned char *, size_t, size_t *);
int dec_utf8(const unsigned char *, size_t, 
		tb_utf8_string *, size_t *);

int tb_der_put_int32(unsigned char *, size_t, int32_t, size_t *);
int tb_der_put_int64(unsigned char *, size_t, int64_t, size_t *);
int tb_der_put_len(unsigned char *, size_t, size_t, size_t *);
int tb_der_put_bool(unsigned char *, size_t, const int *, size_t *);
int tb_der_put_str(unsigned char *, size_t,
		const tb_general_string *, size_t *);
int tb_der_put_bytes(unsigned char *, size_t,
		const tb_octet_string *, size_t *);
int tb_der_put_oid(unsigned char *, size_t,
		const tb_oid *, size_t *);
int tb_der_put_tag(unsigned char *, size_t, Der_class, Der_type,
		int, size_t *);
int tb_der_put_len_and_tag(unsigned char *, size_t, size_t, 
		Der_class, Der_type, int, size_t *);

int enc_bool(unsigned char *, size_t, const int *, size_t *);
int enc_int32(unsigned char *, size_t, const int32_t *, size_t *);
int enc_int64(unsigned char *, size_t, const int64_t *, size_t *);
int enc_uint32(unsigned char *, size_t, const uint32_t *, size_t *);
int enc_uint64(unsigned char *, size_t, const uint64_t *, size_t *);
int enc_enum(unsigned char *, size_t, const unsigned int *, size_t *);
int enc_str(unsigned char *, size_t, const tb_general_string *, size_t *);
int enc_bytes(unsigned char *, size_t, const tb_octet_string *, size_t *);
int enc_oid(unsigned char *, size_t, const tb_oid *, size_t *);
int enc_time(unsigned char *, size_t, const time_t *, size_t *);
int enc_null(unsigned char *, size_t, size_t *);
int enc_utf8(unsigned char *, size_t, const tb_utf8_string *, size_t *);

void free_int32(int32_t *);
void free_int64(int64_t *);
void free_uint32(uint32_t *);
void free_uint64(uint64_t *);
void free_str(tb_general_string *);
void free_bytes(tb_octet_string *);
void free_tboid(tb_oid *);
void free_time(time_t *);
void free_utf8(tb_utf8_string *);

size_t len_len(size_t);
size_t len_len64(uint64_t);
size_t len_tag(size_t);
size_t len_bool(const int *);
size_t len_int32(const int32_t *);
size_t len_int64(const int64_t *);
size_t len_uint32(const uint32_t *);
size_t len_uint64(const uint64_t *);
size_t len_enum(const unsigned int *);
size_t len_str(const tb_general_string *);
size_t len_bytes(const tb_octet_string *);
size_t len_oid(const tb_oid *);
size_t len_time(const time_t *);
size_t len_null(void);
size_t len_utf8(const tb_utf8_string *);

int copy_str(const tb_general_string *, tb_general_string *);
int copy_bytes(const tb_octet_string *, tb_octet_string *);
int copy_tboid(const tb_oid *, tb_oid *);
int copy_null(void *, void *);
int copy_utf8(const tb_utf8_string *, tb_utf8_string *);

int cmp_oid(const tb_oid *, const tb_oid *);
int cmp_bytes(const tb_octet_string *, const tb_octet_string *);

#endif /* __DER_H__ */
