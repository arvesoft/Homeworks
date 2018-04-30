#include <p18cxxx.h>
#include <p18f8722.h>
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#define _XTAL_FREQ   40000000

#include "Includes.h"
#include "LCD.h"


/*#########################NOTLAR########################

 GUNCEL HATALAR:
 * Gecislerde ADIF kapatilmali, ekrana sayiyi basiyor. (cozuldu)
 * Attempts gecislerinde 7-segment display bozuluyor. (cozuldu)
 * timer0 su an 300ms, 250ms olmali (50ms set edildi ( *5 ), counterlar duzeltilecek(duzeltildi) )
 * kucuk flickerlar (sorun degil)(ama cozduk)
 * Tekrar baslarken sikinti (cozuldu)



 ######################################################*/

 // Nazir Bilal Yavuz - 2099471
 // Cemal Erat - 2098960



unsigned int oldval, newval, curoffset, ischanged, locked, deadclock;
// -> oldval and newval for ADC
// -> curoffset is for which offset on the LCD
// -> ischanged is for looking ADCON
// -> locked checks blinking on LCD
// -> deadclock for the restarting program
int count, qq, rr, state2_cnt, blink_count, timer1_cnt, zz, timer1_sec, at_sec, adc_cnt;
// -> count, state2_cnt, timer1_cnt is for counting interrupts
// -> rr, qq, zz is for changing steps in interrupts
// -> blink_count is for counting how many blinking on state2
// -> at_sec is for counting seconds while waiting 20 sec
// -> adc_cnt is for 100 ms adcon check
int rb6_clicked, rb7_clicked, state, attempts;
// -> rb6_clicked, rb7_clicked for push and release
// -> state is for which state:
// -> state1 = when user setting passwords
// -> state2 = 3 times blink passwords
// -> state3 = entering new passwords
// -> state4 = checking passwords
// -> attemps is number of attemps remaining
int pass[4],trypass[4];
// -> pass is for my password
// -> trypass is for new passwords


void blinking_state(int val);
int numberGetter(int adress);
void ForTimer0Int();
void ADCONEnabler();
void startExceptTimer1();
void CheckFor();
void interrupt isr();
void segment_display2();
void segment_display3();
void kickstart();


void updateLCD();

void InitInputs(){

    TRISE = 0x02; // -> RE1 is input
}

void ForTimer1Int()
{
    TMR1IE = 1; // -> timer1 interrupt enabled
    T1CON = 0B00100001;  // -> 1:4 prescale for ~25ms, and timer 1 is on ([0] bit)
}

void ForTimer0Int()
{
    T0CON = 0x82; // -> 1:8 prescale for ~50ms, timer0 interrupt enabled,
    TMR0IF = 0;
}

void ADCONEnabler(){

    ADCON0 = 0x33;  /*  For the enable  */
    ADCON1 = 2;     /*  ADC Interrupt   */
    ADFM = 1;
    TRISH4 = 1;
}

void Disabler(){

    INTCON = 0;          /* In This Function */
    TMR1IE = 0;          /*  All Interrupts  */
    T1CON = 0;           /*   Are Disabled   */
    T0CON = 0;           /*  For Restarting  */
    TMR0IF = 0;
    ADCON0 = 0;
    ADCON1 = 0;
    RBIE=0;
    INTCON2bits.RBPU=1;
    ADIF = 0;

}

void ForRbInt()
{
    PORTB=0;                    /* This function will enable */
    LATB=0;                     /*  Rb6 and Rb7 Interrupts   */

    TRISB=0B11000000; // -> RB6 and RB7 is input
    RBIE=1; // -> Interrupt enabled
    INTCON2bits.RBPU = 0;

}

void startExceptTimer1(){

    INTCON=0xE0;
    ForTimer0Int();
    ForRbInt();
    ADCONEnabler();
}


