/* $Gateweaver: gen_decode.c,v 1.5 2006/02/05 02:10:43 cmaxwell Exp $ */
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

#include "gen_locl.h"

/*
RCSID("$KTH: gen_decode.c,v 1.20 2003/10/03 00:27:46 lha Exp $");
*/

static void
decode_primitive(const char *typename, const char *name)
{
	fprintf (codefile,
			"e = dec_%s(p, len, %s, &l);\n"
			"FORW;\n",
			typename, name);
}

static void
decode_type (const char *name, const Type *t)
{
	switch (t->type) {
		case TType:
#if 0
			decode_type (name, t->symbol->type);
#endif
			fprintf (codefile,
					"e = dec_%s(p, len, %s, &l);\n"
					"FORW;\n",
					t->symbol->gen_name, name);
			break;
		case TInteger:
			if (t->members == NULL)
				decode_primitive("int32", name);
			else {
				char *s;
				asprintf(&s, "(int32_t *)%s", name);
				if (s == NULL)
					errx (1, "out of memory");
				decode_primitive("int32", s);
				free(s);
			}
			break;
		case TUInteger:
			decode_primitive("uint32", name);
			break;
		case TInteger64:
			decode_primitive("int64", name);
			break;
		case TUInteger64:
			decode_primitive("uint64", name);
			break;
		case TEnumerated:
			decode_primitive("enum", name);
			break;
		case TOctetString:
			decode_primitive("bytes", name);
			break;
		case TOID :
			decode_primitive("oid", name);
			break;
		case TBitString: {
			Member *m;
			int tag = -1;
			int pos;

			fprintf (codefile,
					"e = tb_der_match_tag_and_len(p, len, ASN1_C_UNIV, PRIM, "
					"UT_BitString, &reallen, &l);\n"
					"FORW;\n"
					"if (len < reallen)\n"
					"\treturn TBASN1_OVERRUN;\n"
					"p++;\n"
					"len--;\n"
					"reallen--;\n"
					"ret++;\n");
			pos = 0;
			for (m = t->members; m && tag != m->val; m = m->next) {
				while (m->val / 8 > pos / 8) {
					fprintf (codefile,
							"p++; len--; reallen--; ret++;\n");
					pos += 8;
				}
				fprintf (codefile,
						"%s->%s = (*p >> %d) & 1;\n",
						name, m->gen_name, 7 - m->val % 8);
				if (tag == -1)
					tag = m->val;
			}
			fprintf(codefile,
					"p += reallen; len -= reallen; ret += reallen;\n");
			break;
		}
		case TSequence: {
			Member *m;
			int tag = -1;

			if (t->members == NULL)
				break;

			fprintf(codefile,
					"e = tb_der_match_tag_and_len(p, len, ASN1_C_UNIV, CONS, "
					"UT_Sequence, &reallen, &l);\n"
					"FORW;\n"
					"{\n");

			fprintf(codefile,
					"int dce_fix = 0;\n"
					"if (reallen == ASN1_INDEFINITE) dce_fix = 1;\n"
					"else if (len < reallen) return TBASN1_BAD_FORMAT;\n"
					"else len = reallen;\n");

			for (m = t->members; m && tag != m->val; m = m->next) {
				char *s;

				asprintf (&s, "%s(%s)->%s", m->optional ? "" : "&", name, m->gen_name);
				if (0 && m->type->type == TType){
					if(m->optional)
						fprintf (codefile,
								"%s = malloc(sizeof(*%s));\n"
								"if (%s == NULL) return ENOMEM;\n", s, s, s);
					fprintf(codefile, 
							"e = tb_dec_seq_%s(p, len, %d, %d, %s, &l);\n",
							m->type->symbol->gen_name,
							m->val, 
							m->optional,
							s);
					if (m->optional)
						fprintf (codefile, 
								"if (e == TBASN1_MISSING_FIELD) {\n"
								"free(%s);\n"
								"%s = NULL;\n"
								"e = l = 0;\n"
								"}\n",
								s, s);

					fprintf (codefile, "FORW;\n");

				} else {
					fprintf (codefile, "{\n"
							"size_t newlen, oldlen;\n\n"
							"e = tb_der_match_tag(p, len, ASN1_C_CONTEXT, CONS, %d, &l);\n",
							m->val);
					fprintf (codefile,
							"if (e)\n");
					if (m->optional)
						/* XXX should look at e */
						fprintf (codefile,
								"%s = NULL;\n", s);
					else
						fprintf(codefile,
								"return e;\n");
					fprintf(codefile, 
							"else {\n");
					fprintf(codefile,
							"p += l;\n"
							"len -= l;\n"
							"ret += l;\n"
							"e = tb_der_get_len(p, len, &newlen, &l);\n"
							"FORW;\n"
							"{\n");
					fprintf(codefile,
							"int dce_fix = 0;\n"
							"oldlen = len;\n"
							"if (newlen == ASN1_INDEFINITE) dce_fix = 1;\n"
							"else if (len < newlen) return TBASN1_BAD_FORMAT;\n"
							"else len = newlen;\n");
					if (m->optional)
						fprintf(codefile,
								"%s = malloc(sizeof(*%s));\n"
								"if (%s == NULL) return ENOMEM;\n", s, s, s);
					decode_type(s, m->type);
					fprintf(codefile,
							"if (dce_fix){\n"
							"e = tb_der_match_tag_and_len(p, len, "
							"(Der_class)0, (Der_type)0, 0, &reallen, &l);\n"
							"FORW;\n"
							"}else \n"
							"len = oldlen - newlen;\n"
							"}\n"
							"}\n");
					fprintf(codefile,
							"}\n");
				}
				if (tag == -1)
					tag = m->val;
				free (s);
			}
			fprintf(codefile,
					"if (dce_fix){\n"
					"e = tb_der_match_tag_and_len(p, len, "
					"(Der_class)0, (Der_type)0, 0, &reallen, &l);\n"
					"FORW;\n"
					"}\n"
					"}\n");

			break;
		}
		case TSequenceOf: {
			char *n;

			fprintf(codefile,
					"e = tb_der_match_tag_and_len(p, len, ASN1_C_UNIV, CONS, "
					"UT_Sequence, &reallen, &l);\n"
					"FORW;\n"
					"if (len < reallen)\n"
					"return TBASN1_OVERRUN;\n"
					"len = reallen;\n");

			fprintf(codefile,
					"{\n"
					"size_t origlen = len;\n"
					"int oldret = ret;\n"
					"ret = 0;\n"
					"(%s)->len = 0;\n"
					"(%s)->val = NULL;\n"
					"while (ret < origlen) {\n"
					"(%s)->len++;\n"
					"(%s)->val = realloc((%s)->val, sizeof(*((%s)->val)) * (%s)->len);\n",
					name, name, name, name, name, name, name);
			asprintf(&n, "&(%s)->val[(%s)->len-1]", name, name);
			decode_type(n, t->subtype);
			fprintf(codefile, 
					"len = origlen - ret;\n"
					"}\n"
					"ret += oldret;\n"
					"}\n");
			free (n);
			break;
		}
		case TGeneralizedTime:
			decode_primitive("time", name);
			break;
		case TGeneralString:
			decode_primitive("str", name);
			break;
		case TUTF8String:
			decode_primitive("utf8", name);
			break;
		case TNull:
			fprintf(codefile,
					"e = dec_null(p, len, &l);\n"
					"FORW;\n");
			break;
		case TApplication:
			fprintf(codefile,
					"e = tb_der_match_tag_and_len(p, len, ASN1_C_APPL, CONS, %d, "
					"&reallen, &l);\n"
					"FORW;\n"
					"{\n",
					t->application);
			fprintf(codefile,
					"int dce_fix = 0;\n"
					"if (reallen == ASN1_INDEFINITE) dce_fix = 1;\n"
					"else if (len < reallen) return TBASN1_BAD_FORMAT;\n"
					"else len = reallen;\n");
			decode_type(name, t->subtype);
			fprintf(codefile,
					"if (dce_fix){\n"
					"e = tb_der_match_tag_and_len(p, len, "
					"(Der_class)0, (Der_type)0, 0, &reallen, &l);\n"
					"FORW;\n"
					"}\n"
					"}\n");

			break;
		case TBoolean:
			decode_primitive("bool", name);
			break;
		default:
			abort();
	}
}

