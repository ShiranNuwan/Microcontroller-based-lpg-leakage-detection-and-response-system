
// * pin description
// *
// * MQ 2         =   RA0 
// *
// * LED 1        =   RD0
// * Buzzer       =   RD1
// * LED 2        =   RD2
// * LED 3        =   RD3
// *
// * RS           =   RB5
// * EN           =   RB4
// * D4           =   RB3
// * D5           =   RB2
// * D6           =   RB1
// * D7           =   RB0
// *
// * RX           =   RC7   
// * TX           =   RC6
// * fan          =   RD4
// * 


#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>
//#include <pic.h>
#include "lcd.h"
#include "serial.h"
#include <stdio.h>
#include <stdlib.h>

#define Baud_rate 9600
#define _XTAL_FREQ 16000000

unsigned long currentMillis = 0 ;

unsigned long blink_previousMillis = 0 ;
const long blink_interval = 500 ;

int analog_read = 0 ;
int mq2_thresh = 300 ;

char array[20] ;

unsigned long buzzer_previousMillis = 0 ;
const long buzzer_interval = 500 ;
int buzzer_state = 0 ;

unsigned long sms_previousMillis = 0 ;
const long sms_interval = 10000 ;
int sms_state = 0 ;
int f_sms = 1 ;

unsigned long del_previousMillis = 0 ;
const long del_interval = 1000 ;
int del_c = 0 ;



void Init_timer(){
    OPTION_REG = 0b00000100 ; // 1:32
    TMR0 = 100 ;
    GIE = 1 ;
    PEIE = 1 ;
    TMR0IE = 1 ;
    }

__interrupt() void timer_isr(void){
    if( TMR0IF == 1){
        TMR0IF = 0 ;   
        currentMillis ++ ;
        } 
    }

void ADC_Initialize(){
    ADCON0 = 0b01000001 ;
    ADCON1 = 0b11000000 ;
    }

unsigned int ADC_Read(unsigned char channel){
    ADCON0 &= 0x11000101 ;
    ADCON0 |= channel<<3 ;
    __delay_ms(2) ;
    GO_nDONE = 1 ;
    while(GO_nDONE) ;
    return ((ADRESH<<8)+ADRESL) ;
    }

void blink(){
    if(currentMillis - blink_previousMillis >= blink_interval){
        blink_previousMillis = currentMillis ;
        RD0 =~ RD0 ;     
        }
    }

void initializer(){ 
//    UART_send_string(" \n\r LPG Gas Detector ") ;
//    UART_send_string(" \n\r Version 0.1 ") ;
//    UART_send_string(" \n\r Initializing ") ;
    Lcd_Clear() ;
    Lcd_Set_Cursor( 1, 1) ;
    Lcd_Print_String( "Initializing") ;
    __delay_ms( 500) ;
    for(int i = 1 ; i <= 16 ; i++){
        Lcd_Set_Cursor( 2, i) ;
        Lcd_Print_Char(0b11111111) ;
//        UART_send_string(".") ;
        __delay_ms( 50) ;
        }
    Lcd_Clear() ;
    Lcd_Set_Cursor( 1, 1) ; Lcd_Print_String( "Initializing") ;
    Lcd_Set_Cursor( 2, 1) ; Lcd_Print_String( "Done !") ;
//    UART_send_string(" \n\r DONE ") ;
    
    RD1 = 1 ;   __delay_ms( 50) ;   RD1 = 0 ;   __delay_ms( 50) ;
    RD1 = 1 ;   __delay_ms( 50) ;   RD1 = 0 ;
    __delay_ms( 50) ;
    Lcd_Clear() ;
    }

void operate_buzzer(){
    if(currentMillis - buzzer_previousMillis >= buzzer_interval){
        buzzer_previousMillis = currentMillis ;
        if(buzzer_state == 1){
            RD1 =~ RD1 ;
            }
        else{
            RD1 = 0 ;
            }
        }
    }

