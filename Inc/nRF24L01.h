#ifndef _NRF24L01_H_
#define _NRF24L01_H_

/** Usr file include */

/** Usr func define */

/**
  * @Brief : Usr delay func.
  * @Para  : @arg ms
  */
extern void Delay_ms(int ms);

/**
  * @Brief : Usr opra func, ctl 'CEN' IO
  * @Para  : @arg set_1_reset_0 0->reset
                                1->set
  */
extern void nRF24L01_CEN_opra(unsigned char set_1_reset_0);

/**
  * @Brief : Usr opra func, ctl 'CE'  IO
  * @Para  : @arg set_1_reset_0 0->reset
                                1->set
  */
extern void nRF24L01_CE_opra(unsigned char set_1_reset_0);

/**
  * @Brief : Usr opra func, read 'IRQ' IO
  * @Retval: 1->SET
             0->RESET
  */
extern unsigned char nRF24L01_IRQ_read(void);

/**
  * @Brief : Usr opra func, SPI I/O opra.
  * @Para  : @arg val-write in.
  * @Retval: read out.
  */
extern unsigned char nRF24L01_opra_IO(unsigned char val);

/** Model Works selection */
#define CONFIG_CRC_NUM      1   // 0 -only one Byte CRC
                                // 1 -Or you can user tow Byte

#define CONFIG_MASK_IQR     0   // 0 -Function base on interrupt mode
                                // 1 -Search for STATUS without interrupt

// I would not suggest disable auto acknowledgement, and this register
//.. is can't be exchanged in this configuration Header.

#define CONFIG_RF_DR        1   // 0 -250 Kbps
                                // 1 -1 Mbps
                                // 2 -2 Mbps

// in this case, I suggest use the dynamic payload which can auto exchanged
// the size of payload

#define CONFIG_ENABLE_ACK_PAY   1   // 0    -disable
                                    // 1    -enable, you can return 3 Bytes max.

#define CONFIG_ENABLE_P0    1
#define CONFIG_ENABLE_P1    1
#define CONFIG_ENABLE_P2    1
#define CONFIG_ENABLE_P3    1
#define CONFIG_ENABLE_P4    1
#define CONFIG_ENABLE_P5    1

/** Usr Private define */
/*** nRF24L01 command ***/
#define R_REGISTER(x)       (x&0x1F)            /*Read command and status registers. AAAAA =
                                                5 bit Register Map Address*/
                                                
#define W_REGISTER(x)       ((x&0x1F)|0x20)     /*Write command and status registers. (x) = 5
                                                bit Register Map Address
                                                Executable in power down or standby modes
                                                only.*/
                                                
#define R_RX_PAYLOAD        (0x61)              /*Read RX-payload: 1-32 bytes. A read operation
                                                always starts at byte 0. Payload is deleted from
                                                FIFO after it is read. Used in RX mode.*/
                                                
#define W_TX_PAYLOAD        (0xA0)              /*Write TX-payload: 1-32 bytes. A write operation
                                                always starts at byte 0 used in TX payload.*/
#define FLUSH_TX            (0xE1)              /*Flush TX FIFO, used in TX mode*/

#define FLUSH_RX            (0xE2)              /*Flush RX FIFO, used in RX mode
                                                Should not be executed during transmission of
                                                acknowledge, that is, acknowledge package will
                                                not be completed.*/
                                                
#define REUSE_TX_PL         (0xE3)              /*Used for a PTX device
                                                Reuse last transmitted payload.
                                                TX payload reuse is active until
                                                W_TX_PAYLOAD or FLUSH TX is executed. TX
                                                payload reuse must not be activated or deactivated during package transmission.*/
                                                
#define R_RX_PL_WID         (0x60)              /*Read RX payload width for the top
                                                R_RX_PAYLOAD in the RX FIFO.
                                                Note: Flush RX FIFO if the read value is larger
                                                than 32 bytes.*/
                                                
#define W_ACK_PAYLOAD(x)    (0xA8|(x&0x07))     /*Used in RX mode.
                                                Write Payload to be transmitted together with
                                                ACK packet on PIPE (x). ((x) valid in the
                                                range from 000 to 101). Maximum three ACK
                                                packet payloads can be pending. Payloads with
                                                same (x) are handled using first in - first out
                                                principle. Write payload: 1-32 bytes. A write
                                                operation always starts at byte 0.*/
                                                
