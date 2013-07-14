/* $Gateweaver: toolbox_asn1.h,v 1.7 2006/04/13 17:19:09 cmaxwell Exp $ */
#include <stddef.h>
#include <time.h>

#ifndef __toolbox_asn1_definitions__
#define __toolbox_asn1_definitions__

typedef struct tb_octet_string {
    size_t length;
    void *data;
} tb_octet_string;

typedef char *tb_general_string;
typedef char *tb_utf8_string;

typedef struct tb_oid {
    size_t length;
    unsigned *components;
} tb_oid;

typedef struct tbunits {
	char *name;
	unsigned int mult;
} tbunits;


#define TBASN1_MALLOC_ENCODE(T, B, BL, S, L, R)               \
do {                                                          \
    (BL) = len_##T((S));                                      \
    (B) = malloc((BL));                                       \
    if ((B) == NULL)                                          \
        (R) = ENOMEM;                                         \
    else {                                                    \
        (R) = enc_##T(((unsigned char*)(B)) + (BL) - 1, (BL), \
                         (S), (L));                           \
        if ((R) != 0) {                                       \
            free((B));                                        \
            (B) = NULL;                                       \
        }                                                     \
    }                                                         \
} while (0)

#endif

const char * tbox_units(int, const tbunits *);
const char * tbox_units_short(int, const tbunits *);
int tbox_units_parse(const char *, const tbunits *, int *);
