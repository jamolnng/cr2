OUTPUT_ARCH("riscv")
 
ENTRY(_start)
 
MEMORY
{
	flash (rxai!w) : ORIGIN = 0x20010000, LENGTH = 0x6a120
	itim (wx!rai) : ORIGIN = 0x8000000, LENGTH = 0x2000
	ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 0x4000
}
 
PHDRS
{
	flash PT_LOAD;
	ram PT_LOAD;
	ram_init PT_LOAD;
	itim PT_LOAD;
	itim_init PT_LOAD;
}
 
SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;
  PROVIDE(__stack_size = __stack_size);
  
  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } >flash AT>flash :flash
 
  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >flash AT>flash :flash
 
  .text           :
  {
		*(.text.unlikely .text.unlikely.*)
		*(.text.startup .text.startup.*)
		*(.text .text.*)
		*(.gnu.linkonce.t.*)
  } >flash AT>flash :flash

  PROVIDE (__etext = .);
	PROVIDE (_etext = .);
	PROVIDE (etext = .);
 
  .rodata         :
  {
		*(.rdata)
		*(.rodata .rodata.*)
		*(.gnu.linkonce.r.*)
		. = ALIGN(8);
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)
  } >flash AT>flash :flash

	. = ALIGN(8);

	.itim : ALIGN(8) {
		*(.itim .itim.*)
	} >itim AT>flash :itim_init
 
 
  .data : ALIGN(8) {
		*(.data .data.*)
		*(.gnu.linkonce.d.*)
		. = ALIGN(8);
		PROVIDE( __global_pointer$ = . + 0x800 );
		*(.sdata .sdata.* .sdata2.*)
		*(.gnu.linkonce.s.*)
		. = ALIGN(8);
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)
		. = ALIGN(8);
		*(.rdata)
		*(.rodata .rodata.*)
		*(.gnu.linkonce.r.*)
	} >ram AT>flash :ram_init
 
	.bss : ALIGN(8) {
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
	} >ram AT>ram :ram
	PROVIDE( metal_segment_bss_target_start = ADDR(.bss) );
	PROVIDE( metal_segment_bss_target_end = ADDR(.bss) + SIZEOF(.bss) );

	.stack : {
		. += __stack_size;
		PROVIDE( _sp = . );
	} >ram AT>ram :ram
}