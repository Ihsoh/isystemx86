SECTIONS
{
. = 0x20000;
.text : { *(.text) }
.bss : { *(.bss) }
.data : { *(.data) }
}
