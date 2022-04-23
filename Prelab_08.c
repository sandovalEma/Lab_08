/* 
 * File:   Prelab_08.c
 * Author: sando
 *
 * Created on 18 de abril de 2022, 04:17 PM
 */
/*
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
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

#include <xc.h>
#include <stdint.h>

// --------------- CONSTANTES --------------- //
#define _XTAL_FREQ 4000000      // conf. a 4MHz

// --------------- PROTOTIPO DE FUNCIONES --------------- //
 
void setup(void);

//--------------- INTERRUPCIONES --------------- //
void __interrupt() isr(void){
    if (PIR1bits.ADIF){
        if (ADCON0bits.CHS == 0){       //INTERRUPCION AN0
            PORTC   = ADRESH;
        } else if (ADCON0bits.CHS == 1){
            PORTD   = ADRESH;
            
        }
        PIR1bits.ADIF = 0;
    }
   return;         //INTERRUPT RETURN  
}

// --------------- CICLO PRINCIPAL  --------------- //
void main(void) {
    setup();
    while(1){
        if(ADCON0bits.GO == 0){   
            if(ADCON0bits.CHS == 0b0000){
                ADCON0bits.CHS = 0b0001;    // Cambio de canal
            }
            
            else if(ADCON0bits.CHS == 0b0001){
                ADCON0bits.CHS = 0b0000;    // Cambio de canal  
            }
            __delay_us(40);                 // Tiempo de adquisici n
            ADCON0bits.GO = 1;              // Iniciamos proceso de conversi n?
        } 
    }
    return;
}

// --------------- CONFIGURACION --------------- //   
void setup(void){
    ANSEL   = 0b00000011;       // AN0 AN1 analógicos
    ANSELH  = 0;                // I/O digitales

    TRISA   = 0b00000011;       // AN0 AN1 entradas
    TRISC = 0;                  // PORTC como salida
    TRISD = 0;                  // PORTD como salida
    
    PORTA = 0;                  // Limpiamos PORTB
    PORTC = 0;                  // Limpiamos PORTC
    PORTD = 0;                  // Limpiamos PORTD
    
// --------------- CONFIGURACION DE OSCILADOR --------------- //
    OSCCONbits.SCS      = 1;        //OSC INTERNO
    OSCCONbits.IRCF     = 0b0110;   //4MHZ
    
    
// --------------- CONFIGURACION DE ADC --------------- //    
    ADCON0bits.ADCS     = 0b01;     // Fosc/8
    ADCON1bits.VCFG0    = 0;        // VDD
    ADCON1bits.VCFG1    = 0;        // VSS
    ADCON0bits.CHS      = 0b0000;   // AN0
    ADCON1bits.ADFM     = 0;        // LEFT ADJUSTED
    ADCON0bits.ADON     = 1;        // MODULO ADC HABILITADO 
    __delay_us(40);                 // SAMPLE TIME
   
    
}//setup

void resettmr0(void){
    TMR0 = 253;                     // para 15 ms
    INTCONbits.T0IF=0;              // Limpiamos bandera de TMR0
}

// --------------- CONFIGURACION DE INTERRUPCIONES --------------- //    
void configint(void){
    
    INTCONbits.T0IE = 1;            // Habilitamos interrupcion TMR0
    INTCONbits.T0IF = 0;            // Limpiamos bandera de TMR0
    PIR1bits.ADIF   = 0;
    PIE1bits.ADIE   = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE  = 1;
    
}

int display_table(int x){           // funcion que asigna los valores para encender
   switch (x){                      // los leds de los displays segun el numero
       case 0: return 0b00111111;
       //break;
       case 1: return 0b00000110;
      // break;
       case 2: return 0b01011011;
      // break;
       case 3: return 0b01001111;
       //break;
       case 4: return 0b01100110;
       ///break;
       case 5: return 0b01101101;
       //break;
       case 6: return 0b01111101;
     // break;
       case 7: return 0b00000111;
       //break;
       case 8: return 0b01111111;
       //break;
       case 9: return 0b01101111;
       //break;
       default:  return 0b00000000; //control
               break;
   }//switch(x)
}// display_table

int cen(int x){                 // funcion que revisa cuantos cienes hay
    int temp = x;               // asigna una variable local temporal
    int cont = 0;               // contador local
    while( temp-100 >= 0){      // resta para comprobar si ha habido underflow
        temp = temp - 100;      // le resta 100 al temporal si no hay underflow
        cont++;                 // +1 cien
    }
    return cont;                //  regresa el valor de contador
}

int dec(int x){                 // funcion que revisa cuantas decenas hay
    int temp = x;
    int cont = 0;
    while( temp-100 >= 0){      // se eliminan cienes
        temp = temp - 100;
    }//while
    while( temp-10 >= 0){
        temp = temp - 10;
        cont++;
    }
    return cont;
}

int uni(int x){
    int temp = x;
    while( temp-100 >= 0){          // se eliminan cienes
        temp = temp - 100;
    }
    while( temp-10 >= 0){           // se eliminan decenas
        temp = temp - 10;
    }
    return temp;
}
    // --------------- CONFIGURACION DE TIMER 0 --------------- // 
void setuptmr0(void){
    OPTION_REGbits.T0CS = 0;    // Uso de reloj interno
    OPTION_REGbits.PSA = 0;     // Uso de Prescaler con TMR0
    OPTION_REGbits.PS = 0b0111; // Prescaler de 1:256 
    resettmr0();
   
}
*/