int numberGetter(int LCDVAL){

    /*  ADCON represents which value.
        LCDVAL is ADRES which came from
        ADCON.                           */

    if(0 <= LCDVAL && LCDVAL < 100)
        return 0;
    else if(100 <= LCDVAL && LCDVAL < 200)
        return 1;
    else if(200 <= LCDVAL && LCDVAL < 300)
        return 2;
    else if(300 <= LCDVAL && LCDVAL < 400)
        return 3;
    else if(400 <= LCDVAL && LCDVAL < 500)
        return 4;
    else if(500 <= LCDVAL && LCDVAL < 600)
        return 5;
    else if(600 <= LCDVAL && LCDVAL < 700)
        return 6;
    else if(700 <= LCDVAL && LCDVAL < 800)
        return 7;
    else if(800 <= LCDVAL && LCDVAL < 900)
        return 8;
    else if(900 <= LCDVAL)
        return 9;

}

void CheckFor(){


    /* This function checks if
       ADCON value is changed
       or not                  */


    newval = numberGetter(ADRES);

    // oldval is oldvalue and checks newvalue

    if(newval != oldval){
        WriteCommandToLCD(0x8b + curoffset);
        WriteDataToLCD(newval + 48);
        ischanged = 1;
        oldval = newval;

    }


}

void interrupt isr()
{

    if(INTCONbits.TMR0IF){
       INTCONbits.TMR0IF = 0; // Timer0 interrupt came and interrupt is handled
       if( (state==1 || state==3) && count == 5 && !ischanged ){
           // In state1 and state3 with 250ms interval
           // curoffset is blinking

           if(qq){
               WriteCommandToLCD(0x8b + curoffset);
               WriteDataToLCD('#');
               qq=0;
           }

           else{
               WriteCommandToLCD(0x8b + curoffset);
               WriteDataToLCD(' ');
               qq=1;
           }

       }


       count++; // 1 count is 50 ms and
       count %= 6; // count is always 0,1,..,5


       if(state==2 && blink_count<3){
           // blink_count 3 means it shows new pin 3 times

           ADIF = 0; // When LCD is blinking there will be no ADCON interrupt

           if( state2_cnt == 10 ){ // For 500 ms state2_cnt will be 10 (50 * 10)

               if(rr){
                   char newpws[16]="   ---    ---   ";
                   newpws[6] = pass[0]+48; // Setting new pin here
                   newpws[7] = pass[1]+48;
                   newpws[8] = pass[2]+48;
                   newpws[9] = pass[3]+48;
                   WriteCommandToLCD(0x80);
                   WriteStringToLCD(" The new pin is ");
                   WriteCommandToLCD(0xc0);
                   WriteStringToLCD(newpws);
                   rr=0;
               }

               else{

                   blink_count++; // When LCD cleared, blink_count will increment
                   ClearLCDScreen();
                   rr=1;
               }
           }

           state2_cnt++;
           state2_cnt %= 11; // state2_cnt is always 0,1,..,10
       }
       else if(state==2 && blink_count==3){

           if(state2_cnt != 10)
               state2_cnt++; // last time wait 500 ms

           else{

               LATJ = 0; // 7 segment cleared
               state++; // new state is 3
               ADIF = 1; // Now ADCON can interrupt
               locked = 0; // dont blink at first value
               WriteCommandToLCD(0x80);
               WriteStringToLCD(" Enter pin:#### ");
               WriteCommandToLCD(0xc0);
               WriteStringToLCD("  Attempts:2    ");
               ForTimer1Int(); // Enable timer 1 interrupt for 120 sec
               ADCONEnabler(); // Enable ADCON

           }
       }

       adc_cnt++;
       adc_cnt %= 3; // adc_cnt is always 0,1,2

       if(adc_cnt == 2){ // For 100 ms ADCON interrupt

           if(ADIF){ // ADCON interrupt came

               while(ADCON0bits.DONE);
               ADCON0bits.GO = 0;

               if( !locked ){ // sets first value of ADCON

                   oldval = numberGetter(ADRES);
                   locked=1;

               }

                ADIF = 0; // ADCON interrupt handled
                CheckFor(); // Checks if value changed
                ADCONEnabler(); // ADCON enabled

           }
        }
    }

    if(INTCONbits.RBIF == 1){ // Timer1 interrupt came

        INTCONbits.RBIF=0; // Timer1 Interrupt handled

        if( (state==1 || state==3) && curoffset!=3 && ischanged && PORTBbits.RB6==0 ){
            rb6_clicked = 1;
        }

        // For rb6 push and release

        if( (state==1 || state==3) && curoffset!=3 && ischanged && rb6_clicked && PORTBbits.RB6==1 ){
            // When rb6 pushed and release if value is changed via ischanged
            // set value and move right with incrementing
            // curroffset
            if(state==1) pass[curoffset] = newval;
            else trypass[curoffset] = newval;
            curoffset = curoffset+1;
            rb6_clicked = 0;
            ischanged = 0;
            locked = 0; // for the first value
        }

        if( (state==1 || state==3) && curoffset==3 && ischanged && PORTBbits.RB7==0 ){
            rb7_clicked = 1;
        }

        // For rb7 push and release

        if((state==1 || state==3) && curoffset==3 && ischanged && rb7_clicked && PORTBbits.RB7==1 ){
            // When rb7 pushed it checks if its on last offset
            // and if value is changed via ischanged
            if(state==1) pass[curoffset] = newval;
            else trypass[curoffset] = newval;
            rb7_clicked = 0;
            ischanged = 0;
            locked = 0;
            curoffset=0;
            state++; // if state is 4 it will check passwords
        }
    }

    if(PIR1bits.TMR1IF){

        segment_display3(); // 7 segment display will count 120 sec

        timer1_cnt++;
        timer1_cnt %= 38; // For 1 sec it will loop 38 times

        if( timer1_cnt==37 ){

            timer1_sec--; // Second--;

            if(!timer1_sec){

                deadclock = 1; // if second is 0 it will restart
            }
        }

        PIR1bits.TMR1IF = 0; // Interrupt handled

    }
}

