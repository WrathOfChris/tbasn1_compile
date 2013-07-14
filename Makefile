# $Gateweaver: Makefile,v 1.7 2007/02/27 22:00:30 cmaxwell Exp $
PROG=	tbasn1_compile
SRCS=	parse.c lex.l main.c hash.c symbol.c gen.c \
        gen_encode.c gen_decode.c gen_free.c gen_length.c gen_copy.c \
        gen_glue.c gen_add.c gen_del.c
SRCS+=	der_length.c der_put.c
NOMAN=1

CFLAGS+=-I. -I$(.CURDIR) -DFALSE=0 -DTRUE=1

# Allow constants defined within an enumeration to act as ASN.1 constants
CFLAGS+=-DENUM_CONSTANTS

#DPADD+=${LIBTOOLBOX}
#LDADD+=-ltoolbox

CLEANFILES=	parse.[ch] lex.c

parse.h parse.c:	parse.y
	${YACC} -d -o parse.c ${.ALLSRC}

.include <bsd.prog.mk>
