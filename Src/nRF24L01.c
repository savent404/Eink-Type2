#include "nRF24L01.h"
/** Button func define */
static unsigned char ReadReg(unsigned char reg){
    unsigned char buff;
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(R_REGISTER(reg));
    buff = nRF24L01_opra_IO(0x00);//nop
    nRF24L01_CEN_opra(1);
    return buff;
}

static void ReadRegs(unsigned char loc, unsigned char *buf, int size){
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(R_REGISTER(loc));
    while (size--){
        *buf = nRF24L01_opra_IO(0x00);
        buf++;
    }
    nRF24L01_CEN_opra(1);
}

static void WriteReg(unsigned char loc, unsigned char val){
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(W_REGISTER(loc));
    nRF24L01_opra_IO(val);
    nRF24L01_CEN_opra(1);
}

static void WriteRegs(unsigned char loc, unsigned char *buf, int size){
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(W_REGISTER(loc));
    while (size--){
        nRF24L01_opra_IO(*buf);
        buf++;
    }
    nRF24L01_CEN_opra(1);
}

/** mid func define */
static void FIFO_Wsend(unsigned char *src, int cnt){
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(W_TX_PAYLOAD);
    while (cnt--){
        nRF24L01_opra_IO(*src);
        src++;
    }
    nRF24L01_CEN_opra(1);
}

static void FIFO_Rget(unsigned char *des, int cnt){
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(R_RX_PAYLOAD);
    while (cnt--){
        *des = nRF24L01_opra_IO(0x00);
        des++;
    }
    nRF24L01_CEN_opra(1);
}

static void FIFO_WAck(nRF24L01_AckStructure *apt){
    unsigned char *pt = apt->pSrc, cnt;
    
    // error para, exit.
    if (!apt)
        return;
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(W_ACK_PAYLOAD(apt->Px));
    cnt = apt->size;
    while (cnt--){
        nRF24L01_opra_IO(*pt);
        pt++;
    }
    nRF24L01_CEN_opra(1);
}
static void ModeExchang(nRF24L01_Mode mode){
    unsigned char buff;
    if (mode == STANDBY1){  /* when CE is set, default enter STANDBY2 mode(FIFO Tx empty)
                            or Tx mode (FIFO Tx is not empty) */
        nRF24L01_CE_opra(0);
        nRF24L01_CEN_opra(0);
        nRF24L01_opra_IO(FLUSH_RX);
        nRF24L01_CEN_opra(1);
        
        buff = ReadReg(0x00); //CONFIG reg
        buff &= 0xfe;         //PRIM_RX = 0;
        buff |= 0x02;         //PWR_UP  = 1;
        WriteReg(0x00, buff);
        nRF24L01_CE_opra(1);
    }
    else if (mode == TX){   /* do not FLUSH FIFO Tx, it will enter Tx Mode if FIFO Tx is not empty.
                            or it will into STANDBY2 mode */
        ModeExchang(STANDBY1);
    }
    else if (mode == RX){
        nRF24L01_CE_opra(0);
        buff = ReadReg(0x00); //CONFIG reg
        buff |= 0x01;         //PRIM_RX = 1;
        WriteReg(0x00, buff);
        nRF24L01_CE_opra(1);
    }
}

