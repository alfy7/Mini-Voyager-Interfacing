/*
 * This code controls the 6 charlieplexed LEDs on the Mini-Voyager 1 using 3 GPIO pins on the MSP430G2553
 *
 * It uses Timer0 interrupts to keep switching between the LEDs
 * The 3 pins can be connected to any of the Ports on the MSP430 and the #define statements must be modified appropriately
 * The least significant 6 bits of the variable charlie_val control the 6 LEDs individually
 *
 * Left Pin-> CHARLIE_2
 * Center Pin-> CHARLIE_1
 * Right Pin-> CHARLIE_0
 *
 * charlie_iter keeps going from 0 to 5 and back to 0 everytime Timer0 raises an interrupt
 * The charlie function checks if the charlie_iter numbered LED has to be on by checking charlie_val
 * If it has to be on, it will send appropriate ON, OFF and HIGH IMPEDANCE states to the 3 pins connected to the LEDs
 */

#include <msp430.h> 

//Direction Control Register for the 3 Pins connected to the Charlieplexed LEDs
#define CHARLIE_0_D P2DIR
#define CHARLIE_1_D P2DIR
#define CHARLIE_2_D P2DIR
//Output Register for the 3 Pins connected to the Charlieplexed LEDs
#define CHARLIE_0_O P2OUT
#define CHARLIE_1_O P2OUT
#define CHARLIE_2_O P2OUT
//Pin Number for the 3 Pins connected to the Charlieplexed LEDs
#define CHARLIE_0_B BIT0
#define CHARLIE_1_B BIT1
#define CHARLIE_2_B BIT2

int charlie_val=0x01; //Variable to hold the LED states
int charlie_iter=0; //Required for switching between the LEDs in succession
void charlie(int val, int iter)
{
    if((val&(1<<iter))!=0)
    {
        switch(iter) //Turn on a particular LED according to charlie_val
        {
        case 3:
            CHARLIE_0_D|=CHARLIE_0_B;
            CHARLIE_1_D|=CHARLIE_1_B;
            CHARLIE_2_D&=~CHARLIE_2_B;
            CHARLIE_0_O|=CHARLIE_0_B;
            CHARLIE_1_O&=~CHARLIE_1_B;
            break;
        case 4:
            CHARLIE_0_D|=CHARLIE_0_B;
            CHARLIE_1_D|=CHARLIE_1_B;
            CHARLIE_2_D&=~CHARLIE_2_B;
            CHARLIE_1_O|=CHARLIE_1_B;
            CHARLIE_0_O&=~CHARLIE_0_B;
            break;
        case 0:
            CHARLIE_1_D|=CHARLIE_1_B;
            CHARLIE_2_D|=CHARLIE_2_B;
            CHARLIE_0_D&=~CHARLIE_0_B;
            CHARLIE_1_O|=CHARLIE_1_B;
            CHARLIE_2_O&=~CHARLIE_2_B;
            break;
        case 1:
            CHARLIE_1_D|=CHARLIE_1_B;
            CHARLIE_2_D|=CHARLIE_2_B;
            CHARLIE_0_D&=~CHARLIE_0_B;
            CHARLIE_2_O|=CHARLIE_2_B;
            CHARLIE_1_O&=~CHARLIE_1_B;
            break;
        case 2:
            CHARLIE_2_D|=CHARLIE_2_B;
            CHARLIE_0_D|=CHARLIE_0_B;
            CHARLIE_1_D&=~CHARLIE_1_B;
            CHARLIE_2_O|=CHARLIE_2_B;
            CHARLIE_0_O&=~CHARLIE_0_B;
            break;
        case 5:
            CHARLIE_2_D|=CHARLIE_2_B;
            CHARLIE_0_D|=CHARLIE_0_B;
            CHARLIE_1_D&=~CHARLIE_1_B;
            CHARLIE_0_O|=CHARLIE_0_B;
            CHARLIE_2_O&=~CHARLIE_2_B;
            break;
        }
    }
    if((val&0x3F)==0) //Turn off all LEDs
    {
        CHARLIE_0_D&=~CHARLIE_0_B;
        CHARLIE_1_D&=~CHARLIE_1_B;
        CHARLIE_2_D&=~CHARLIE_2_B;
    }

}

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    TACTL|=TASSEL_2+MC_1+TAIE; //Start Timer0 with SMCLK clock source, UP mode and enable overflow interrupt
    TACCR0=3000-1; //Deciding the refresh rate for the Charlieplexed LEDs

    __enable_interrupt(); //Enable maskable interrupts
    int p; //Required for the while loop
    while(1)//Run code forever
    {   //A simple animation to illustrate the Charlieplexed LEDs
        charlie_val=0x01;
        for(p=0;p<7;p++)
        {
            __delay_cycles(150000);
            charlie_val=(charlie_val<<1)+((charlie_val&(1<<5))>>5);
        }
        charlie_val=0x03;
        for(p=0;p<5;p++)
        {
            __delay_cycles(150000);
            charlie_val=(charlie_val<<1)+1;
        }
        charlie_val=(charlie_val<<1);
        for(p=0;p<6;p++)
        {
            __delay_cycles(150000);
            charlie_val=(charlie_val<<1);
        }
    }
    return 0;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TMR0()
{
    if(TAIV==TA0IV_TAIFG)   //Check if Timer overflow caused the interrupt
        //This would be required in projects where multiple interrupts have
        //the same interrupt vector. Here it is only optional.
    {
        charlie(charlie_val,charlie_iter); //Call the charlie function
        charlie_iter++; //Increment iter
        if(charlie_iter==6) //If all 6 LEDs have been turned on/off (according to charlie_val)
            charlie_iter=0; //Reset iter, to start again
        TACTL&=~(TAIFG); //Reset the interrupt flag
    }
}

