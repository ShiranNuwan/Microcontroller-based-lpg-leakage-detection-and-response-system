
#define Baud_rate 9600
#define _XTAL_FREQ 16000000

void Initialize_UART(void){
    TRISC6 = 0 ; 
    TRISC7 = 1 ;
    
    TX9   = 0 ;
    TXEN  = 1 ; 
    SYNC  = 0 ;
    BRGH  = 1 ;

    SPEN  = 1 ;
    RX9   = 0 ;
    CREN  = 1 ;
    
    SPBRG = ((_XTAL_FREQ/16)/Baud_rate) - 1 ;

    }

char UART_get_char(){
    if(OERR){
        CREN = 0 ;
        CREN = 1 ;
        } 
    while(!RCIF) ; 
    return RCREG ;
    }

void UART_send_char(char bt){
    while(!TXIF) ;
    TXREG = bt ; 
    }

void UART_send_string(char* st_pt){
    while(*st_pt) 
        UART_send_char(*st_pt++) ;
    }





