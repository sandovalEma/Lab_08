/* 
 * File:   Postlab_08.c
 * Author: sando
 *
 * Created on 20 de abril de 2022, 11:45 AM
 */
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

#define _XTAL_FREQ 8000000 
#define _TMR0_VALUE 217

//--------------- VARIABLES --------------- //
char display[10]={0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X67};    
// Se define una tabla para mostrar los valores en los Display 7 segmentos.
int banderas; 
int UNI;
int DECE;
int CEN;
int cont;
int res;

// --------------- PROTOTIPO DE FUNCIONES --------------- //
 
void confi(void);
void ISR (void);
int  division(void);


//--------------- INTERRUPCIONES --------------- //

void __interrupt() ISR(void){
    if(T0IF == 1){
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
            cont = ADRESH;
        }
        PIR1bits.ADIF = 0;
    }
    return;
}

// --------------- CICLO PRINCIPAL  --------------- //
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

// --------------- CONFIGURACION --------------- //    

void confi(void){
    ANSEL = 0b00000011;
    ANSELH = 0X00;              // I/O digitales

    TRISA = 0x03;               // PORTA como salida, excepto RA0 y RA1
    TRISB = 0;                  // PORTC como salida
    TRISC = 0;                  // PORTC como salida
    TRISD = 0;                  // PORTD como salida

    PORTA = 0;                  // Limpiamos PORTA
    PORTB = 0;                  // Limpiamos PORTB
    PORTC = 0;                  // Limpiamos PORTC
    PORTD = 0;                  // Limpiamos PORTD
  
  // --------------- CONFIGURACION DE OSCILADOR --------------- //    
    OSCCONbits.SCS = 1;         // Oscilador interno
    OSCCONbits.IRCF = 0b0111;   // 8MHz
  
 // --------------- CONFIGURACION DE INTERRUPCIONES --------------- //    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.T0IE = 1;        // Habilitamos interrupciones del TMR0
    INTCONbits.T0IF = 0;        // Limpiamos bandera de interrupción del TMR0
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    banderas= 0X00; 
  
 // --------------- CONFIGURACION DE TIMER 0 --------------- //    
    OPTION_REGbits.T0CS = 0;    // Uso de reloj interno
    OPTION_REGbits.PSA = 0;     // Uso de Prescaler con TMR0
    OPTION_REGbits.PS = 0b0111; // Prescaler de 1:256 
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
  
}
// --------------- FUNCIONES --------------- //
int division(void){
    CEN = cont/100; //declaramos que el valor de la centena estara dada por el 
                    //valor del puerto C dividido 100
    res = cont%100; // el residuo de la division anterior sera colocado en otra 
                    //variable
    DECE = res/10;  // la variable antes mencionada se dividira por 10 y se 
                    // encontraran las decenas
    UNI = res%10;   // el residuo de la operacion anterior seran nuestras unidades
} 
