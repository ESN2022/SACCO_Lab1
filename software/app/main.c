#include <system.h>
#include "altera_avalon_pio_regs.h"
int main()
{
	IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_0_BASE,0x01);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0xff);
	
	return 0;
}