/** top func define */
void nRF24L01_Init(void){
    nRF24L01_CE_opra(0);
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(0x00);
    nRF24L01_CEN_opra(1);
    nRF24L01_CE_opra(1);
    
    nRF24L01_POWER(1);
    nRF24L01_RegInit();
}
void nRF24L01_RegInit(void){
    unsigned char buff;

    nRF24L01_CE_opra(0);
    //CONFIG     00H = 0x0A
    /*+++++++++++++++++++++ Usr define */
    buff = ReadReg(0x00);
    #if CONFIG_CRC_NUM
    buff |= 0x04;
    #else
    buff &= 0xFB;
    
    #endif

    #if CONFIG_MASK_IQR
    buff |= 0x70;
    #else
    buff &= 0x8F;
    #endif
    WriteReg(0x00, buff);

    //EN_AA     01H = 0x3F enabled all pipe's auto ack

    //EN_RXADDR 02H = 0x03 enabled Pipe0 and Pipe1
    /*+++++++++++++++++++++ Usr define */
    buff = ReadReg(0x02);
    #if CONFIG_ENABLE_P0
    buff |= 0x01;
    #else
    buff &= 0xFE;
    #endif
    #if CONFIG_ENABLE_P1
    buff |= 0x02;
    #else
    buff &= 0xFD;
    #endif
    #if CONFIG_ENABLE_P2
    buff |= 0x04;
    #else
    buff &= 0xFB;
    #endif
    #if CONFIG_ENABLE_P3
    buff |= 0x08;
    #else
    buff &= 0xF7;
    #endif
    #if CONFIG_ENABLE_P4
    buff |= 0x10;
    #else
    buff &= 0xEF;
    #endif
    #if CONFIG_ENABLE_P5
    buff |= 0x20;
    #else
    buff &= 0xDF;
    #endif
    WriteReg(0x02, buff);

    //SETUP_AW      03H = 0x03 RX/TX address is 5Bytes
    //SETUP_RETR    04H = 0x03 auto ack will retry 3 times,
    //and it will   delay 250us*3
    //RF_CH         05H = 0x02 default channel is 2400 + 2(MHz)999999999999999990
    //RF_SETUP      06H = 0x0e/0x0f //TX AP = 0dBm -- 11.3mA
                                    //250Kbps - speed data rate
    //+++++++++++++++++++++++ Set RF Tx Speed Rate */
    buff = ReadReg(0x06);
    #if   CONFIG_RF_DR == 0
    buff |= 0x08
    buff &= 0xDF;
    WriteReg(0x06, buff);
    #elif CONFIG_RF_DR == 1
    buff &= 0xF7;
    buff &= 0xDF;
    WriteReg(0x06, buff);
    #elif CONFIG_RF_DR == 2
    buff &= 0xF7;
    buff |= 0x20;
    WriteReg(0x06, buff);
    #endif
    //STATUS        07H = 0x0E  FIFO RX is empty, no pipe should be readed
    //OBSERVE_TX    08H = 0x00
    //RPD           09H = 0x00
    //RX_ADDR_P0    0AH = 0xE7E7E7E7E7 (5Bytes)
    //RX_ADDR_P1    0BH = 0xC2C2C2C2C2 (5Bytes)
    //RX_ADDR_P2    0CH = 0xC3
    //RX_ADDR_P3    0DH = 0xC4
    //RX_ADDR_P4    0EH = 0xC5
    //RX_ADDR_P5    0FH = 0xC6
    //TX_ADDR       10H = 0xE7E7E7E7E7 (5Bytes)

    //if used ShockBurst, not care RX_PW_Px
    //RX_PW_P0      11H = 0x00
    //RX_PW_P1      12H = 0x00
    //RX_PW_P2      13H = 0x00
    //RX_PW_P3      14H = 0x00
    //RX_PW_P4      15H = 0x00
    //RX_PW_P5      16H = 0x00

    //FIFO_STATUS   17H = 0x11  FIFO RX/TX is empty

    //DYNPD         1CH = 0x00
    //++++++++++++++Exchanged DYNPD: enable pipe P0~P1
    buff = ReadReg(0x1C);
    #if CONFIG_ENABLE_P0
    buff |= 0x01;
    #endif
    #if CONFIG_ENABLE_P1
    buff |= 0x02;
    #endif
    #if CONFIG_ENABLE_P2
    buff |= 0x04;
    #endif
    #if CONFIG_ENABLE_P3
    buff |= 0x08;
    #endif
    #if CONFIG_ENABLE_P4
    buff |= 0x10;
    #endif
    #if CONFIG_ENABLE_P5
    buff |= 0x20;
    #endif
    WriteReg(0x1C, buff);

    //FEATURE       1DH = 0x00
    //++++++++++++++Exchanged FRETURE: ENABLE Auto Ack\Dynamic Payload
    buff = ReadReg(0x1D);
    #if CONFIG_ENABLE_ACK_PAY
    buff |= 0x02;
    #endif
    buff |= 0x01;
    buff |= 0x04;
    WriteReg(0x1D, buff);
    
    nRF24L01_CE_opra(1);
}
void nRF24L01_POWER(unsigned char flag_1_on_0_off){
    unsigned char buff;
    nRF24L01_CE_opra(0);
    buff = ReadReg(0x00); //CONFIG reg
    buff |= flag_1_on_0_off << 1;
    WriteReg(0x00, buff);
    nRF24L01_CE_opra(1);
}
void nRF24L01_Channel_Init(unsigned char ch){
    if (ch > 127)
        return;
    nRF24L01_CE_opra(0);
    WriteReg(0x05, ch);
    nRF24L01_CE_opra(1);
}
void nRF24L01_RxInit(Pipex pipe, unsigned char *addr){
    nRF24L01_CE_opra(0);
    if (pipe == P0){
        WriteRegs(0x0A, addr, 5);
    }
    else if (pipe == P1){
        WriteRegs(0x0B, addr, 5);
    }
    else {
        WriteReg(0x0A + pipe, *addr);
    }
    nRF24L01_CE_opra(1);
}
void nRF24L01_TxInit(unsigned char *addr){
    nRF24L01_CE_opra(0);
    WriteRegs(0x10, addr, 5);
    nRF24L01_CE_opra(1);
}

