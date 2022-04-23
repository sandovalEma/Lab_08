/* 
 * File:   Lab_08.c
 * Author: sando
 *
 * Created on 18 de abril de 2022, 07:40 PM
 */

#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
/*
#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 8000000 
#define _TMR0_VALUE 217

char display[10] = {0X3F,0X06,0X5B,0X04F,0X66,0X6D,0X7D,0X07,0X7F,0X67};
//Aqui se definen las variables para las 
//conversiones a decimal y el multiplexado
int banderas;
int UNI;
int DECE;
int CEN;
int cont;
int res;


///////Prototipos////////
void confi(void);
void ISR (void);
int  division(void);


/////Interrupcion///////////

void __interrupt() ISR(void){
   /* if(T0IF == 1){
     PORTB = 0X00;
     
       if (banderas == 0b00000000){// Cuando la interrupcion del timer 0 se
         PORTBbits.RB2 = 0;        // encienda, entonces revisara la variable 
         PORTBbits.RB0 = 1;        // banderas y encendera mi display 
         PORTD = (display[UNI]);   // correspondiente y colocar el valor deseado
         banderas = 0b00000001;   
        }
     else if (banderas == 0b00000001){
         PORTBbits.RB0 = 0;
         PORTBbits.RB1 = 1;
         PORTD = (display[DECE]);
         banderas = 0b00000010;   
        }
     else if (banderas == 0b00000010){
         PORTBbits.RB2 = 1;
         PORTBbits.RB1 = 0;
         PORTD = (display[CEN]);
         banderas = 0b00000000;   
            }
     TMR0 = _TMR0_VALUE;
     INTCONbits.T0IF = 0;
    
    } 
    if (PIR1bits.ADIF){
        if(ADCON0bits.CHS == 0){
            PORTC = ADRESH;
        }
        else{
            PORTD = ADRESH;
        }
        PIR1bits.ADIF = 0;
    }
    
   

    return;
}

void main(void) {
    confi();
    ADCON0bits.GO = 1;
    
    while(1)
    {
        division();
        if(ADCON0bits.GO == 0){
            if (ADCON0bits.CHS == 0){
                ADCON0bits.CHS = 1;
            }
            else {
                ADCON0bits.CHS = 0;
                
            }
            __delay_us(200);
            ADCON0bits.GO = 1;
        
        }
    }
    
    return;
}

void confi(void){
  ANSEL = 0b00000011;
  ANSELH = 0X00;
  //Aqui configuramos entradas y salidas
  TRISA = 0X03;
  TRISB = 0X00;
  TRISC = 0X00;
  TRISD = 0X00;
  
  PORTA = 0X00;
  PORTB = 0X00;
  PORTC = 0X00;
  PORTD = 0X00;
  
  // colocamos nuestro oscilador interno en 8Mhz
  OSCCONbits.IRCF2 = 1;
  OSCCONbits.IRCF1 = 1;
  OSCCONbits.IRCF0 = 1; 
  OSCCONbits.SCS = 1;
  
  //Llamamos a las banderas del timer 0
  INTCONbits.GIE = 1;
  INTCONbits.T0IE = 1;
  INTCONbits.T0IF = 0;
  
  //configuramos el timer 0
  OPTION_REGbits.PSA = 0; 
  OPTION_REGbits.T0CS = 0; 
  OPTION_REGbits.PS2 = 1; 
  OPTION_REGbits.PS1 = 1; 
  OPTION_REGbits.PS0 = 1; 
  TMR0 = _TMR0_VALUE; 
  
  //Configuracion del ADC
  ADCON1bits.ADFM = 0;
  ADCON1bits.VCFG0 = 0;
  ADCON1bits.VCFG1 = 0;
  
  ADCON0bits.ADCS = 1;
  ADCON0bits.CHS = 0;
  __delay_us(200);
  ADCON0bits.ADON = 1;
  __delay_us(200);
  
  
  //Configuracion de interrupciones
  PIR1bits.ADIF = 0;
  PIE1bits.ADIE = 1;
  INTCONbits.PEIE = 1;
  INTCONbits.GIE = 1;
  INTCONbits.T0IE = 1;
  INTCONbits.T0IF = 0;
  
  banderas= 0X00;
 
}
int division(void){
    CEN = cont/100; //declaramos que el valor de la centena estara dada por el 
                    //valor del puerto C dividido 100
    res = cont%100; // el residuo de la division anterior sera colocado en otra 
                    //variable
    DECE = res/10;  // la variable antes mencionada se dividira por 10 y se 
                    // encontraran las decenas
    UNI = res%10;   // el residuo de la operacion anterior seran nuestras unidades
}
*/