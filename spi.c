#include "spi.h"

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	//DDRB = (1<<MOSI)|(1<<SCK);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}
void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}



