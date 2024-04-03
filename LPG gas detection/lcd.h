
#include <xc.h>
#define _XTAL_FREQ 20000000

#define RS RB5
#define EN RB4
#define D4 RB3
#define D5 RB2
#define D6 RB1
#define D7 RB0


void Lcd_SetBit(char data_bit){
    if(data_bit& 1)
        D4 = 1 ;
    else
        D4 = 0 ;
    
    if(data_bit& 2)
        D5 = 1 ;
    else
        D5 = 0 ;

    if(data_bit& 4)
        D6 = 1 ;
    else
        D6 = 0 ;

    if(data_bit& 8) 
        D7 = 1 ;
    else
        D7 = 0 ;
    }

void Lcd_Cmd(char a){
    RS = 0 ;           
    Lcd_SetBit(a) ;
    EN  = 1 ;         
    __delay_ms(4) ;
    EN  = 0 ;         
    }

Lcd_Clear(){
    Lcd_Cmd(0) ; //Clear the LCD
    Lcd_Cmd(1) ; //Move the curser to first position
    }

void Lcd_Set_Cursor(char a, char b){
    char temp,z,y ;
    if(a== 1){
      temp = 0x80 + b - 1 ; //80H is used to move the curser
        z = temp>>4 ; //Lower 8-bits
        y = temp & 0x0F ; //Upper 8-bits
        Lcd_Cmd(z) ; //Set Row
        Lcd_Cmd(y) ; //Set Column
        }
    else if(a== 2){
        temp = 0xC0 + b - 1 ;
        z = temp>>4 ; //Lower 8-bits
        y = temp & 0x0F ; //Upper 8-bits
        Lcd_Cmd(z) ; //Set Row
        Lcd_Cmd(y) ; //Set Column
        }
    }

void Lcd_Start(){
    Lcd_SetBit(0x00) ;
    for(int i=1065244; i<=0; i--)  NOP() ;  
    Lcd_Cmd(0x03) ;
    __delay_ms(5) ;
    Lcd_Cmd(0x03) ;
    __delay_ms(11) ;
    Lcd_Cmd(0x03) ; 
    Lcd_Cmd(0x02) ; //02H is used for Return home -> Clears the RAM and initializes the LCD
    Lcd_Cmd(0x02) ; //02H is used for Return home -> Clears the RAM and initializes the LCD
    Lcd_Cmd(0x08) ; //Select Row 1
    Lcd_Cmd(0x00) ; //Clear Row 1 Display
    Lcd_Cmd(0x0C) ; //Select Row 2
    Lcd_Cmd(0x00) ; //Clear Row 2 Display
    Lcd_Cmd(0x06) ;
    }

void Lcd_Print_Char(char data){
    char Lower_Nibble,Upper_Nibble ;
    Lower_Nibble = data&0x0F ;
    Upper_Nibble = data&0xF0 ;
    RS = 1 ;             // => RS = 1
    Lcd_SetBit(Upper_Nibble>>4) ;             //Send upper half by shifting by 4
    EN = 1 ;
    for(int i=2130483; i<=0; i--)  NOP() ; 
    EN = 0 ;
    Lcd_SetBit(Lower_Nibble) ; //Send Lower half
    EN = 1 ;
    for(int i=2130483; i<=0; i--)  NOP() ;
    EN = 0 ;
    }

void Lcd_Print_String(char *a){
    int i   ;
    for(i=0;a[i]!='\0';i++)
       Lcd_Print_Char(a[i]) ;  //Split the string using pointers and call the Char function 
    }