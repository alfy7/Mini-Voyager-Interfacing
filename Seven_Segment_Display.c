/*
 * This code controls the 4 digit seven segment display on the Mini-Voyager 1 using 12 GPIO pins on the MSP430G2553
 *
 * It uses Timer0 interrupts to keep switching between the digits
 * The 12 pins can be connected to any of the Ports on the MSP430 and the #define statements must be modified appropriately
 * The array ssd_val[4] controls the 4 digits and ssd_dot[4] controls the decimal point individually
 *
 * A-> SSD_A
 * B-> SSD_B
 * C-> SSD_C
 * D-> SSD_D
 * E-> SSD_E
 * F-> SSD_F
 * G-> SSD_G
 * DP-> SSD_DP
 *
 * D0->SSD_0
 * D1->SSD_1
 * D2->SSD_2
 * D3->SSD_3
 *
 * For this code, connect as follows
 * A-> P2.0
 * B-> P2.1
 * C-> P2.2
 * D-> P2.3
 * E-> P2.4
 * F-> P2.5
 * G-> P2.6
 * DP-> P2.7
 *
 * D0-> P1.0
 * D1-> P1.1
 * D2-> P1.2
 * D3-> P1.3
 *
 * ssd_iter keeps going from 0 to 3 and back to 0 everytime Timer0 raises an interrupt
 * The Seg7() function returns the 7 segment display code for the number passed to it in the format [abcedfg0]
 * The update7Seg() takes in a digit and updates that digit on the display
 * The Timer0 interrupt calls update7Seg periodically and updates each digit one by one
 *
 * If you are using Port pins which are not configured as GPIO pins by default, you must configure them as GPIO
 * before calling ssd_init() function which sets all the pins connected to the seven segment display as output pins
 */

#include <msp430.h> 

//Direction Control Register for the 12 Pins connected to the Seven Segment Display
#define SSD_A_D P2DIR
#define SSD_B_D P2DIR
#define SSD_C_D P2DIR
#define SSD_D_D P2DIR
#define SSD_E_D P2DIR
#define SSD_F_D P2DIR
#define SSD_G_D P2DIR
#define SSD_DP_D P2DIR
#define SSD_0_D P1DIR
#define SSD_1_D P1DIR
#define SSD_2_D P1DIR
#define SSD_3_D P1DIR
//Output Register for the 12 Pins connected to the Seven Segment Display
#define SSD_A_O P2OUT
#define SSD_B_O P2OUT
#define SSD_C_O P2OUT
#define SSD_D_O P2OUT
#define SSD_E_O P2OUT
#define SSD_F_O P2OUT
#define SSD_G_O P2OUT
#define SSD_DP_O P2OUT
#define SSD_0_O P1OUT
#define SSD_1_O P1OUT
#define SSD_2_O P1OUT
#define SSD_3_O P1OUT
//Pin Number for the 12 Pins connected to the Seven Segment Display
#define SSD_A_B BIT0
#define SSD_B_B BIT1
#define SSD_C_B BIT2
#define SSD_D_B BIT3
#define SSD_E_B BIT4
#define SSD_F_B BIT5
#define SSD_G_B BIT6
#define SSD_DP_B BIT7
#define SSD_0_B BIT0
#define SSD_1_B BIT1
#define SSD_2_B BIT2
#define SSD_3_B BIT3

