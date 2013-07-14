/* $Gateweaver: der_put.c,v 1.9 2007/03/12 17:49:09 cmaxwell Exp $ */
/* $KTH: der_put.c,v 1.32 2005/05/29 14:23:01 lha Exp $ */
/*
 * Copyright (c) 2005 Christopher Maxwell.  All rights reserved.
 */
/*
 * Copyright (c) 1997-2003 Kungliga Tekniska Högskolan
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

#include "der_locl.h"

/*
 * All encoding functions take a pointer `p' to first position in
 * which to write, from the right, `len' which means the maximum
 * number of characters we are able to write.  The function returns
 * the number of characters written in `size' (if non-NULL).
 * The return value is 0 or an error.
 */

static int
tb_der_put_uint32(unsigned char *p, size_t len,
		uint32_t val, size_t *size)
{
    unsigned char *base = p;

    if (val) {
		while (len > 0 && val) {
			*p-- = val % 256;
			val /= 256;
			--len;
		}
		if (val != 0)
			return TBASN1_OVERFLOW;
		else {
			*size = base - p;
			return 0;
		}
	} else if (len < 1)
		return TBASN1_OVERFLOW;
	else {
		*p    = 0;
		*size = 1;
		return 0;
    }
}

int
tb_der_put_int32(unsigned char *p, size_t len,
		int32_t val, size_t *size)
{
    unsigned char *base = p;

    if (val >= 0) {
		do {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = val % 256;
			len--;
			val /= 256;
		} while(val);
		if (p[1] >= 128) {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = 0;
			len--;
		}
	} else {
		val = ~val;
		do {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = ~(val % 256);
			len--;
			val /= 256;
		} while(val);
		if (p[1] < 128) {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = 0xff;
			len--;
		}
	}
	*size = base - p;
    return 0;
}

static int
tb_der_put_uint64(unsigned char *p, size_t len,
		uint64_t val, size_t *size)
{
    unsigned char *base = p;

    if (val) {
		while (len > 0 && val) {
			*p-- = val % 256;
			val /= 256;
			--len;
		}
		if (val != 0)
			return TBASN1_OVERFLOW;
		else {
			*size = base - p;
			return 0;
		}
	} else if (len < 1)
		return TBASN1_OVERFLOW;
	else {
		*p    = 0;
		*size = 1;
		return 0;
    }
}

int
tb_der_put_int64(unsigned char *p, size_t len,
		int64_t val, size_t *size)
{
	unsigned char *base = p;

	if (val >= 0) {
		do {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = val % 256;
			len--;
			val /= 256;
		} while (val);
		if (p[1] >= 128) {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = 0;
			len--;
		}
	} else {
		val = ~val;
		do {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = ~(val % 256);
			len--;
			val /= 256;
		} while (val);
		if (p[1] < 128) {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = 0xFF;
			len--;
		}
	}
	*size = base - p;
	return 0;
}

int
tb_der_put_len(unsigned char *p, size_t len,
		size_t val, size_t *size)
{
    if (len < 1)
		return TBASN1_OVERFLOW;
    if (val < 128) {
		*p = val;
		*size = 1;
		return 0;
    } else {
		size_t l;
		int e;

		if ((e = tb_der_put_uint64(p, len - 1, val, &l)))
			return e;
		p -= l;
		*p = 0x80 | l;
		*size = l + 1;
		return 0;
	}
}

int
tb_der_put_bool(unsigned char *p, size_t len,
		const int *data, size_t *size)
{
	if (len < 1)
		return TBASN1_OVERFLOW;
	if (*data != 0)
		*p = 0xff;
	else
		*p = 0;
	*size = 1;
	return 0;
}

int
tb_der_put_str(unsigned char *p, size_t len, 
		const tb_general_string *str, size_t *size)
{
	size_t slen = 0;
	if (str && *str)
		slen = strlen(*str);

	if (len < slen)
		return TBASN1_OVERFLOW;
	p -= slen;
	len -= slen;
	if (str)
		memcpy(p+1, *str, slen);
	*size = slen;
	return 0;
}

int
tb_der_put_bytes(unsigned char *p, size_t len, 
		const tb_octet_string *data, size_t *size)
{
	if (len < data->length)
		return TBASN1_OVERFLOW;
	p -= data->length;
	len -= data->length;
	memcpy(p+1, data->data, data->length);
	*size = data->length;
	return 0;
}

int
tb_der_put_oid(unsigned char *p, size_t len,
		const tb_oid *data, size_t *size)
{
	unsigned char *base = p;
	size_t n;

	if (data->length < 1)
		return TBASN1_OVERFLOW;
	for (n = data->length - 1; n >= 2; --n) {
		unsigned u = data->components[n];

		if (len < 1)
			return TBASN1_OVERFLOW;
		*p-- = u % 128;
		u /= 128;
		--len;
		while (u > 0) {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = 128 + u % 128;
			u /= 128;
			--len;
		}
	}
	if (len < 1)
		return TBASN1_OVERFLOW;
	*p-- = 40 * data->components[0] + data->components[1];
	*size = base - p;
	return 0;
}

