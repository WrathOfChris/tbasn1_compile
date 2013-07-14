/* $Gateweaver: gen.c,v 1.14 2006/08/18 18:56:55 cmaxwell Exp $ */
/* $KTH: gen.c,v 1.57 2004/03/07 12:44:00 lha Exp $ */
/*
 * Copyright (c) 2005 Christopher Maxwell.  All rights reserved.
 */
/*
 * Copyright (c) 1997 - 2002 Kungliga Tekniska Högskolan
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

FILE *headerfile, *codefile, *logfile;

#define STEM "asn1"

static const char *orig_filename;
static char *header;
static char *headerbase;

/*
 * list of all IMPORTs
 */

struct import {
    const char *module;
    struct import *next;
};

static struct import *imports = NULL;

void
add_import(const char *module)
{
    struct import *tmp = malloc(sizeof(*tmp));
    if (tmp == NULL)
		err(1, NULL);

    tmp->module = module;
    tmp->next   = imports;
    imports     = tmp;
}

const char *
filename(void)
{
    return orig_filename;
}

void
init_generate(const char *filename, const char *base)
{
    orig_filename = filename;
    if (base)
		asprintf(&headerbase, "%s", base);
    else
		headerbase = strdup(STEM);
    asprintf(&header, "%s.h", headerbase);
    if (header == NULL)
        err(1, NULL);

    headerfile = fopen(header, "w");
    if (headerfile == NULL)
		err (1, "open %s", header);
    fprintf(headerfile,
			"/* Generated from %s */\n"
			"/* Do not edit */\n\n",
			filename);
    fprintf(headerfile, 
			"#ifndef __%s_h__\n"
			"#define __%s_h__\n\n", headerbase, headerbase);
    fprintf(headerfile, 
			"#include <stddef.h>\n"
			"#include <time.h>\n\n");
	fprintf(headerfile,
			"#ifndef __toolbox_asn1_definitions__\n"
			"#define __toolbox_asn1_definitions__\n\n");
	fprintf(headerfile,
			"typedef struct tb_octet_string {\n"
			"\tsize_t length;\n"
			"\tvoid *data;\n"
			"} tb_octet_string;\n\n");
	fprintf(headerfile,
			"typedef char *tb_general_string;\n\n"
		   );
	fprintf(headerfile,
			"typedef char *tb_utf8_string;\n\n"
		   );
	fprintf(headerfile,
			"typedef struct tb_oid {\n"
			"\tsize_t length;\n"
			"\tunsigned *components;\n"
			"} tb_oid;\n\n");
	fprintf(headerfile,
			"typedef struct tbunits {\n"
			"\tchar *name;\n"
			"\tunsigned int mult;\n"
			"} tbunits;\n\n");
	fprintf(headerfile,
			"#define TBASN1_MALLOC_ENCODE(T, B, BL, S, L, R)               \\\n"
			"do {                                                          \\\n"
			"    (BL) = len_##T((S));                                      \\\n"
			"    (B) = malloc((BL));                                       \\\n"
			"    if ((B) == NULL)                                          \\\n"
			"        (R) = ENOMEM;                                         \\\n"
			"    else {                                                    \\\n"
			"        (R) = enc_##T(((unsigned char*)(B)) + (BL) - 1, (BL), \\\n"
			"                         (S), (L));                           \\\n"
			"        if ((R) != 0) {                                       \\\n"
			"            free((B));                                        \\\n"
			"            (B) = NULL;                                       \\\n"
			"        }                                                     \\\n"
			"    }                                                         \\\n"
			"} while (0)\n\n");
    fprintf(headerfile, "#endif\n\n");
    logfile = fopen(STEM "_files", "w");
    if (logfile == NULL)
		err (1, "open " STEM "_files");
}

void
close_generate(void)
{
    fprintf(headerfile, "#endif /* __%s_h__ */\n", headerbase);

    fclose(headerfile);
    fprintf(logfile, "\n");
    fclose(logfile);
}

void
generate_constant(const Symbol *s)
{
	fprintf(headerfile, "enum { %s = %d };\n\n",
			s->gen_name, s->constant);
}