int ssd_val[4]={0,0,0,0}; //Variable to hold the seven segment display values
int ssd_dot[4]={0,0,0,0}; //Variable to hold the dots
int ssd_iter=0; //Required for switching between the digits in succession
int Seg7(int a){

    switch(a){
    //Returns proper code for each number
    case 0: return 0x00FC; // 1111 1100
    case 1: return 0x0060; // 0110 0000
    case 2: return 0x00DA; // 1101 1010
    case 3: return 0x00F2; // 1111 0010
    case 4: return 0x0066; // 0110 0110
    case 5: return 0x00B6; // 1011 0110
    case 6: return 0x00BE; // 1011 1110
    case 7: return 0x00E0; // 1110 0000
    case 8: return 0x00FE; // 1111 1110
    case 9: return 0x00F6; // 1111 0110

    }
    //If number is greater than 9, don't display anything
    return 0x00;

}
void update7Seg(int iter)
{
    int disp=Seg7(ssd_val[iter]); //Get the code for the digit
    //Turn off all the segments
    SSD_A_O&=~SSD_A_B;
    SSD_B_O&=~SSD_B_B;
    SSD_C_O&=~SSD_C_B;
    SSD_D_O&=~SSD_D_B;
    SSD_E_O&=~SSD_E_B;
    SSD_F_O&=~SSD_F_B;
    SSD_G_O&=~SSD_G_B;
    SSD_DP_O&=~SSD_DP_B;
    switch(iter) //Choose the proper digit
    {
    case 0:
        SSD_0_O|=SSD_0_B;
        SSD_1_O&=~SSD_1_B;
        SSD_2_O&=~SSD_2_B;
        SSD_3_O&=~SSD_3_B;
        break;
    case 1:
        SSD_1_O|=SSD_1_B;
        SSD_0_O&=~SSD_0_B;
        SSD_2_O&=~SSD_2_B;
        SSD_3_O&=~SSD_3_B;
        break;
    case 2:
        SSD_2_O|=SSD_2_B;
        SSD_0_O&=~SSD_0_B;
        SSD_1_O&=~SSD_1_B;
        SSD_3_O&=~SSD_3_B;
        break;
    case 3:
        SSD_3_O|=SSD_3_B;
        SSD_0_O&=~SSD_0_B;
        SSD_1_O&=~SSD_1_B;
        SSD_2_O&=~SSD_2_B;
        break;

    }
    //Turn on the required segments
    disp=disp>>1;
    if(disp&0x01)SSD_G_O|=SSD_G_B;
    disp=disp>>1;
    if(disp&0x01)SSD_F_O|=SSD_F_B;
    disp=disp>>1;
    if(disp&0x01)SSD_E_O|=SSD_E_B;
    disp=disp>>1;
    if(disp&0x01)SSD_D_O|=SSD_D_B;
    disp=disp>>1;
    if(disp&0x01)SSD_C_O|=SSD_C_B;
    disp=disp>>1;
    if(disp&0x01)SSD_B_O|=SSD_B_B;
    disp=disp>>1;
    if(disp&0x01)SSD_A_O|=SSD_A_B;
    //Turn on the required Dots
    if(ssd_dot[iter]) SSD_DP_O|=SSD_DP_B;
}
void ssd_init()
{
    //Initialize all the 12 pins as output pins
    SSD_A_D|=SSD_A_B;
    SSD_B_D|=SSD_B_B;
    SSD_C_D|=SSD_C_B;
    SSD_D_D|=SSD_D_B;
    SSD_E_D|=SSD_E_B;
    SSD_F_D|=SSD_F_B;
    SSD_G_D|=SSD_G_B;
    SSD_DP_D|=SSD_DP_B;
    SSD_0_D|=SSD_0_B;
    SSD_1_D|=SSD_1_B;
    SSD_2_D|=SSD_2_B;
    SSD_3_D|=SSD_3_B;
}
int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P2SEL&=~(BIT6|BIT7); //We are using P2.6 and P2.7 which are to be connected to the oscillator by default
    P2SEL2&=~(BIT6|BIT7); //So we set them as GPIO pins

    ssd_init(); //Set the pins connected to the SSD as output pins

    TACTL|=TASSEL_2+MC_1+TAIE; //Start Timer0 with SMCLK clock source, UP mode and enable overflow interrupt
    TACCR0=7000-1; //Deciding the refresh rate for the seven segment display

    __enable_interrupt(); //Enable maskable interrupts

    while(1)//Run code forever
    {   //A simple code to display 0 to 9999 and back to 0
        __delay_cycles(50000);
                ssd_val[0]++;
                if(ssd_val[0]==10) {ssd_val[1]++;ssd_val[0]=0;}
                if(ssd_val[1]==10) {ssd_val[2]++;ssd_val[1]=0;}
                if(ssd_val[2]==10) {ssd_val[3]++;ssd_val[2]=0;}
                if(ssd_val[3]==10) {ssd_val[3]=0;}

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
        update7Seg(ssd_iter); //Call the  update7Seg(); function
        ssd_iter++; //Increment iter
        if(ssd_iter==4) //If digits have been refreshed
            ssd_iter=0; //Reset iter, to refresh again
        TACTL&=~(TAIFG); //Reset the interrupt flag
    }
}

