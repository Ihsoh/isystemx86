SECTIONS
{
	. = 0x20000;
	.text :
	{
		*(.text)
		. = ALIGN(4);
	}
	/*
		.ctors is come from https://github.com/foreverbell/BadAppleOS/blob/master/kernel/link.ld
	*/
	.ctors :
	{
		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		*(.ctors)
		LONG(0)
		__CTOR_END__ = .;

		__INIT_ARRAY_LIST__ = .;
		LONG((__INIT_ARRAY_END__ - __INIT_ARRAY_LIST__) / 4 - 2)
		*(.init_array)
		LONG(0)
		__INIT_ARRAY_END__ = .;

		/* dtor omitted. */

		. = ALIGN(4);
	}
	.bss :
	{
		*(.bss)
		. = ALIGN(4);
	}
	.data :
	{
		*(.data)
		. = ALIGN(4);
	}
}
