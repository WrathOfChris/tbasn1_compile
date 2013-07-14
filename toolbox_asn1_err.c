/* Generated from toolbox_asn1_err.et */
/* $Gateweaver: toolbox_asn1_err.et,v 1.4 2005/12/30 03:29:38 cmaxwell Exp $ */

#include <stddef.h>
#include <com_err.h>
#include "toolbox_asn1_err.h"

static const char *tasn_error_strings[] = {
	/* 000 */ "ASN.1 failed call to system time library",
	/* 001 */ "ASN.1 structure is missing a required field",
	/* 002 */ "ASN.1 unexpected field number",
	/* 003 */ "ASN.1 type numbers are inconsistent",
	/* 004 */ "ASN.1 value too large",
	/* 005 */ "ASN.1 encoding ended unexpectedly",
	/* 006 */ "ASN.1 identifier doesn't match expected value",
	/* 007 */ "ASN.1 length doesn't match expected value",
	/* 008 */ "ASN.1 badly-formatted encoding",
	/* 009 */ "ASN.1 parse error",
	/* 010 */ "ASN.1 requested element not found",
	NULL
};

#define num_errors 11

void initialize_tasn_error_table_r(struct et_list **list)
{
    initialize_error_table_r(list, tasn_error_strings, num_errors, ERROR_TABLE_BASE_tasn);
}

void initialize_tasn_error_table(void)
{
    init_error_table(tasn_error_strings, ERROR_TABLE_BASE_tasn, num_errors);
}
