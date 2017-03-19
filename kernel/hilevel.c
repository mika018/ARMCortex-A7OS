#include "hilevel.h"


void hilevel_handler_rst( ctx_t* ctx              ) {
  
	scheduler_initialise( ctx );
	ipc_initialise();

	TIMER0->Timer1Load  = 0x00100000; // select period = 2^20 ticks ~= 1 sec
	TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
	TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
	TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
	TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

	GICC0->PMR          = 0x000000F0; // unmask all            interrupts
	GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
	GICC0->CTLR         = 0x00000001; // enable GIC interface
	GICD0->CTLR         = 0x00000001; // enable GIC distributor

	int_enable_irq();

	return;
}

void hilevel_handler_svc( ctx_t* ctx, uint32_t id ) {
  /* Based on the identified encoded as an immediate operand in the
   * instruction,
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call,
   * - write any return value back to preserved usr mode registers.
   */

	switch( id ) {
		case 0x00 : { // 0x00 => yield()
			scheduler_run( ctx );
			break;
		}
		case 0x01 : { // 0x01 => get_pid()
			ctx->gpr[ 0 ] = scheduler_get_pid();
			break;
		}
		case 0x02 : { // 0x02 => fork()
			pid_t result = scheduler_fork( ctx );
			ctx->gpr[ 0 ] = result;
			break;
		}
		case 0x03 : { // 0x03 => exec( x )
			scheduler_exec( ctx );
			break;
		}
		case 0x04 : { // 0x04 => exit( x )
			scheduler_exit( ctx );
			break;
		}
		case 0x05 : { // 0x05 => print( x, n )
			char*  x = ( char* )( ctx->gpr[ 0 ] );
			int    n = ( int   )( ctx->gpr[ 1 ] );

			for( int i = 0; i < n; i++ ) {
			PL011_putc( UART0, *x++, true );
			}
			break;
		}
		case 0x06 : { // 0x06 => kill
			pid_t pid = ( pid_t )( ctx->gpr[ 0 ] );
			int   sig = ( int   )( ctx->gpr[ 1 ] );

			int result = scheduler_kill( pid, sig );

			ctx->gpr[ 0 ] = result;
			break;
		}
		case 0x07 : { // 0x07 => msend( pipe_id, pid_src, pid_des, x)
			int   pipe_id = ( int   )( ctx->gpr[ 0 ] );
			pid_t pid_src = ( pid_t )( ctx->gpr[ 1 ] );
			pid_t pid_des = ( pid_t )( ctx->gpr[ 2 ] );
			int         x = ( int   )( ctx->gpr[ 3 ] );
			
			ipc_send_message( pipe_id, pid_src, pid_des, x );

			break;
		}
		case 0x08 : { // 0x08 => mreceive( pipe_id, pid_des )
			int   pipe_id = ( int   )( ctx->gpr[ 0 ] );
			pid_t pid_des = ( pid_t )( ctx->gpr[ 1 ] );
			
			int r = ipc_receive_message( pipe_id, pid_des );

			ctx->gpr[ 0 ] = r;
			break;
		}
		case 0x09 : { // 0x08 => pipe( pid_1, pid_2 )
			pid_t pid_1 = ( pid_t )( ctx->gpr[ 0 ] );
			pid_t pid_2 = ( pid_t )( ctx->gpr[ 1 ] );

			int r = ipc_pipe( pid_1, pid_2 );

			ctx->gpr[ 0 ] = r;
			break;
		}
		case 0x12 : { // 0x12 => write( fd, x, n )
			int   fd = ( int   )( ctx->gpr[ 0 ] );
			char*  x = ( char* )( ctx->gpr[ 1 ] );
			int    n = ( int   )( ctx->gpr[ 2 ] );

			for( int i = 0; i < n; i++ ) {
			PL011_putc( UART0, *x++, true );
			}

			ctx->gpr[ 0 ] = n;
			break;
		}
		default   : { // 0x?? => unknown/unsupported
			break;
		}
	}

	return;
}

void hilevel_handler_irq( ctx_t* ctx ) {
  // Step 2: read  the interrupt identifier so we know the source.

  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.

  if( id == GIC_SOURCE_TIMER0 ) {
    // PL011_putc( UART0, 'T', true );
    scheduler_run( ctx );
    TIMER0->Timer1IntClr = 0x01;
  }

  // Step 5: write the interrupt identifier to signal we're done.

  GICC0->EOIR = id;

  return;
}