void
generate_type_decode(const Symbol *s)
{
	fprintf(headerfile,
			"int    "
			"dec_%s(const unsigned char *, size_t, %s *, size_t *);\n",
			s->gen_name, s->gen_name);

	fprintf(codefile, "#define FORW "
			"if(e) goto fail; "
			"p += l; "
			"len -= l; "
			"ret += l\n\n");

	fprintf(codefile, "int\n"
			"dec_%s(const unsigned char *p,"
			" size_t len, %s *data, size_t *size)\n"
			"{\n",
			s->gen_name, s->gen_name);

	switch (s->type->type) {
		case TInteger:
		case TUInteger:
		case TInteger64:
		case TUInteger64:
		case TBoolean:
		case TOctetString:
		case TOID:
		case TGeneralizedTime:
		case TGeneralString:
		case TUTF8String:
		case TNull:
		case TEnumerated:
		case TBitString:
		case TSequence:
		case TSequenceOf:
		case TApplication:
		case TType:
			fprintf(codefile,
					"size_t ret = 0, reallen;\n"
					"size_t l;\n"
					"int e;\n\n");
			fprintf(codefile, "memset(data, 0, sizeof(*data));\n");
			fprintf(codefile, "reallen = 0;\n"); /* hack to avoid `unused variable' */

			decode_type("data", s->type);
			fprintf(codefile, 
					"if (size) *size = ret;\n"
					"return 0;\n");
			fprintf(codefile,
					"fail:\n"
					"free_%s(data);\n"
					"return e;\n",
					s->gen_name);
			break;
		default:
			abort ();
	}
	fprintf (codefile, "}\n\n");
}

