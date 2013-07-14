/* $Gateweaver: main.c,v 1.4 2006/04/08 21:14:10 cmaxwell Exp $ */
/* $KTH: main.c,v 1.11 2001/02/20 01:44:52 assar Exp $ */
/*
 * Copyright (c) 2005 Christopher Maxwell.  All rights reserved.
 */
/*
 * Copyright (c) 1997, 1998, 1999 Kungliga Tekniska H�gskolan
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

extern FILE *yyin;
extern const char *__progname;
const char *external_parser = NULL;

int version_flag;
int help_flag;

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-x extparser] [asn1-file [name]]", __progname);
	exit(1);
}

int
main(int argc, char **argv)
{
    int ret, ch;
    const char *file;
    const char *name = NULL;

	while ((ch = getopt(argc, argv, "x:")) != -1) {
		switch (ch) {
			case 'x':
				external_parser = optarg;
				break;
			case '?':
			default:
				usage();
		}
	}
	argc -= optind;
	argv += optind;

    if (argc == 0) {
		file = "stdin";
		name = "stdin";
		yyin = stdin;
    } else {
		file = argv[0];
		yyin = fopen(file, "r");
		if (yyin == NULL)
			err (1, "open %s", file);
		name = argv[1];
    }

    init_generate(file, name);
    initsym();
    ret = yyparse();
    close_generate();
    return ret;
}
