/*
 * This  code controls the LCD display on the Mini-Voyager 2.
 *
 * The code can be used to understand the intricate details of interfacing to the LCD display,
 * and hence, is not made into a full library, which hides the implementation details.
 * We use 4 bit control and hence we only need to connect the pins D7-D4 for Data/Instruction.
 * We also need to connect the Register Select and Enable pins.
 *
 * The LCD needs 5V supply to run. Just near the USB port on the Launchpad, a header can be soldered on the hole
 * labelled  tp1 to get 5V (or just try putting a jumper wire through it and stabilize it)
 * Even though the logic levels on the LCD display are 5V, the MSP430's 3.3V and ground will be correctly read as
 * logic HIGH and LOW. However, we cannot read data from the LCD display as the MSP430 cannot handle 5V and so we permanently
 * ground the R/W pin, essentially we can only write to the LCD now (This is already done in the Mini-Voyager)
 *
 * The 6 pins of the LCD Display can be connected to any of the Ports on the MSP430 and the #define statements must be modified appropriately
 *
D7 -> LCD_D7
D6 -> LCD_D6
D5 -> LCD_D5
D4 -> LCD_D4
Enable -> LCD_E
Register Select -> LCD_RS

 * I have connected the pins as follows
D7 -> P2.3
D6 -> P2.2
D5 -> P2.1
D4 -> P2.0
Enable -> P2.4
Register Select -> P2.5

 * If you are using Port pins which are not configured as GPIO pins by default, you must configure them as GPIO
 * before calling LCD_Initialize() function which sets all the pins connected to the LCD display as output pins
 *
 */
#include <msp430.h> 

//Direction Control Register for the 6 Pins connected to the LCD
#define LCD_D7_D P2DIR
#define LCD_D6_D P2DIR
#define LCD_D5_D P2DIR
#define LCD_D4_D P2DIR
#define LCD_E_D P2DIR
#define LCD_RS_D P2DIR

//Output Register for the 6 Pins connected to the LCD
#define LCD_D7_O P2OUT
#define LCD_D6_O P2OUT
#define LCD_D5_O P2OUT
#define LCD_D4_O P2OUT
#define LCD_E_O P2OUT
#define LCD_RS_O P2OUT

//Pin number for the 6 Pins connected to the LCD
#define LCD_D7_B BIT3
#define LCD_D6_B BIT2
#define LCD_D5_B BIT1
#define LCD_D4_B BIT0
#define LCD_E_B BIT4
#define LCD_RS_B BIT5

void LCD_Enable(void)   //Every time a nibble/byte of data/instruction is written, the Enable Pin must be pulsed for
//the LCD to read the data/instruction
{
    __delay_cycles(1000); //Delay
    LCD_E_O|=LCD_E_B; //Set the Enable pin HIGH
    __delay_cycles(1000); //Delay
    LCD_E_O&=~LCD_E_B; //Set the Enable pin LOW
    __delay_cycles(1000); //Delay
}

void LCD_Write(int di,int val) //This function writes data or instruction to the LCD in 4 bit format
{
    int high_val=val>>4,low_val=val&0x0F; //Get the higher and lower nibble of the byte to be sent

    if(di==0) //If instruction
        LCD_RS_O&=~LCD_RS_B; //Set RS pin LOW
    else //If data
        LCD_RS_O|=LCD_RS_B; //Set RS pin HIGH

    //Set the D7 D6 D5 D4 pins properly to the higher nibble
    if(high_val&0x01)
        LCD_D4_O|=LCD_D4_B;
    else
        LCD_D4_O&=~LCD_D4_B;
    high_val=high_val>>1;
    if(high_val&0x01)
        LCD_D5_O|=LCD_D5_B;
    else
        LCD_D5_O&=~LCD_D5_B;
    high_val=high_val>>1;
    if(high_val&0x01)
        LCD_D6_O|=LCD_D6_B;
    else
        LCD_D6_O&=~LCD_D6_B;
    high_val=high_val>>1;
    if(high_val&0x01)
        LCD_D7_O|=LCD_D7_B;
    else
        LCD_D7_O&=~LCD_D7_B;

    LCD_Enable(); //Pulse the Enable pin
    //Set the D7 D6 D5 D4 pins properly to the lower nibble
    if(low_val&0x01)
        LCD_D4_O|=LCD_D4_B;
    else
        LCD_D4_O&=~LCD_D4_B;
    low_val=low_val>>1;
    if(low_val&0x01)
        LCD_D5_O|=LCD_D5_B;
    else
        LCD_D5_O&=~LCD_D5_B;
    low_val=low_val>>1;
    if(low_val&0x01)
        LCD_D6_O|=LCD_D6_B;
    else
        LCD_D6_O&=~LCD_D6_B;
    low_val=low_val>>1;
    if(low_val&0x01)
        LCD_D7_O|=LCD_D7_B;
    else
        LCD_D7_O&=~LCD_D7_B;

    LCD_Enable(); //Pulse the Enable pin

}

void LCD_Initialize(void)
{
    __delay_cycles(1000000); //Wait for the LCD to power up completely
    //Set all pins connected to the LCD as output pins
    LCD_D7_D|=LCD_D7_B;
    LCD_D6_D|=LCD_D6_B;
    LCD_D5_D|=LCD_D5_B;
    LCD_D4_D|=LCD_D4_B;
    LCD_E_D|=LCD_E_B;
    LCD_RS_D|=LCD_RS_B;
    //Setup the LCD
    LCD_Write(0,0x2C); //Function Set : Data length (4 bit),Display Lines (2 lines), Character Font (5x10)
    LCD_Write(0,0x06); //Cursor moves right automatically, no Display shift
    LCD_Write(0,0x0C); //Display On, Cursor Off, Blinking Off
    LCD_Write(0,0x02); //Sets Cursor to 0, Resets Display shift
    LCD_Write(0,0x01); //Sets Cursor to 0, Clear Display Data
    __delay_cycles(1000);
}

void LCD_Write_String(char* ch, int rpos,int cpos, int len)
//This function writes a string *ch to the LCD. rpos and cpos desribe the location where
//the string has to be written on the LCD display. The first "len" characters pointed by ch are only printed
{
    int ddram_addr; //Address of Data RAM where data has to be stored
    if(rpos==0)
        ddram_addr=0x80; //Address for 1st line
    else
        ddram_addr=0xC0;//Address for 2nd line
    ddram_addr+=cpos; //Adjust address to proper column
    LCD_Write(0,0x02); //Sets Cursor to 0, Resets Display shift
    LCD_Write(0,ddram_addr); //Sets Cursor to the required location

    //Print the string
    while(((*ch)!='\0')&&(len>0))
    {
        LCD_Write(1,*ch);ch++;len--;
    }
}

void LCD_Clear(void)
{
    LCD_Write(0,0x01); //Command to clear the display
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    LCD_Initialize(); //Initialize the LCD
    //Simple example to illustrate the LCD functions
    char strA[17]="Hello, World!",strB[17]="This is LCD";
    strA[16]='\0';strB[16]='\0';
    LCD_Write_String(strA,0,2,16);
    LCD_Write_String(strB,1,3,16);
    return 0;
}