void
generate_seq_type_decode (const Symbol *s)
{
	fprintf(headerfile,
			"int dec_seq_%s(const unsigned char *, size_t, int, int, "
			"%s *, size_t *);\n",
			s->gen_name, s->gen_name);

	fprintf(codefile, "int\n"
			"dec_seq_%s(const unsigned char *p, size_t len, int tag, "
			"int optional, %s *data, size_t *size)\n"
			"{\n",
			s->gen_name, s->gen_name);

	fprintf(codefile,
			"size_t newlen, oldlen;\n"
			"size_t l, ret = 0;\n"
			"int e;\n");

	fprintf(codefile,
			"e = tb_der_match_tag(p, len, ASN1_C_CONTEXT, CONS, tag, &l);\n"
			"if (e)\n"
			"return e;\n");
	fprintf(codefile, 
			"p += l;\n"
			"len -= l;\n"
			"ret += l;\n"
			"e = tb_der_get_len(p, len, &newlen, &l);\n"
			"if (e)\n"
			"return e;\n"
			"p += l;\n"
			"len -= l;\n"
			"ret += l;\n");
	fprintf(codefile,
			"oldlen = len;\n"
			"if (newlen == ASN1_INDEFINITE) dce_fix = 1;\n"
			"else if (len < newlen) return TBASN1_BAD_FORMAT;\n"
			"else len = newlen;\n");
	fprintf(codefile,
			"e = dec_%s(p, len, data, &l);\n"
			"if (e)\n"
			"return e;\n"
			"p += l;\n"
			"len -= l;\n"
			"ret += l;\n",
			s->gen_name);
	fprintf(codefile,
			"if (dce_fix) {\n"
			"size_t reallen;\n\n"
			"e = tb_der_match_tag_and_len(p, len, "
			"(Der_class)0, (Der_type)0, 0, &reallen, &l);\n"
			"if (e)\n"
			"return e;\n"
			"ret += l;\n"
			"}\n");
	fprintf(codefile, 
			"if (size) *size = ret;\n"
			"return 0;\n");

	fprintf(codefile, "}\n\n");
}