int
tb_der_put_tag(unsigned char *p, size_t len, Der_class class, Der_type type,
		int tag, size_t *size)
{
	if (len < 1)
		return TBASN1_OVERFLOW;
	if (tag < 0x1F) {
		/* low-tag-number form */
		*p = (class << 6) | (type << 5) | tag; /* XXX */
		*size = 1;
	} else {
		unsigned char *base = p;

		/*
		 * high-tag-number form
		 * bits 5-1 have value "1", tag is encoded base-128 with bit8 set on
		 * all but the last octet
		 */
		*p-- = tag % 128;
		tag /= 128;
		--len;
		while (tag > 0) {
			if (len < 1)
				return TBASN1_OVERFLOW;
			*p-- = 0x80 + tag % 128;
			tag /= 128;
			--len;
		}
		if (len < 1)
			return TBASN1_OVERFLOW;
		*p-- = (class << 6) | (type << 5) | 0x1F;
		*size = base - p;
		return 0;
	}
	return 0;
}

int
tb_der_put_len_and_tag(unsigned char *p, size_t len, size_t len_val,
		Der_class class, Der_type type, int tag, size_t *size)
{
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_len(p, len, len_val, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_tag(p, len, class, type, tag, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_bool(unsigned char *p, size_t len,
		const int *data, size_t *size)
{
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_bool(p, len, data, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag(p, len, l, ASN1_C_UNIV, PRIM, UT_Boolean, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_int32(unsigned char *p, size_t len,
		const int32_t *data, size_t *size)
{
	int32_t num = *data;
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_int32(p, len, num, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag(p, len, l, ASN1_C_UNIV, PRIM, UT_Integer, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_int64(unsigned char *p, size_t len,
		const int64_t *data, size_t *size)
{
	int64_t num = *data;
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_int64(p, len, num, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag(p, len, l, ASN1_C_UNIV, PRIM, UT_Integer, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_uint32(unsigned char *p, size_t len,
		const uint32_t *data, size_t *size)
{
	uint32_t num = *data;
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_uint32(p, len, num, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	/* if first octet has msb set, we need to pad with a zero byte */
	if (p[1] >= 128) {
		if (len == 0)
			return TBASN1_OVERFLOW;
		*p-- = 0;
		len--;
		ret++;
		l++;
	}
	if ((e = tb_der_put_len_and_tag(p, len, l, ASN1_C_UNIV, PRIM, UT_Integer, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_uint64(unsigned char *p, size_t len,
		const uint64_t *data, size_t *size)
{
	uint64_t num = *data;
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_uint64(p, len, num, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	/* if first octet has msb set, we need to pad with a zero byte */
	if (p[1] >= 128) {
		if (len == 0)
			return TBASN1_OVERFLOW;
		*p-- = 0;
		len--;
		ret++;
		l++;
	}
	if ((e = tb_der_put_len_and_tag(p, len, l, ASN1_C_UNIV, PRIM, UT_Integer, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_enum(unsigned char *p, size_t len,
		const unsigned int *data, size_t *size)
{
	unsigned int num = *data;
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_uint32(p, len, num, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag (p, len, l, ASN1_C_UNIV, PRIM, UT_Enumerated, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_str(unsigned char *p, size_t len, 
		const tb_general_string *data, size_t *size)
{
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_str(p, len, data, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag (p, len, l, ASN1_C_UNIV, PRIM, UT_GeneralString, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_bytes(unsigned char *p, size_t len, 
		const tb_octet_string *k, size_t *size)
{
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_bytes(p, len, k, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag(p, len, l, ASN1_C_UNIV, PRIM, UT_OctetString, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
enc_oid(unsigned char *p, size_t len,
		const tb_oid *k, size_t *size)
{
	size_t ret = 0;
	size_t l;
	int e;

	if ((e = tb_der_put_oid(p, len, k, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag (p, len, l, ASN1_C_UNIV, PRIM, UT_OID, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}

int
time2generaltime(time_t t, tb_octet_string *s)
{
	struct tm *tm;
	size_t len;

	len = 15;

	if ((s->data = malloc(len + 1)) == NULL)
		return ENOMEM;
	s->length = len;
	tm = gmtime(&t);
	snprintf(s->data, len + 1, "%04d%02d%02d%02d%02d%02dZ", 
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
			tm->tm_hour, tm->tm_min, tm->tm_sec);
	return 0;
}

int
enc_time(unsigned char *p, size_t len,
		const time_t *t, size_t *size)
{
	size_t ret = 0;
	size_t l;
	tb_octet_string k;
	int e;

	if ((e = time2generaltime(*t, &k)))
		return e;
	e = tb_der_put_bytes(p, len, &k, &l);
	free(k.data);
	if (e)
		return e;
	p -= l;
	len -= l;
	ret += l;
	if ((e = tb_der_put_len_and_tag(p, len, k.length, ASN1_C_UNIV, PRIM, 
			UT_GeneralizedTime, &l)))
		return e;
	p -= l;
	len -= l;
	ret += l;
	*size = ret;
	return 0;
}
