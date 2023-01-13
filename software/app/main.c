#include <system.h>
#include "altera_avalon_pio_regs.h"
#include <stdio.h>
#include <sys/alt_irq.h>

int start;
int time = 25000;

static void button_irq()
{			
			for (int i =0; i<=7; i++)
			{
				IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x01<<i);
				usleep(time);
			}
			for (int i =1; i<=7; i++)
			{
				IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x80>>i);
				usleep(time);
			}
			IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x00);
			IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_2_BASE, 0x1);
}


static void switch_irq()
{
	time = (int) IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
	time = time*10000 + 25000;
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_1_BASE, 0xf);
}

int main()
{
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_1_BASE, 0xf);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_1_BASE, 0xf);
	alt_ic_isr_register(PIO_1_IRQ_INTERRUPT_CONTROLLER_ID,PIO_1_IRQ,(void*) switch_irq,NULL, 0x0);

	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_2_BASE, 0x1);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_2_BASE, 0x1);
	alt_ic_isr_register(PIO_1_IRQ_INTERRUPT_CONTROLLER_ID,PIO_2_IRQ,(void*) button_irq,NULL, 0x0);

	while(1);
	return 0;
}