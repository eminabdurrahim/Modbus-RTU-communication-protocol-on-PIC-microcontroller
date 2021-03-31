/*
 * File:   newmain.c
 * Author: Abdurrahim Emin
 *
 * Created on 07 Aral?k 2020 Pazartesi, 09:24
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pic18f45k22.h>
#include <htc.h>
#define _XTAL_FREQ 8000000
#pragma config FOSC = HSHP
#pragma config WDTEN = OFF
#define TX_EN PORTCbits.RC5
unsigned adr = 7;
int rcv_cnt = 0;
char rcv_buf[45];
bit rcv_flag = 0;
int rcv_len = 0;
int tx_cnt = 0;
char tx_buf[70];
bit tx_flag = 0;
int tx_len = 0;
char v1 = 0;
int v2 = 0;
int v3 = 0;
int v4 = 0;
int v5;
uint16_t x;
char var2;
char v7[70];
int timeout_cnt = 0;
uint16_t CRC16(const uint8_t *nData, uint16_t wLength);
void I2C_Init();
char I2C_Start(char slave_write_address);
void I2C_Ready();
char I2C_Write(unsigned char data);
char I2C_Stop();
char I2C_Read(char flag);
void I2C_Ack();
void I2C_Nack();

int main(int argc, char** argv) {


    ANSELC = 0;
    TRISC = 0x9F; //1001 1111
    PORTC = 0x00;

    BAUDCONbits.BRG16 = 0;
    TXSTAbits.BRGH = 0;
    SPBRG = 12;

    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;

    RCIE = 1;
    RCIP = 1;

    T0CON = 0xC3;
    TMR0IE = 1;

    PEIE = 1;
    GIE = 1;

    while (1) {


        if (rcv_flag) {
            if (rcv_buf[1] == 0x03) {
                if ((rcv_buf[2] == 0x00 & rcv_buf[3] > 0x40) || (rcv_buf[5] > 0x40)) {
                    tx_buf[0] = 0x07;
                    tx_buf[1] = 0x81;
                    tx_buf[2] = 0x02;
                    x = CRC16(tx_buf, 3);
                    tx_buf[3] = x;
                    tx_buf[4] = (0xFF00 & x) >> 8;
                    tx_len = 5;
                } else {
                    tx_buf[0] = rcv_buf[0];
                    tx_buf[1] = rcv_buf[1];
                    v5 = rcv_buf[5]*2;
                    tx_buf[2] = v5;
                    GIE = 0;
                    I2C_Init();
                    for (int i = rcv_buf[3]; i < v5; i++) {

                        I2C_Start(0xA0);
                        I2C_Write(i);
                        I2C_Stop();
                        I2C_Start(0xA1);
                        v7[i] = I2C_Read(1);
                        I2C_Stop();
                    }
                    GIE = 1;
                    for (int q = rcv_buf[3]; q < v5; q++) {
                        tx_buf[3 + q] = v7[q];
                    }
                    x = CRC16(tx_buf, v5 + 3);
                    tx_buf[v5 + 3] = x & (0x00FF);
                    tx_buf[v5 + 4] = (0xFF00 & x) >> 8;
                    tx_len = v5 + 5;
                }

            } else if (rcv_buf[1] == 0x06) {
                if (rcv_buf[2] == 0x00 & rcv_buf[3] > 0x40) {
                    tx_buf[0] = 0x07;
                    tx_buf[1] = 0x81;
                    tx_buf[2] = 0x02;
                    x = CRC16(tx_buf, 3);
                    tx_buf[3] = x;
                    tx_buf[4] = (0xFF00 & x) >> 8;
                    tx_len = 5;

                } else {
                    tx_buf[0] = rcv_buf[0];
                    tx_buf[1] = rcv_buf[1];
                    v1 = rcv_buf[3]*2 + 1;
                    GIE = 0;
                    I2C_Init();
                    I2C_Start(0xA0);
                    I2C_Write(v1 - 1);
                    I2C_Write(rcv_buf[4]);
                    I2C_Stop();
                    __delay_ms(90);
                    __delay_ms(90);
                    I2C_Start(0xA0);
                    I2C_Write(v1);
                    I2C_Write(rcv_buf[5]);
                    I2C_Stop();
                    GIE = 1;
                    tx_buf[2] = rcv_buf[2];
                    tx_buf[3] = rcv_buf[3];
                    tx_buf[4] = rcv_buf[4];
                    tx_buf[5] = rcv_buf[5];
                    x = CRC16(tx_buf, 6);
                    tx_buf[6] = x;
                    tx_buf[7] = (0xFF00 & x) >> 8;
                    tx_len = 8;
                }
            } else if (rcv_buf[1] == 0x10) {
                if ((rcv_buf[2] == 0x00 & rcv_buf[3] > 0x40) || (rcv_buf[5] > 0x40)) {
                    tx_buf[0] = 0x07;
                    tx_buf[1] = 0x81;
                    tx_buf[2] = 0x02;
                    x = CRC16(tx_buf, 3);
                    tx_buf[3] = x;
                    tx_buf[4] = (0xFF00 & x) >> 8;
                    tx_len = 5;
                } else {
                    tx_buf[0] = rcv_buf[0];
                    tx_buf[1] = rcv_buf[1];
                    tx_buf[2] = rcv_buf[2];
                    tx_buf[3] = rcv_buf[3];
                    tx_buf[4] = rcv_buf[4];
                    tx_buf[5] = rcv_buf[5];
                    x = CRC16(tx_buf, 6);
                    tx_buf[6] = x;
                    tx_buf[7] = (0xFF00 & x) >> 8;
                    GIE = 0;
                    I2C_Init();
                    for (int i = rcv_buf[3]; i < rcv_buf[5]; i++) {
                        v1 = i * 2 + 1;
                        I2C_Start(0xA0);
                        I2C_Write(v1 - 1);
                        I2C_Write(rcv_buf[7]);
                        I2C_Stop();
                        __delay_ms(90);
                        __delay_ms(90);
                        I2C_Start(0xA0);
                        I2C_Write(v1);
                        I2C_Write(rcv_buf[8]);
                        I2C_Stop();
                        __delay_ms(90);
                        __delay_ms(90);
                    }
                    GIE = 1;
                    tx_len = 8;
                }
            } else {
                tx_buf[0] = 0x07;
                tx_buf[1] = 0x81;
                tx_buf[2] = 0x01;
                x = CRC16(tx_buf, 3);
                tx_buf[3] = x;
                tx_buf[4] = (0xFF00 & x) >> 8;
                tx_len = 5;
            }


            tx_flag = 1;
            rcv_flag = 0;
        }
    }



    return (EXIT_SUCCESS);
}

void high_priority interrupt isr() {

    if (RC1IF) {
        rcv_buf[rcv_cnt] = RCREG;
        rcv_cnt++;
        timeout_cnt = 0;
    }

    if (TMR0IF) {
        TMR0IF = 0;
        TMR0 = 128;

        if (RCSTAbits.FERR || RCSTAbits.OERR) {
            RCSTAbits.SPEN = 0;
            TXSTAbits.TXEN = 1;
            RCSTAbits.CREN = 1;
            TXSTAbits.SYNC = 0;
            RCSTAbits.SPEN = 1;
        }


        timeout_cnt++;
        if (timeout_cnt >= 10) {
            if (rcv_cnt > 7) {
                if (rcv_buf[0] == adr) {
                    rcv_flag = 1;
                    rcv_len = rcv_cnt;
                }
            }
            rcv_cnt = 0;
            timeout_cnt = 0;
        }
        if (!tx_flag) {
            TX_EN = 0;
        }


        if (tx_flag && TXSTAbits.TRMT) {
            TX_EN = 1;
            if (tx_cnt < tx_len) {
                TXREG = tx_buf[tx_cnt];
                tx_cnt++;
            } else {
                tx_cnt = 0;
                tx_len = 0;
                TX_EN = 0;
                tx_flag = 0;
            }
        }

    }
}

uint16_t CRC16(const uint8_t *nData, uint16_t wLength) {
    static const uint16_t wCRCTable[] = {
        0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
        0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
        0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
        0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
        0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
        0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
        0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
        0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
        0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
        0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
        0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
        0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
        0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
        0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
        0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
        0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
        0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
        0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
        0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
        0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
        0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
        0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
        0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
        0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
        0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
        0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
        0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
        0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
        0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
        0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
        0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
        0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
    };

    uint8_t nTemp;
    uint16_t wCRCWord = 0xFFFF;

    while (wLength--) {
        nTemp = *nData++ ^ wCRCWord;
        wCRCWord >>= 8;
        wCRCWord ^= wCRCTable[nTemp];
    }
    return wCRCWord;

}

void I2C_Init() {
    ANSELC = 0;
    TRISC3 = 1; /* Set up I2C lines by setting as input */
    TRISC4 = 1;

    SSPSTAT = 0x80; /* Slew rate disabled, other bits are cleared */
    SSPCON1 = 0x28; /* Enable SSP port for I2C Master mode,
			clock = FOSC / (4 * (SSPADD+1))*/
    SSPCON2 = 0;
    SSPADD = 19; /* Clock 100 kHz */
    SSPIE = 1; /* Enable SSPIF interrupt */
    SSPIF = 0;
}