const char *
get_typestring(const Type *t, int real)
{
	switch (t->type) {
		case TInteger:		return real ? "int32_t" : "int32";
		case TUInteger:		return real ? "uint32_t" : "uint32";
		case TInteger64:	return real ? "int64_t" : "int64";
		case TUInteger64:	return real ? "uint64_t" : "uint64";
		case TBoolean:		return "BOOLEAN";
		case TEnumerated:	return "ENUM";

		case TSequence:
			return real == TYPE_TEXT ? "SEQUENCE" : t->symbol->gen_name;
		case TType:
			return real == TYPE_TEXT
				? get_typestring(t->symbol->type, real)
				: t->symbol->gen_name;

		case TOctetString:
			if (real == TYPE_TEXT || real == TYPE_REAL)
				return "OCTETSTRING";
			return NULL;
		case TGeneralString:
			if (real == TYPE_TEXT || real == TYPE_REAL)
				return "STRING";
			return NULL;
		case TUTF8String:
			if (real == TYPE_TEXT || real == TYPE_REAL)
				return "UTF8STRING";
			return NULL;
		case TApplication:		return real == TYPE_TEXT ? "APPLICATION" : NULL;
		case TOID:				return real == TYPE_TEXT ? "OID" : "tb_oid";
		case TBitString:		return real == TYPE_TEXT ? "BITSTRING" : NULL;
		case TSequenceOf:		return real == TYPE_TEXT ? "SEQUENCEOF" : NULL;
		case TGeneralizedTime:	return real == TYPE_TEXT ? "TIME" : NULL;
		case TNull:				return real == TYPE_TEXT ? "NULL" : NULL;
	}
}

static void
space(int level)
{
    while(level-- > 0)
		fprintf(headerfile, "  ");
}

static void
define_asn1 (int level, Type *t)
{
	switch (t->type) {
		case TType:
			space(level);
			fprintf(headerfile, "%s", t->symbol->name);
			break;
		case TInteger:
			space(level);
			fprintf(headerfile, "INTEGER");
			break;
		case TUInteger:
			space(level);
			fprintf(headerfile, "UNSIGNED INTEGER");
			break;
		case TInteger64:
			space(level);
			fprintf(headerfile, "INTEGER64");
			break;
		case TUInteger64:
			space(level);
			fprintf(headerfile, "UNSIGNED INTEGER64");
			break;
		case TOctetString:
			space(level);
			fprintf(headerfile, "OCTET STRING");
			break;
		case TOID : {
			Member *m;
			char *tag = NULL;

			space(level);
			fprintf(headerfile, "OBJECT IDENTIFIER {\n");
			for (m = t->members; m && m->name != tag; m = m->next) {
				if (tag == NULL)
					tag = m->name;
				space(level + 1);
				if (m->name == NULL)
					fprintf(headerfile, "%d\n", m->val);
				else
					fprintf(headerfile, "%s(%d)\n", m->name, m->val);
			}
			space(level);
			fprintf (headerfile, "}");
			break;
		}
		case TBitString: {
			Member *m;
			int tag = -1;

			space(level);
			fprintf(headerfile, "BIT STRING {\n");
			for (m = t->members; m && m->val != tag; m = m->next) {
				if (tag == -1)
					tag = m->val;
				space(level + 1);
				fprintf(headerfile, "%s(%d)%s\n", m->name, m->val, 
						m->next->val == tag?"":",");
			}
			space(level);
			fprintf (headerfile, "}");
			break;
		}
		case TEnumerated: {
			Member *m;
			int tag = -1;

			space(level);
			fprintf(headerfile, "ENUMERATED {\n");
			for (m = t->members; m && m->val != tag; m = m->next) {
				if (tag == -1)
					tag = m->val;
				space(level + 1);
				fprintf (headerfile, "%s(%d)%s\n", m->name, m->val, 
						m->next->val == tag?"":",");
			}
			space(level);
			fprintf(headerfile, "}");
			break;
		}
		case TSequence: {
			Member *m;
			int tag;
			int max_width = 0;

			space(level);
			fprintf(headerfile, "SEQUENCE {\n");
			for (m = t->members, tag = -1; m && m->val != tag; m = m->next) {
				if (tag == -1)
					tag = m->val;
				if (strlen(m->name) + (m->val > 9) > max_width)
					max_width = strlen(m->name) + (m->val > 9);
			}
			max_width += 3 + 2;
			if (max_width < 16)
				max_width = 16;
			for (m = t->members, tag = -1 ; m && m->val != tag; m = m->next) {
				int width;
				if (tag == -1)
					tag = m->val;
				space(level + 1);
				fprintf(headerfile, "%s[%d]", m->name, m->val);
				width = max_width - strlen(m->name) - 3 - (m->val > 9) - 2;
				fprintf(headerfile, "%*s", width, "");
				define_asn1(level + 1, m->type);
				if (m->optional)
					fprintf(headerfile, " OPTIONAL");
				if (m->next->val != tag)
					fprintf (headerfile, ",");
				fprintf(headerfile, "\n");
			}
			space(level);
			fprintf (headerfile, "}");
			break;
		}
		case TSequenceOf: {
			space(level);
			fprintf(headerfile, "SEQUENCE OF ");
			define_asn1(0, t->subtype);
			break;
		}
		case TGeneralizedTime:
			space(level);
			fprintf (headerfile, "GeneralizedTime");
			break;
		case TGeneralString:
			space(level);
			fprintf(headerfile, "GeneralString");
			break;
		case TApplication:
			fprintf(headerfile, "[APPLICATION %d] ", t->application);
			define_asn1(level, t->subtype);
			break;
		case TBoolean:
			space(level);
			fprintf (headerfile, "BOOLEAN");
			break;
		case TUTF8String:
			space(level);
			fprintf(headerfile, "UTF8String");
			break;
		case TNull:
			space(level);
			fprintf (headerfile, "NULL");
			break;
		default:
			abort ();
	}
}

