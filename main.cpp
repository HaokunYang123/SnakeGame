#include "spiAVR.h"
#include "timerISR.h"
#include "periph.h"
#include "helper.h"
// #include "serialATmega.h"
#define NUM_TASKS 1 // TODO: Change to the number of tasks being used

// Task struct for concurrent synchSMs implmentations
typedef struct _task
{
  signed char state;         // Task's current state
  unsigned long period;      // Task period
  unsigned long elapsedTime; // Time elapsed since last task tick
  int (*TickFct)(int);       // Task tick function
} task;

// wait since I am using a joystick, (I have all the stuff from the joystick connected to port c ) I should init it as 0 for ddr?

// TODO: Define Periods for each task

const unsigned long TASK1_PERIOD = 100;
// const unsigned long TASK2_PERIOD = 1;
// const unsigned long TASK3_PERIOD = 1;
// const unsigned long TASK4_PERIOD = 100;
//  e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned long GCD_PERIOD = 1; // TODO:Set the GCD Period

task tasks[NUM_TASKS];

void TimerISR()
{
  for (unsigned int i = 0; i < NUM_TASKS; i++)
  { // Iterate through each task in the task array
    if (tasks[i].elapsedTime == tasks[i].period)
    {                                                    // Check if the task is ready to tick
      tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
      tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
    }
    tasks[i].elapsedTime += GCD_PERIOD; // Increment the elapsed time by GCD_PERIOD
  }
}

