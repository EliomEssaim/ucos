SECTIONS
{
	. = 0x1000000;
	Image_ROM_Base = .;
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	Image_RO_Limit = .;
	. = 0x02060000;
	Image_RO_Base =.;
	. = 0x02070000;
	Image_RW_Base = .;
	.data : { *(.data) }
	Image_ZI_Base = .;
	.bss : { *(.bss) }
	Image_ZI_Limit = .;
	__bss_start__ = .;
	__bss_end__ = .;
	__EH_FRAME_BEGIN__ = .;
	__EH_FRAME_END__ = .;
PROVIDE (__stack = .);
	end = .;
	_end = .;
	.debug_info     0 : { *(.debug_info)  }
 	.debug_line		0 : { *(.debug_line)  } 
   	.debug_abbrev   0 : { *(.debug_abbrev)}
   	.debug_frame    0 : { *(.debug_frame) }
}