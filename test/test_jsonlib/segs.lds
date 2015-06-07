SECTIONS
{
. = 0x01300000;
.text : { *(.text) }
.bss : { *(.bss) }
.data : { *(.data) }
}
