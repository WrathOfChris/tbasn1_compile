tbasn1_compile
==============

ASN.1 compiler extended from KTH/Stacken asn1_compile

- implements dce_fix code to adjust lengths correctly
- renames variables to not conflict with real asn1_compile
- add ENUM_CONSTANTS to allow recursive masking of constants
- allow '|' or'ing of constants
- add_TYPE() for all SEQUENCE OF rules
- add external parser callout, allowing flexible code generation
- add OID handling, for generating MIBs
