#ifndef SPIAVR_H
#define SPIAVR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "helper.h"

// B5 should always be SCK(spi clock) and B3 should always be MOSI. If you are using an
// SPI peripheral that sends data back to the arduino, you will need to use B4 as the MISO pin.
// The SS pin can be any digital pin on the arduino. Right before sending an 8 bit value with
// the SPI_SEND() funtion, you will need to set your SS pin to low. If you have multiple SPI
// devices, they will share the SCK, MOSI and MISO pins but should have different SS pins.
// To send a value to a specific device, set it's SS pin to low and all other SS pins to high.

// Outputs, pin definitions
#define PIN_SCK PORTB5  // SHOULD ALWAYS BE B5 ON THE ARDUINO
#define PIN_MOSI PORTB3 // SHOULD ALWAYS BE B3 ON THE ARDUINO
#define PIN_SS PORTB2

// If SS is on a different port, make sure to change the init to take that into account.
void SPI_INIT()
{
    DDRB |= (1 << PIN_SCK) | (1 << PIN_MOSI) | (1 << PIN_SS); // initialize your pins.
    SPCR |= (1 << SPE) | (1 << MSTR);                         // initialize SPI coomunication
}

void SPI_SEND(char data)
{
    SPDR = data; // set data that you want to transmit
    while (!(SPSR & (1 << SPIF)))
        ; // wait until done transmitting
}

void HardwareReset()
{
    PORTC = SetBit(PORTC, 5, 0);
    _delay_ms(200);
    PORTC = SetBit(PORTC, 5, 1);
    _delay_ms(200);
}

void Send_Command(char i)
{
    PORTB = SetBit(PORTB, 0, 0);
    SPI_SEND(i);
}

void Send_Data(char j)
{
    PORTB = SetBit(PORTB, 0, 1);
    SPI_SEND(j);
}

void ST7735_init()
{
    PORTB = SetBit(PORTB, 2, 0);
    HardwareReset();
    Send_Command(0x01); //software reset
    _delay_ms(150);
    Send_Command(0x11); //sleep out & booster on
    _delay_ms(200);
    Send_Command(0x3A); // interface pixel format
    Send_Data(0x06);
    _delay_ms(10);
    Send_Command(0x29); //display on
    _delay_ms(200);
}

#endif /* SPIAVR_H */