void read_sensor(){
    
    analog_read = (ADC_Read(0)) ;
//    analog_read = analog_read * 0.488281 ;
    sprintf( array, "MQ 2 = %d  ",analog_read ) ;
//    UART_send_string(" \n\r") ;
//    UART_send_string(array) ;

    Lcd_Set_Cursor( 1, 1) ; Lcd_Print_String( array) ;
    
    if(analog_read > mq2_thresh){
        Lcd_Set_Cursor( 2, 1) ; Lcd_Print_String("Warning   ") ;
        buzzer_state = 1 ;
        RD2 = 0 ;
        RD3 = 1 ;
        RD4 = 1 ;
        
        if(f_sms == 1){
            f_sms = 0 ;
            sms_state = 1 ;
            del_c = 0 ;
            
            }
        
        if(currentMillis - sms_previousMillis >= sms_interval){
            sms_previousMillis = currentMillis ;
            sms_state = 1 ;
            }
        }
    
    else{
        Lcd_Set_Cursor( 2, 1) ; Lcd_Print_String("Normal    ") ;
        buzzer_state = 0 ;
        sms_state = 0 ;
        del_c = 0 ;
        RD2 = 1 ;
        RD3 = 0 ;
        RD4 = 0 ;
        
        f_sms = 1 ;
    
        }
    }

void send_sms(){
    if(sms_state == 1){
        if(currentMillis - del_previousMillis >= del_interval ){
            del_c ++ ;
            
            if(del_c == 1){ 
                UART_send_string("\n\r") ;
                UART_send_string("\n\rAT\n\r") ;
                }
            if(del_c == 2){ 
                UART_send_string("\n\rAT+CMGF=1\n\r ") ;
                }
            if(del_c == 3){ 
                UART_send_string("\n\rAT+CMGS=\"+94771234567\"\n\r") ;
                }
            if(del_c == 4){ 
                UART_send_string("\n\rGas Leak Detected\n\r ") ;
                UART_send_char(26) ;
                UART_send_string("\n\r") ;
                }
            if(del_c == 5){ 
                sms_state = 0 ;
                del_c = 0 ;
                }
            }
        }
    }

int main(){
    
    Init_timer() ;
    Initialize_UART() ;
    
//    TRISA = 0xFF ; // 0b11111111
    ADC_Initialize() ;
    
    TRISB = 0 ;
    PORTB = 0B00000000 ;
    
    TRISD = 0 ;
    PORTD = 0B00000000 ;
    
    RD0 = 0 ;   RD1 = 0 ;   RD2 = 0 ;   RD3 = 0 ;   RD4 = 0 ;

    Lcd_Start() ;
    
    Lcd_Clear() ;
    Lcd_Set_Cursor( 1, 1) ;
    Lcd_Print_String( "LPG Gas Detector") ;
    Lcd_Set_Cursor( 2, 1) ;
    Lcd_Print_String( "Ver-0.1");
    
    RD0 = 1 ;   RD1 = 1 ;   RD2 = 1 ;   RD3 = 1 ;
    __delay_ms( 1000) ;

    RD0 = 0 ;   RD1 = 0 ;   RD2 = 0 ;   RD3 = 0 ;
    __delay_ms( 1000) ;
    
//    UART_send_string("\n\r") ;
//    UART_send_string("\n\rAT\n\r") ;
//     __delay_ms( 1000) ;
//     UART_send_string("\n\rAT+CMGF=1\n\r ") ;
//     __delay_ms( 1000) ;
//     UART_send_string("\n\rAT+CMGS=\"+94711258542\"\n\r") ;
//     __delay_ms( 1000) ;
//     UART_send_string("\n\rHELLO\n\r ") ;
//     UART_send_char(26) ;
//     __delay_ms( 1000) ;
    
    initializer() ;

    while(1){

        blink() ;
        read_sensor() ;
        operate_buzzer() ;
        send_sms() ;

        }
    return 0 ;
    }