char I2C_Start(char slave_write_address) {
    SSPCON2bits.SEN = 1; /* Send start pulse */
    while (SSPCON2bits.SEN); /* Wait for completion of start pulse */
    SSPIF = 0;
    if (!SSPSTATbits.S) /* Check whether START detected last */
        return 0; /* Return 0 to indicate start failed */
    return (I2C_Write(slave_write_address)); /* Write slave device address
						with write to communicate */
}

void I2C_Ready() {
    while (BCLIF); /* Wait if bit collision interrupt flag is set*/

    /* Wait for Buffer full and read write flag*/
    while (SSPSTATbits.BF || (SSPSTATbits.R_nW));
    SSPIF = 0; /* Clear SSPIF interrupt flag*/
}

char I2C_Write(unsigned char data) {
    SSPBUF = data; /* Write data to SSPBUF*/
    I2C_Ready();
    if (SSPCON2bits.ACKSTAT) /* Check for acknowledge bit*/
        return 1;
    else
        return 2;
}

char I2C_Stop() {
    I2C_Ready();
    SSPCON2bits.PEN = 1; /* Stop communication*/
    while (SSPCON2bits.PEN); /* Wait for end of stop pulse*/
    SSPIF = 0;
    if (!SSPSTATbits.P) /* Check whether STOP is detected last */
        return 0; /* If not return 0 to indicate start failed*/
    return 1;
}

char I2C_Read(char flag) {
    /* Read data from location and send ack or nack depending upon parameter*/

    int buffer = 0;
    SSPCON2bits.RCEN = 1; /* Enable receive */

    /* Wait for buffer full flag which when complete byte received */
    while (!SSPSTATbits.BF);
    buffer = SSPBUF; /* Copy SSPBUF to buffer */

    /* Send acknowledgment or negative acknowledgment after read to
    continue or stop reading */
    if (flag == 0)
        I2C_Ack();
    else
        I2C_Nack();
    I2C_Ready();
    return (buffer);
}

void I2C_Ack() {
    SSPCON2bits.ACKDT = 0; /* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN = 1; /* Enable ACK to send */
    while (SSPCON2bits.ACKEN);
}

void I2C_Nack() {
    SSPCON2bits.ACKDT = 1; /* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN = 1; /* Enable ACK to send */
    while (SSPCON2bits.ACKEN);
}