FLAG nRF24L01_RxPack(nRF24L01_RxStructure *pt, nRF24L01_AckStructure *apt){
    unsigned char buff;
    nRF24L01_CE_opra(0);
    ModeExchang(STANDBY1);
    ModeExchang(RX);
    FIFO_WAck(apt);
    
    // waiting for IRQ, or exit with 'RESET'
    while (nRF24L01_IRQ_read() == 1){
//        Delay_ms(1);
//        if (!cnt--){
//            pt->Rxnum = 0;
//            return _RESET;
//        }
    }
    
    // get status and clear status register's flag
    buff = ReadReg(0x07);
    WriteReg(0x07, buff);
    
    // get RX pipe (0~5)
    if (buff & 0x40){
        pt->Px = (Pipex)((buff & 0x0E) >> 1);
    }
    else {
        return _RESET;
    }
    
    // get payload size
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(R_RX_PL_WID);
    pt->Rxnum = nRF24L01_opra_IO(0x00);
    nRF24L01_CEN_opra(1);
    
    // get payload
    if (pt->Rxnum <= 32){
        FIFO_Rget(pt->pRec, pt->Rxnum);
        buff = _SET;
    }
    else {
        buff = _RESET;
    }
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(FLUSH_RX);
    nRF24L01_CEN_opra(1);
    
    nRF24L01_CE_opra(1);
    return (FLAG)buff;
}
FLAG nRF24L01_TxPack(nRF24L01_TxStructure *pt){
    unsigned char buff;
    
    nRF24L01_CE_opra(0);
    
    // ready to write FIFO
    ModeExchang(STANDBY1);
    
    // write to FIFO
    FIFO_Wsend(pt->pSrc, pt->Txnum);
    
    // Trasmit
    ModeExchang(TX);
    
    while (nRF24L01_IRQ_read() == _SET){
//        Delay_ms(1);
//        if (!cnt--){
//            return _RESET;
//        }
    }
    
    // read and clear interrupt flag and fifo
    buff = ReadReg(0x07);
    WriteReg(0x07, buff);
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(FLUSH_TX);
    nRF24L01_CEN_opra(1);
    
    // Tx error, didn't get a ack
    if (buff & 0x10){ // flag MAX_RT
        // it should write RF_CH to clear MAX_RT bit
        return _RESET;
    }
    
    // If we have a Ack Payload
    if (buff & 0x40){
        nRF24L01_CEN_opra(0);
        nRF24L01_opra_IO(R_RX_PL_WID);
        pt->Rxnum = nRF24L01_opra_IO(0x00);
        nRF24L01_CEN_opra(1);
        
        if(pt->Rxnum > 32){
            nRF24L01_CEN_opra(0);
            nRF24L01_opra_IO(FLUSH_RX);
            nRF24L01_CEN_opra(1);
        }
        
        // read
        FIFO_Rget(pt->pRec, pt->Rxnum);
    }
    else {
        pt->Rxnum = 0;
    }
    
    // be carefull
    nRF24L01_CEN_opra(0);
    nRF24L01_opra_IO(FLUSH_RX);
    nRF24L01_CEN_opra(1);
    nRF24L01_CE_opra(1);
    
    // Final Check
    if (buff & 0x20){
        return _SET;
    }
    else {
        return _RESET;
    }
}
FLAG nRF24L01_Check(void){
    unsigned char buff;
    
    nRF24L01_CE_opra(0);
    buff = ReadReg(0x0F);
    WriteReg(0x0F, 0x55);
    if (ReadReg(0x0F) == 0x55){
        WriteReg(0x0f, buff);
        nRF24L01_CE_opra(1);
        return _SET;
    }
    else {
        nRF24L01_CE_opra(1);
        return _RESET;
    }

}

/** test func define */
void nRF24L01_ReadAllRegister(unsigned char *pt){
    int i = 0;

    nRF24L01_CE_opra(0);
    //for (i = 0; i < 0x09; i++){
    // Did I ignore the 0x09H (RPD) ?
    for (i = 0; i < 0x0A; i++){
        *pt = ReadReg(i);
        pt++;
    }
    ReadRegs(0x0a, pt, 5);
    pt += 5;
    ReadRegs(0x0b, pt, 5);
    pt += 5;
    
    for (i = 0; i < 0x04; i++){
        *pt = ReadReg(0x0C + i);
        pt++;
    }
    
    ReadRegs(0x10, pt, 5);
    pt += 5;

    for (i = 0; i < 0x07; i++){
        *pt = ReadReg(0x11 + i);
        pt++;
    }
    
    for (i = 0; i < 0x02; i++){
        *pt = ReadReg(0x1C + i);
        pt++;
    }

    nRF24L01_CE_opra(1);
}
