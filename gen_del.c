/* $Gateweaver: gen_del.c,v 1.4 2006/04/08 21:28:30 cmaxwell Exp $ */
/*
 * Copyright (c) 2005 Christopher Maxwell.  All rights reserved.
 */

#include "gen_locl.h"

static void
del_this_type(const char *name, const char *obj, const Type *t)
{
	fprintf(codefile, "\tunsigned int u;\n");
	fprintf(codefile, "\t%s *this;\n", get_typestring(t->subtype, TYPE_REAL));

	fprintf(codefile, "\tif (!%s) return TBOX_SUCCESS;\n", name);
	fprintf(codefile, "\tif (%s->len <= 0) return TBASN1_NOTFOUND;\n", name);

	fprintf(codefile, "\tfor (u = 0; u < %s->len; u++) {\n", name);
	fprintf(codefile, "\t\tif (&%s->val[u] != %s) continue;\n", name, obj);
	fprintf(codefile, "\t\t%s->len--;\n", name);

	fprintf(codefile, "\t\tif (%s->len == 0) {\n", name);
	fprintf(codefile, "\t\t\tfree(%s->val);\n", name);
	fprintf(codefile, "\t\t\t%s->val = NULL;\n", name);
	fprintf(codefile, "\t\t} else {\n");

	fprintf(codefile, "\t\t\tbcopy(&%s->val[u + 1], &%s->val[u], "
			"sizeof(*%s->val) * (%s->len - u));\n",
			name, name, name, name);
	fprintf(codefile, "\t\t\tif ((this = realloc(%s->val, "
			"%s->len * sizeof(*%s->val))) == NULL) return TBOX_SUCCESS;\n",
			name, name, name);
	fprintf(codefile, "\t\t\t%s->val = this;\n", name);
	fprintf(codefile, "\t\t}\n");
	fprintf(codefile, "\t\treturn TBOX_SUCCESS;\n");

	fprintf(codefile, "\t}\n");
	fprintf(codefile, "\treturn TBASN1_NOTFOUND;\n");
}

void
generate_type_del(const Symbol *s)
{
	const char *name = NULL;
    if (s->type->type == TSequenceOf) {
		name = get_typestring(s->type->subtype, TYPE_REAL);

		fprintf(headerfile,
				"int del_%s(%s *, %s *);\n",
				get_typestring(s->type->subtype, TYPE_NAME), s->gen_name, name);

		fprintf(codefile,
				"int\n"
				"del_%s(%s *data, %s *del)\n"
				"{\n",
				get_typestring(s->type->subtype, TYPE_NAME), s->gen_name, name);

		del_this_type("data", "del", s->type);
		fprintf(codefile, "}\n\n");
    }
}
