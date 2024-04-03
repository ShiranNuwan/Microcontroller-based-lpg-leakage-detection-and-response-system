
#define Baud_rate 57600
#define _XTAL_FREQ 20000000

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

void UART_send_char(char bt){
    while(!TXIF) ;
    TXREG = bt ; 
    }

char UART_get_char(){
//    if(OERR){
//        CREN = 0 ;
//        CREN = 1 ;
//        } 
    while(!RCIF) ; 
    return RCREG ;
    }

void UART_Read_Text(char *Output, unsigned int length){
    unsigned int i;
    for(int i=0;i<length;i++)
    Output[i] = UART_get_char();
    }

char UART_Data_Ready(){
    return RCIF ;
    }

void UART_send_string(char* st_pt){
    while(*st_pt) 
        UART_send_char(*st_pt++) ;
    }

void serialwrite(char ch){
    while(TXIF==0);  // Wait till the transmitter register becomes empty
    TXIF=0;          // Clear transmitter flag
    TXREG=ch;        // load the char to be transmitted into transmit reg
    }

serialprint(char *str){
    while(*str){
        serialwrite(*str++);
        }
    }