void generateSNAKE()
{
  Send_Command(0x3A); // interface pixel format
  Send_Data(0x05);    // 16 bit color because it's the eaisest to do

  Send_Command(0x2A); // column address set
  // Send_Data(129); //XS15
  Send_Data(0);   // XS15
  Send_Data(2);   // XS0
  Send_Data(0);   // XE15
  Send_Data(129); // XE0

  Send_Command(0x2B); // row address set
  Send_Data(0);       // YS15
  Send_Data(1);       // YS0
  Send_Data(0);       // YE15
  Send_Data(128);     // YE0
  Send_Command(0x2C); // memory write
  for(int pixel = 0; pixel < 16384; pixel++){          // the loop for 128x128 = 16384
    Send_Data(0xFF); // needs two param for the 16 bit
    Send_Data(0xFF);
    pixel++;
  }

}
void removeSNAKE()
{
  Send_Command(0x3A); // interface pixel format
  Send_Data(0x05);    // 18 bit color because it's the eaisest to do

  Send_Command(0x2A); // column address set
  // Send_Data(129); //XS15
  Send_Data(0);   // XS15
  Send_Data(2);   // XS0
  Send_Data(0);   // XE15
  Send_Data(129); // XE0

  Send_Command(0x2B); // row address set
  Send_Data(0);       // YS15
  Send_Data(1);       // YS0
  Send_Data(0);       // YE15
  Send_Data(128);     // YE0
  Send_Command(0x2C); // memory write
  for(int pixel = 0; pixel < 16384; pixel++){          // the loop for 128x128 = 16384
    Send_Data(0xFF); // needs two param for the 16 bit
    Send_Data(0xFF);
    pixel++;
  }

}
// state machine here
enum ReadJoySticks
{
  init1,
  left,
  right,
  down,
  up
} DirectionState;
// A0 is X and A1 is Y
int Joystick(int state)
{
  switch (DirectionState)
  {
  case init1:
    if (ADC_read(PORTC0) < 400)
    {
      DirectionState = down;
      
    }
    else if (ADC_read(PORTC0) > 600)
    {
      DirectionState = up;
    }
    else if (ADC_read(PORTC1) < 400)
    {
      DirectionState = left;
    }
    else if (ADC_read(PORTC1) > 600)
    {
      DirectionState = right;
    }
    else
    {
      DirectionState = init1;
      removeSNAKE();
    }
    break;
  case left:
    DirectionState = init1;
    break;
  case right:
    DirectionState = init1;
    break;
  case down:
    DirectionState = init1;
    generateSNAKE();
    break;
  case up:
    DirectionState = init1;
    break;
  }
  switch (DirectionState)
  {
  case init1:
    break;
  case left:
    break;
  case right:
    break;
  case down:
    break;
  case up:
    break;
  }
  return DirectionState;
}
int main(void)
{
  // TODO: initialize all your inputs and ouputs
  //   DDR 1 is output and 0 is input
  // DDRC = 0b000000;
  // PORTC = 0b111110;

  // Marios suggestion
  // DDRB = 0x2F;
  // PORTB = 0x10;
  DDRC = 0x00;
  PORTC = 0xFF;

  // DDRB = 0xFF;
  // PORTB = 0x00;
  DDRB = 0x2F; // setting the MISO pin to input even tho we are not using it
  PORTB = 0x10;

  DDRD = 0xFF;
  PORTD = 0x00;

   ADC_init(); // initializes ADC

  //    serial_init(9600);

  // TODO: Initialize the buzzer timer/pwm(timer0)
  OCR0A = 255; // sets duty cycle to 50% since TOP is always 256

  TCCR0A |= (1 << COM0A1);               // use Channel A
  TCCR0A |= (1 << WGM01) | (1 << WGM00); // set fast PWM Mode
  TCCR0B = (TCCR0B & 0xF8) | 0x02;       // set prescaler to 8
  TCCR0B = (TCCR0B & 0xF8) | 0x03;       // set prescaler to 64
  TCCR0B = (TCCR0B & 0xF8) | 0x04;       // set prescaler to 256
  // TCCR0B = (TCCR0B & 0xF8) | 0x05;       // set prescaler to 1024// set prescaler to 1024
  // TODO: Initialize the servo timer/pwm(timer1)
  TCCR1A |= (1 << WGM11) | (1 << COM1A1);              // COM1A1 sets it to channel A
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // CS11 sets the prescaler to be 8
  // WGM11, WGM12, WGM13 set timer to fast pwm mode

  // ICR1 = 39999; // 20ms pwm period

  // OCR1A = servoAngle; // set the range of the value 1999 to 3999/* set your value here *///
  unsigned int i = 0;
  // TODO: Initialize tasks here
  //  e.g.
  //  tasks[0].period = ;
  //  tasks[0].state = ;
  //  tasks[0].elapsedTime = ;
  //  tasks[0].TickFct = ;

  tasks[i].state = init1;
  tasks[i].period = 100;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &Joystick;
  ++i;
  //  tasks[i].state = init2;
  //  tasks[i].period = 5;
  //  tasks[i].elapsedTime = tasks[i].period;
  //  tasks[i].TickFct = &L7Task2_EX1;
  //  ++i;
  //  tasks[i].state = init3;
  //  tasks[i].period = 5;
  //  tasks[i].elapsedTime = tasks[i].period;
  //  tasks[i].TickFct = &L7Task3_EX1;
  // ++i;

  TimerSet(GCD_PERIOD);
  TimerOn();

  SPI_INIT();
  ST7735_init();
  Send_Command(0x3A); // interface pixel format
  Send_Data(0x05);    // 16 bit color because it's the eaisest to do

  Send_Command(0x2A); // column address set
  // Send_Data(129); //XS15
  Send_Data(0);   // XS15
  Send_Data(2);   // XS0
  Send_Data(0);   // XE15
  Send_Data(129); // XE0

  Send_Command(0x2B); // row address set
  Send_Data(0);       // YS15
  Send_Data(1);       // YS0
  Send_Data(0);       // YE15
  Send_Data(128);     // YE0
  Send_Command(0x2C); // memory write
  for(int pixel = 0; pixel < 16384; pixel++){          // the loop for 128x128 = 16384
    Send_Data(0xFF); // needs two param for the 16 bit
    Send_Data(0xFF);
    pixel++;
  }

  // so if I want to uncover the head of the snake of 4x4 grid I can just do
  // Send command 0x2A
  //  and then I can do like
  while (1)
  {
    // serial_println(ADC_read(PORTC0), 10);

    //    serial_println(ADC_read(PORTC1), 10);
  }

  return 0;
}
