BITS	32
CPU		386

SECTION	.text

Header:

DB	'MTA32', 0
TIMES 256 - ($ - Header) DB 0
