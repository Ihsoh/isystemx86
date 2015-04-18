SECTIONS
{
. = 0x01300000;
.text : { *(.text) }
.data : { *(.data) }
.bss : { *(.bss) }
}