int seg_table(int val){

    /* LATJ numbers -- retrieved from previous homework -- */

    if(val==0) return 0b00111111 ;//0 representation in 7-seg. disp. portJ
    if(val==1) return 0b00000110 ;//1 representation in 7-seg. disp. portJ
    if(val==2) return 0b01011011 ;//2 representation in 7-seg. disp. portJ
    if(val==3) return 0b01001111 ;//3 representation in 7-seg. disp. portJ
    if(val==4) return 0b01100110 ;//4 representation in 7-seg. disp. portJ
    if(val==5) return 0b01101101 ;//5 representation in 7-seg. disp. portJ
    if(val==6) return 0b01111101 ;//6 representation in 7-seg. disp. portJ
    if(val==7) return 0b00000111 ;//7 representation in 7-seg. disp. portJ
    if(val==8) return 0b01111111 ;//8 representation in 7-seg. disp. portJ
    if(val==9) return 0b01100111 ;//9 representation in 7-seg. disp. portJ
}

void segment_display2() {

    // LATJ wil display ----
    LATH = 0xF;
    LATJ = 0x40;
}

int secmod1,secmod2,secmod3;

void segment_display3() {

    // LATJ will display current second and
    // LATJ = 0 is clearing

    LATH = 0x1;
    LATJ = seg_table(0);

    __delay_ms(2);
    LATH = 0x8;
    LATJ = 0;
    LATJ = seg_table(secmod1=timer1_sec%10);

    __delay_ms(2);
    LATH = 0x4;
    LATJ = 0;
    LATJ = seg_table(secmod2=(timer1_sec/10)%10);

    __delay_ms(2);
    LATH = 0x2;
    LATJ = 0;
    LATJ = seg_table(secmod3=(timer1_sec/100)%10);

    __delay_ms(2);
    LATH = 0;


}

void segment_display_end(){

    // Infinite loop if pass is true

    while(1){
        secmod1=timer1_sec%10;
        secmod2=(timer1_sec/10)%10;
        secmod3=(timer1_sec/100)%10;
        __delay_ms(1);
        LATH = 0x8;
        LATJ = seg_table(secmod1);
        __delay_ms(1);
        LATH = 0x4;
        LATJ = seg_table(secmod2);
        __delay_ms(1);
        LATH = 0x2;
        LATJ = seg_table(secmod3);
        __delay_ms(1);
        LATH = 0x1;
        LATJ = seg_table(0);
    }
}

