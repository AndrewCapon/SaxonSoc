#include <stdint.h>

#include "saxon.h"
#include "soc.h"
#include "flash.h"
#include "io.h"

volatile char globalC = 'b';

void print_hex(uint32_t val, uint32_t digits)
{
	for (int i = (4*digits)-4; i >= 0; i -= 4)
		uart_write(UART_A, "0123456789ABCDEF"[(val >> i) % 16]);
}

void flash_init () {
  write_u32(500, SPI_FLASH_SETUP);
  write_u32(500, SPI_FLASH_HOLD);
  write_u32(500, SPI_FLASH_DISABLE);
}

void flash_divider(uint32_t divider) {
  write_u32(divider, SPI_FLASH_DIVIDER);
}

void flash_mode(uint32_t mode) {
  write_u32(mode, SPI_FLASH_DATAMODE);
}

void flash_begin() {
  write_u32(0x11000000, SPI_FLASH_XFER);
}

void flash_end() {
 write_u32(0x10000000, SPI_FLASH_XFER);
}

uint8_t flash_xfer(uint8_t data) {
  write_u32(data | 0x01000000, SPI_FLASH_XFER);

  for(int i=0;i<1000;i++) {
    uint32_t r = read_u32(SPI_FLASH_XFER);
    if ((r & SPI_FLASH_RX_VALID)) return r & 0xff;
  }
  return 0xFF;
}

void flash_read(uint32_t addr, uint8_t *data, int n) {
        flash_begin();
        flash_xfer(0x03);
        flash_xfer(addr >> 16);
        flash_xfer(addr >> 8);
        flash_xfer(addr);
        while (n--)
                *(data++) = flash_xfer(0);
        flash_end();
}

void flash_wake() {
  flash_begin();
  flash_xfer(0xab);
  flash_end();
}

uint8_t data[4];

void main() {
    uart_writeStr(UART_A, "\nFlash memory at 0x50000\n\n");

    flash_init();
    flash_divider(1);
    flash_mode(0);
    flash_wake();

    for (int i=0; i<100;i++) {
      flash_read(0x50000 + i*4 , data, 4);
      uart_writeStr(UART_A, "Data : ");
      print_hex((uint32_t) *((uint32_t *) data), 8);
      uart_write(UART_A, '\n');
    }
    
    GPIO_A->OUTPUT_ENABLE = 0x000000FF;
    GPIO_A->OUTPUT = 0x00000000;

    globalC+=1;
    UART_A->DATA = globalC;

    uint32_t counter = 0;
    while(1){
        if(counter++ == 10000){
            GPIO_A->OUTPUT = GPIO_A->OUTPUT + 1;
            counter = 0;
        }
        while(UART_A->STATUS >> 24){ //UART RX interrupt
            UART_A->DATA = (UART_A->DATA) & 0xFF;
        }
    }
}

