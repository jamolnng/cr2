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
    *(.text .text.*)
  } >flash AT>flash :flash
 
  .rodata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)
  } >flash AT>flash :flash

  .itim : ALIGN(8) {
		*(.itim .itim.*)
	} >itim AT>flash :itim_init
 
  . = ALIGN(4);
 
  .data          :
  {
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
  } >ram AT>flash :ram_init
 
  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );
 
  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  .bss : ALIGN(8) {
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
	} >ram AT>ram :ram
 
  . = ALIGN(8);
  PROVIDE( _end = . );
  PROVIDE( end = . );
 
  .stack ORIGIN(ram) + LENGTH(ram) - __stack_size :
  {
    PROVIDE( _heap_end = . );
    . = __stack_size;
    PROVIDE( _sp = . );
  } >ram AT>ram :ram
}