void pass_check(){

    // Checks password
    if( pass[0]==trypass[0] && pass[1]==trypass[1] && pass[2]==trypass[2] && pass[3]==trypass[3] ){
        WriteCommandToLCD(0x80);
        WriteStringToLCD("Safe is opening!");
        WriteCommandToLCD(0xc0);
        WriteStringToLCD("$$$$$$$$$$$$$$$$");

        // If it is true disable interrupts and show segment_display_end();
        INTCON=0;
        segment_display_end();
    }
    else{

        attempts--;
        if(!attempts){ // Pass is wrong and attempts is 0

            ADIF=0; // While waiting 20 sec interrupts are disabled
            INTCONbits.RBIE = 0; // While waiting 20 sec interrupts are disabled

            WriteCommandToLCD(0x80);
            WriteStringToLCD(" Enter pin:XXXX ");
            WriteCommandToLCD(0xc0);
            at_sec=timer1_sec;
            WriteStringToLCD("Try after 20 sec");

            while(at_sec-timer1_sec<21){
                // Wait 20 sec
            }

            attempts = 1;
            WriteCommandToLCD(0xc0);
            WriteStringToLCD("  Attempts:1    ");
            ADCONEnabler();
        }

        ADIF=1;
        INTCONbits.RBIE = 1; // interrupts are enabled

        WriteCommandToLCD(0x80);
        WriteStringToLCD(" Enter pin:#### ");
        WriteCommandToLCD(0xcb);
        WriteDataToLCD(attempts+48); // Number of attempts will show here
        state--; // State is 3
    }
}

void blinking_state( int val ){

    locked=0;
    startExceptTimer1();
//    TMR0ON = 1;
//    TMR0 = 0;
    while(1){
        if(state==4){
            pass_check(); // If state is 4 check pass
        }
        if(deadclock) break; // if deadlock is 1 break loop
    }
}

void kickstart(){

    __delay_ms(15);
    __delay_ms(15);
    __delay_ms(15);
    __delay_ms(15);
    attempts = 2;
    oldval = 0; newval = 0; curoffset = 0; ischanged = 0; locked = 0; deadclock = 0;
    rb6_clicked = 0; rb7_clicked = 0 ; state=0;
    count = 0; qq = 0 ; state2_cnt = 8 ; blink_count = 0 ;timer1_cnt = 0 ; zz = 0 ;at_sec=0; adc_cnt = 0;
    rr=1;
    timer1_sec=120;

    InitLCD();			// Initialize LCD in 4bit mode
    InitInputs();

    ClearLCDScreen();           // Clear LCD screen
    WriteCommandToLCD(0x80);
    WriteStringToLCD(" $>Very  Safe<$ ");	// Show very safe
    WriteCommandToLCD(0xc0);
    WriteStringToLCD(" $$$$$$$$$$$$$$ ");

    int cont=1;
    ADCONEnabler();

    while(1){

        if( cont && PORTEbits.RE1 == 0 ){ // RE1 push and release
            while( PORTEbits.RE1 == 0 );

            for(int i=0;i<1665;i++){
                for(int j=0;j<1000;j++){
                    // Busy wait for 3 sec
                }
            }

            ClearLCDScreen();
            WriteCommandToLCD(0x80);
            WriteStringToLCD(" Set a pin:#### "); // Show set pin
            ADCONEnabler();
            locked = 0;
            segment_display2(); // 7 segment will show ----
            cont=0;
            state++; // State is 1
            blinking_state(1); // Code will be loop in blinking state
            Disabler(); // If blinking state finish, it means code will restart
                        // so disable all inrerrupts
            break;
        }
    }

}

// Main Function
void main(void)
{
    while(1){

    kickstart(); // start or restart program

    }
}

