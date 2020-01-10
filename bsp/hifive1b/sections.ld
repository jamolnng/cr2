.itim : ALIGN(8) {
  *(.itim .itim.*)
} >itim AT>flash :itim_init
PROVIDE(__itim_source_start = LOADADDR(.itim));
PROVIDE(__itim_target_start = ADDR(.itim) );
PROVIDE(__itim_target_end = ADDR(.itim) + SIZEOF(.itim));
PROVIDE(__itim_memory_end = ORIGIN(itim) + LENGTH(itim));