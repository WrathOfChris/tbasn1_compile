/* $Gateweaver: gen_add.c,v 1.4 2006/04/08 21:28:30 cmaxwell Exp $ */
/*
 * Copyright (c) 2005 Christopher Maxwell.  All rights reserved.
 */

#include "gen_locl.h"

static void
add_this_type(const char *name, const Type *t)
{
	fprintf(codefile, "\t%s *this;\n", get_typestring(t->subtype, TYPE_REAL));

	fprintf(codefile, "\tif (!%s) return NULL;\n", name);

	fprintf(codefile, "\tif ((this = realloc(%s->val, "
			"(%s->len + 1) * sizeof(*%s->val))) == NULL) return NULL;\n",
			name, name, name);
	fprintf(codefile, "\t%s->len++;\n", name);
	fprintf(codefile, "\t%s->val = this;\n", name);
	fprintf(codefile, "\tbzero(&%s->val[%s->len - 1], sizeof(*%s->val));\n",
			name, name, name);
	fprintf(codefile, "\treturn &%s->val[%s->len - 1];\n",
			name, name);
}

void
generate_type_add(const Symbol *s)
{
	const char *name = NULL;
    if (s->type->type == TSequenceOf) {
		name = get_typestring(s->type->subtype, TYPE_REAL);

		fprintf(headerfile,
				"%s * add_%s(%s *);\n",
				name, get_typestring(s->type->subtype, TYPE_NAME), s->gen_name);

		fprintf(codefile,
				"%s *\n"
				"add_%s(%s *data)\n"
				"{\n",
				name, get_typestring(s->type->subtype, TYPE_NAME), s->gen_name);

		add_this_type("data", s->type);
		fprintf(codefile, "}\n\n");
    }
}