static void
define_type(int level, const char *name, Type *t, int typedefp)
{
    switch (t->type) {
		case TType:
			space(level);
			fprintf(headerfile, "%s %s;\n", t->symbol->gen_name, name);
			break;
		case TInteger:
			space(level);
			if (t->members == NULL) {
				fprintf (headerfile, "int32_t %s;\n", name);
			} else {
				Member *m;
				int tag = -1;
				fprintf(headerfile, "enum %s {\n", typedefp ? name : "");
				for (m = t->members; m && m->val != tag; m = m->next) {
					if (tag == -1)
						tag = m->val;
					space (level + 1);
					fprintf(headerfile, "%s = %d%s\n", m->gen_name, m->val, 
							m->next->val == tag ? "" : ",");
				}
				fprintf (headerfile, "} %s;\n", name);
			}
			break;
		case TUInteger:
			space(level);
			fprintf (headerfile, "uint32_t %s;\n", name);
			break;
		case TInteger64:
			space(level);
			fprintf (headerfile, "int64_t %s;\n", name);
			break;
		case TUInteger64:
			space(level);
			fprintf (headerfile, "uint64_t %s;\n", name);
			break;
		case TOctetString:
			space(level);
			fprintf (headerfile, "tb_octet_string %s;\n", name);
			break;
		case TOID :
			space(level);
			fprintf(headerfile, "tb_oid %s;\n", name);
			break;
		case TBitString: {
			Member *m;
			Type i;
			int tag = -1;

			i.type = TUInteger;
			space(level);
			fprintf(headerfile, "struct %s {\n", typedefp ? name : "");
			for (m = t->members; m && m->val != tag; m = m->next) {
				char *n;

				asprintf(&n, "%s:1", m->gen_name);
				define_type(level + 1, n, &i, FALSE);
				free(n);
				if (tag == -1)
					tag = m->val;
			}
			space(level);
			fprintf (headerfile, "} %s;\n\n", name);
			break;
		}
		case TEnumerated: {
			Member *m;
			int tag = -1;

			space(level);
			fprintf(headerfile, "enum %s {\n", typedefp ? name : "");
			for (m = t->members; m && m->val != tag; m = m->next) {
				if (tag == -1)
					tag = m->val;
				space(level + 1);
				fprintf(headerfile, "%s = %d%s\n", m->gen_name, m->val,
						m->next->val == tag ? "" : ",");
			}
			space(level);
			fprintf (headerfile, "} %s;\n\n", name);
			break;
		}
		case TSequence: {
			Member *m;
			int tag = -1;

			space(level);
			fprintf(headerfile, "struct %s {\n", typedefp ? name : "");
			for (m = t->members; m && m->val != tag; m = m->next) {
				if (m->optional) {
					char *n;

					asprintf(&n, "*%s", m->gen_name);
					define_type(level + 1, n, m->type, FALSE);
					free(n);
				} else
					define_type(level + 1, m->gen_name, m->type, FALSE);
				if (tag == -1)
					tag = m->val;
			}
			space(level);
			fprintf (headerfile, "} %s;\n", name);
			break;
		}
		case TSequenceOf: {
			Type i;

			i.type = TUInteger;
			i.application = 0;

			space(level);
			fprintf (headerfile, "struct %s {\n", typedefp ? name : "");
			define_type (level + 1, "len", &i, FALSE);
			define_type (level + 1, "*val", t->subtype, FALSE);
			space(level);
			fprintf (headerfile, "} %s;\n", name);
			break;
		}
		case TGeneralizedTime:
			space(level);
			fprintf (headerfile, "time_t %s;\n", name);
			break;
		case TGeneralString:
			space(level);
			fprintf (headerfile, "tb_general_string %s;\n", name);
			break;
		case TUTF8String:
			space(level);
			fprintf (headerfile, "tb_utf8_string %s;\n", name);
			break;
		case TBoolean:
			space(level);
			fprintf (headerfile, "int %s;\n", name);
			break;
		case TNull:
			space(level);
			fprintf (headerfile, "NULL %s;\n", name);
			break;
		case TApplication:
			define_type (level, name, t->subtype, FALSE);
			break;
		default:
			abort ();
	}
}

