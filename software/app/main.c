#include <system.h>
#include "altera_avalon_pio_regs.h"
int main()
{
	IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_0_BASE,0x01);

	int trigger;
	int speed;
	while(1)
	{
		trigger = IORD_ALTERA_AVALON_PIO_DATA(PIO_2_BASE);
		if (trigger !=1)
		{		
			speed = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);				
			for (int i =0; i<=7; i++)
			{
				IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x01<<i);
				usleep(speed*50000+100000);
				
			}
			IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x00);
		}
	}
	return 0;
}