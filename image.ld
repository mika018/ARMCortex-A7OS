SECTIONS {
  /* assign load address (per  QEMU)                        */
  .       =     0x70010000;
  /* place text segment(s)                                  */
  .text : { kernel/lolevel.o(.text) *(.text .rodata) }
  /* place data segment(s)                                  */
  .data : {                         *(.data        ) }
  /* place bss  segment(s)                                  */
  .bss  : {                         *(.bss         ) }
  /* align       address (per AAPCS)                        */
  .       = ALIGN( 8 );
  /* allocate stack for irq mode                            */
  .       = . + 0x00001000;
  tos_irq = .;
  /* allocate stack for svc mode                            */
  .       = . + 0x00001000;
  tos_svc = .;
  /* allocate stack space for 15 user processes (5000 each) */
   .       = . + 0x00075000;
  tos_user = .;
}