static void
generate_type_external(FILE *ext, const Type *t)
{
	Member *m;
	int tag;
	char subtype[256];

	switch (t->type) {
		case TBitString:
		case TEnumerated:
			tag = -1;
			for (m = t->members; m && m->val != tag; m = m->next) {
				if (tag == -1)
					tag = m->val;
				fprintf(ext, "%s %d\n", m->name, m->val);
			}
			break;

		case TSequence:
			tag = -1;
			for (m = t->members; m && m->val != tag; m = m->next) {
				if (tag == -1)
					tag = m->val;
				strlcpy(subtype, get_typestring(m->type, TYPE_TEXT),
						sizeof(subtype));
				fprintf(ext, "%s %s %d %s %s\n",
						get_typestring(m->type, TYPE_REAL), m->name, m->val,
						m->optional ? "OPTIONAL" : "REQUIRED",
						subtype);
			}
			break;

		case TApplication:
			generate_type_external(ext, t->subtype);
			break;

		default:
			break;
	}

}

static void
generate_type_header(const Symbol *s)
{
    fprintf(headerfile, "/*\n");
    fprintf(headerfile, "%s ::= ", s->name);
    define_asn1(0, s->type);
    fprintf(headerfile, "\n*/\n\n");

    fprintf(headerfile, "typedef ");
    define_type(0, s->gen_name, s->type, TRUE);

    fprintf(headerfile, "\n");
}


void
generate_type(const Symbol *s)
{
    struct import *i;
    char *filename;

    if (asprintf(&filename, "%s_%s.x", STEM, s->gen_name) == -1)
		err (1, NULL);
	codefile = fopen(filename, "w");
    if (codefile == NULL)
		err (1, "fopen %s", filename);
    fprintf(logfile, "%s ", filename);
    free(filename);
    fprintf (codefile, 
			"/* Generated from %s */\n"
			"/* Do not edit */\n\n"
			"#include <stdio.h>\n"
			"#include <stdlib.h>\n"
			"#include <time.h>\n"
			"#include <string.h>\n"
			"#include <errno.h>\n",
			orig_filename);

    for (i = imports; i != NULL; i = i->next)
		fprintf (codefile,
				"#include <%s_asn1.h>\n",
				i->module);
	fprintf (codefile,
			"#include <%s.h>\n",
			headerbase);
	fprintf (codefile,
			"#include <toolbox.h>\n"
			"#include <toolbox_asn1_err.h>\n"
			"#include <toolbox_asn1_der.h>\n"
			"\n");

	if (s->stype == Stype && s->type->type == TChoice) {
		fprintf(codefile,
				"/* CHOICE */\n"
				"int asn1_%s_dummy_holder = 1;\n", s->gen_name);
	} else {
		generate_type_header(s);
		generate_type_encode(s);
		generate_type_decode(s);
		generate_type_free(s);
		generate_type_length(s);
		generate_type_copy(s);
		generate_type_add(s);
		generate_type_del(s);
		generate_glue(s);
	}
	fprintf(headerfile, "\n\n");
	fclose(codefile);

	if (external_parser) {
		const char *args[6];
		char tmpfile[15] = "ext.XXXXXXXXXX";
		int fd, exitcode = -1;
		FILE *temp;

		if ((fd = mkstemp(tmpfile)) == -1 ||
				(temp = fdopen(fd, "w+")) == NULL) {
			if (fd != -1) {
				unlink(tmpfile);
				close(fd);
			}
			fprintf(stderr, "cannot create external \"%s\": %s\n", tmpfile,
					strerror(errno));
			exit(1);
		}
		generate_type_external(temp, s->type);
		fclose(temp);

		args[0] = external_parser;
		args[1] = s->gen_name;		/* typedef struct name */
		args[2] = tmpfile;
		args[3] = get_typestring(s->type, TYPE_TEXT);
		args[4] = NULL;

		switch (vfork()) {
			case -1:
				fprintf(stderr, "can not execute \"%s\": %s\n", strerror(errno));
				unlink(tmpfile);
				exit(1);
				break;
			case 0:
				execv(*args, args);
				fprintf(stderr, "did not execute \"%s\": %s\n", strerror(errno));
				unlink(tmpfile);
				exit(1);
				break;
			default:
				wait(&exitcode);
				if (exitcode != 0)
					fprintf(stderr, "external parser failed with code %d\n",
							exitcode);
				break;
		}
		unlink(tmpfile);
	}
}
