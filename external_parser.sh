#!/bin/sh
#
# Just a demo of an external parser.
# 	$1	Type name
#	$2	File with contents
#	$3	Type type

echo >&2 "---\t$1\t$2\t$3"
cat >&2 <$2