#define W_TX_PAYLOAD_NO     (0xB0)              /*Used in TX mode. Disables AUTOACK on this
                                                specific packet.*/
                                                
#define NOP                 (0xFF)              /*No Operation. Might be used to read the STATUS
                                                register*/

typedef enum {_RESET = 0, _SET} FLAG;

typedef enum {STANDBY1, RX, TX} nRF24L01_Mode;

typedef enum {P0 = 0, P1 = 1, P2 = 2, P3= 3, P4 = 4, P5 = 5} Pipex;

typedef struct {
    unsigned char *pRec;
    unsigned char Rxnum;
    Pipex	Px;
} nRF24L01_RxStructure;

typedef struct {
    unsigned char *pRec;
    unsigned char *pSrc;
    unsigned char Rxnum;
    unsigned char Txnum;
} nRF24L01_TxStructure;

typedef struct {
    unsigned char *pSrc;
    Pipex	Px;
    unsigned char size:2;
} nRF24L01_AckStructure;

/** Button func define */

/**
  * @Brief : read one byte from register.
  * @Para  : @arg reg->register addr
  * @Retval: byte readed.
  */
static unsigned char ReadReg(unsigned char reg);

/**
  * @Brief : read bytes from register.
  * @Para  : @arg loc->register addr
             @arg buf->readed data
             @arg size->size of data
  */
static void ReadRegs(unsigned char loc, unsigned char *buf, int size);

/**
  * @Brief : write one byte to register
  * @Para  : @arg loc->register addr
             @arg val->ready to write
  */
static void WriteReg(unsigned char loc, unsigned char val);

/**
  * @Brief : write bytes to register
  * @Para  : @arg loc->register addr
             @arg buf->ready to write
             @arg size->size of buf
  */
static void WriteRegs(unsigned char loc, unsigned char *buf, int size);

/** mid func define */

/**
  * @Brief : Write data to FIFO
  * @Para  : @arg src->ready to write
             @arg cnt->size of buf
  */
static void FIFO_Wsend(unsigned char *src, int cnt);

/**
  * @Brief : Read data from FIFO
  * @Para  : @arg des->readed data
             @arg cnt->size of data
  */
static void FIFO_Rget(unsigned char *des, int cnt);

/**
  * @Brief : Write ack pack to FIFO
  * @Para  : @arg apt->Ack Structure pointer
  */
static void FIFO_WAck(nRF24L01_AckStructure *apt);

/**
  * @Brief : Exchang mode (STANDBY | TX | RX)
  * @Para  : @arg mode STANDBY1
                       TX
                       RX
  */
static void ModeExchang(nRF24L01_Mode mode);

/** top func define */

/**
  * @Brief : Init nRF24L01 as #define func.
  */
void nRF24L01_Init(void);

/**
  * @Brief : Init register as #define func
  */
void nRF24L01_RegInit(void);

/**
  * @Brief : Write register to SET/RESET nRF24L01
  * @Para  : flag_1_on_0_off->as it's name
  */
void nRF24L01_POWER(unsigned char flag_1_on_0_off);

/**
  * @Brief : Exchang nRF24L01 channel, when MAX_RT flag is SET,
             it will RESET MAX_RT flag
  * @Para  : @arg ch->channel of nRF24L01 (0~127) Freq = 2400 + ch (MHz)
  */
void nRF24L01_Channel_Init(unsigned char ch);

/**
  * @Brief : Init Rx mode, using which pipe, and this pipe's address
  * @Para  : if we init P0/P1(Pipex) , address would be 5 Bytes
             or address if one byte.
  */
void nRF24L01_RxInit(Pipex pipe, unsigned char *addr);

/**
  * @Brief : Init Tx address
  * @Para  : addr has 5 bytes.
  */
void nRF24L01_TxInit(unsigned char *addr);

/**
  * @Brief : Rx pack. if should ack some bytes, use apt.
  */
FLAG nRF24L01_RxPack(nRF24L01_RxStructure *pt, nRF24L01_AckStructure *apt);

/**
  * @Brief : Tx pack. if recieve ack pack, num would not to be zero.
  */
FLAG nRF24L01_TxPack(nRF24L01_TxStructure *pt);

/**
  * @Brief : Check nRF24L01 exit.
  */
FLAG nRF24L01_Check(void);

/** test func define */
void nRF24L01_ReadAllRegister(unsigned char *pt);


#endif
