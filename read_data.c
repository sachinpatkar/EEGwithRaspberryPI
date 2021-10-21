#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "spi.h"
#include "gpio.h"

int main(void) 
{      
    printf("start");
    gpio_t *gpio_in;
    gpio_in = gpio_new();
    gpio_open (gpio_in, "/dev/gpiochip0", 26, GPIO_DIR_IN);

    spi_t *spi;
    spi = spi_new();
    spi_open_advanced(spi, "/dev/spidev0.0", 0x01, 1000000, MSB_FIRST, 8, 1);
    
    void write_reg(uint8_t reg_address, uint8_t val_hex)
           {
           uint8_t zero3[3] = {0x00, 0x00,0x00}; 
           uint8_t reg_address_shift = 0x40 | reg_address;
           uint8_t write [3] = {reg_address_shift, 0x00,val_hex};
           spi_transfer (spi, write, zero3, 3);
           }
    void send_command(uint8_t command)
           {
           uint8_t zero1[1] = {0x00};
           uint8_t write_command [1] = {command};
           spi_transfer (spi, write_command, zero1, 1);
           }        
          write_reg (0x14,0x80);//led
          write_reg (0x05,0x00);//ch1
          write_reg (0x06,0x00);//ch2
          write_reg (0x07,0x00);//ch3
          write_reg (0x08,0x00);//ch4
          write_reg (0x09,0x00);//ch5
          write_reg (0x0A,0x00);//ch6
          write_reg (0x0B,0x00);//ch7
          write_reg (0x0C,0x00);//ch8
          write_reg (0x15,0x20);// mics
          write_reg (0x01,0x96);// reg1
          write_reg (0x02,0xD4);// reg2
          write_reg (0x03,0xE0);// reg3
          send_command (0x10);//sdatc
          send_command (0x08);//start

bool value;
bool was_one = 0;
uint8_t buf[27] = {0}; 
float package [8]={0};   
uint32_t data_test = 0x7FFFFF;
uint32_t data_check = 0xFFFFFF;
uint8_t zero27[27] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 

while (1)  
 {      
        gpio_read (gpio_in, &value);
        if (value == 1)
        {   
            was_one = 1;
        }
        if ((value == 0) && ( was_one == 1))
        {   
            was_one = 0;
            spi_transfer (spi, zero27, buf, 27);
            for (int i = 1; i<9; i++)  
              {              
                int offset = 3*i;
                uint32_t voltage = (buf[offset] << 8) | buf[offset+1];
                voltage = (voltage << 8) | buf[offset+2];
                uint32_t voltage_test = voltage | data_test;

                if (voltage_test == data_check)
                {
                    voltage = 16777214 - voltage;                   
                }

                  package[i-1]=0.27*voltage;   // (4.5*1000000/16777214)=0.27 
                  printf("%.1f\n", package[4]); // 0 - first chanel
            } 
      }
 }
  spi_close(spi);
  spi_free(spi);
  gpio_close(gpio_in);
  gpio_free(gpio_in);
return 0;
}


