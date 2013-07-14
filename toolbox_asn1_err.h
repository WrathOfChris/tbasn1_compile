/* Generated from toolbox_asn1_err.et */
/* $Gateweaver: toolbox_asn1_err.et,v 1.4 2005/12/30 03:29:38 cmaxwell Exp $ */

#ifndef __toolbox_asn1_err_h__
#define __toolbox_asn1_err_h__

struct et_list;

void initialize_tasn_error_table_r(struct et_list **);

void initialize_tasn_error_table(void);
#define init_tasn_err_tbl initialize_tasn_error_table

typedef enum tasn_error_number{
	TBASN1_BAD_TIMEFORMAT = -1178900480,
	TBASN1_MISSING_FIELD = -1178900479,
	TBASN1_MISPLACED_FIELD = -1178900478,
	TBASN1_TYPE_MISMATCH = -1178900477,
	TBASN1_OVERFLOW = -1178900476,
	TBASN1_OVERRUN = -1178900475,
	TBASN1_BAD_ID = -1178900474,
	TBASN1_BAD_LENGTH = -1178900473,
	TBASN1_BAD_FORMAT = -1178900472,
	TBASN1_PARSE_ERROR = -1178900471,
	TBASN1_NOTFOUND = -1178900470
} tasn_error_number;

#define ERROR_TABLE_BASE_tasn -1178900480

#endif /* __toolbox_asn1_err_h__ */
