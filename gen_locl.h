/* $Gateweaver: gen_locl.h,v 1.5 2006/04/08 21:28:30 cmaxwell Exp $ */
/* $KTH: gen_locl.h,v 1.9 2001/09/27 16:21:47 assar Exp $ */
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

#ifndef __GEN_LOCL_H__
#define __GEN_LOCL_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <err.h>
#include "hash.h"
#include "symbol.h"

void generate_type(const Symbol *);
void generate_constant(const Symbol *);
void generate_type_encode(const Symbol *);
void generate_type_decode(const Symbol *);
void generate_seq_type_decode(const Symbol *);
void generate_type_free(const Symbol *);
void generate_type_length(const Symbol *);
void generate_type_copy(const Symbol *);
void generate_type_maybe(const Symbol *);
void generate_type_add(const Symbol *);
void generate_glue(const Symbol *);

void init_generate(const char *, const char *);
const char *filename(void);
void close_generate(void);
void add_import(const char *);
int yyparse(void);

#define TYPE_NAME	0
#define TYPE_REAL	1
#define TYPE_TEXT	2
const char * get_typestring(const Type *, int);

extern FILE *headerfile, *codefile, *logfile;
extern const char *external_parser;

#endif /* __GEN_LOCL_H__ */
