

#include <mbed.h>
#include "fsl_i2c.h"
#include "pixy.h"
#include "board_fmuk66.h"

//
// NA - Welcome to printf() debugging - gdb causes Hardfaults when debugging while I2C is in progress
//

const int pixy_address = 0x54<<1;       // Mbed uses 8-bit addresses
uint8_t pixy_tx_buf[64] = {0};
uint8_t pixy_rx_buf[64] = {0};


// @brief       Initialize Pixy
void pixy_init(void)
{
    uint8_t i;

    // TODO
}


// @brief       Run Pixy I2C test
void pixy_test(void)
{
    printf("* TONY THE PONY HE COMES!!!1\n");     // TO͇̹̺ͅƝ̴ȳ̳ TH̘Ë͖́̉ ͠P̯͍̭O̚​N̐Y̡ H̸̡̪̯ͨ͊̽̅̾̎Ȩ̬̩̾͛ͪ̈́̀́͘ ̶̧̨̱̹̭̯ͧ̾ͬC̷̙̲̝͖ͭ̏ͥͮ͟Oͮ͏̮̪̝͍M̲̖͊̒ͪͩͬ̚̚͜Ȇ̴̟̟͙̞ͩ͌͝S̨̥̫͎̭ͯ̿̔̀ͅ
    thread_sleep_for(2000);

    ui_led_green = 1;
    pixy_detect();
    ui_led_green = 0;
    buzz(2000, 25);
}


// @brief   Dump the Pixy hardware/software version data - See https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:porting_guide
void pixy_detect(void)
{
    uint8_t i, lenReceived, rxbuf[32]={0};
    uint8_t versionRequest[] = { 0xae, 0xc1, 0x0e, 0x00 };
  
    // clear out any stale data
//    while(pixy_recv(rxbuf, 1));       // FIXME: hangs here

    printf("* pixy_detect():\n");

    pixy_send(versionRequest, 4);   
    thread_sleep_for(10);
    lenReceived = pixy_recv(rxbuf, 6 + 16); // 6 bytes of header and checksum and 16 bytes of version data
    
    printf("\n");
    thread_sleep_for(100);

    // print result
    printf("*    Received %d bytes: ", lenReceived);
    for (i=0; i<lenReceived; i++) {
        printf("%02x  ", rxbuf[i]); 
    }
    printf("\n");

    printf("*    checksum sync        : %02x %02x\n", rxbuf[0], rxbuf[1] );
    printf("*    version response type: %02x\n", rxbuf[2] );
    printf("*    data length          : %02x\n", rxbuf[3] );
    printf("*    data checksum        : %02x %02x\n", rxbuf[4], rxbuf[5] );
    printf("*    hardware version     : %02x %02x\n", rxbuf[6], rxbuf[7] );
    printf("*    firmware version     : %02x %02x\n", rxbuf[8], rxbuf[9] );
    printf("*    firmware build nr    : %02x %02x\n", rxbuf[10], rxbuf[11] );
    printf("*    firmware type ascii  : %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X - \"%s\"\n", rxbuf[12], rxbuf[13], rxbuf[14], rxbuf[15], rxbuf[16], rxbuf[17], rxbuf[18], rxbuf[19], rxbuf[20], rxbuf[21], &rxbuf[12] );
    printf("* end.\n");
}


// @brief       Send data to Pixy
uint8_t pixy_send(uint8_t *data, uint8_t len)
{
    int ack;
    uint8_t txn=0;      // nr of transmitted bytes
    uint8_t i;
    
    printf("* pixy_send():\n");
    printf("*    start - len=%d\n", len);
    // send START
    i2c_pixy.start();

    // Send ADDRESS in WRITE mode and continue when ACK received
    printf("*    write addr=0x%02x (7b: 0x%02x)\n", pixy_address | kI2C_Write, pixy_address>>1);
    if ( i2c_pixy.write(pixy_address | kI2C_Write)) 
    {
        printf("*    ack rcvd on addr\n");
        for (i=0; i<len; i++) {
            printf("*    i=%2d, data=0x%02x, ", i, data[i]);
            if ( (i2c_pixy.write(data[i]) )) {
                printf("ACK\n");
                txn++;               
            } else {
                // Abort if no ACK received
                printf("NAK\n");
                break;
            }
        }
    } else {
        printf("*    NO ack rcvd on addr\n");
    }
    i2c_pixy.stop();

    printf("*    stop - len=%d, txn=%d\n", len, txn);
    printf("* end.\n");
    return txn;
}


// @brief   Receive data from Pixy
uint8_t pixy_recv(uint8_t *data, uint8_t len)
{
    int ack;            // with write: 0=NAK, 1=ACK, 2=timeout
    uint8_t rxn=0;      // nr of received bytes
    uint8_t i;

    printf("* pixy_recv():\n");
    printf("*    start - len=%d\n", len);
    // send START
    i2c_pixy.start();

    // send ADDRESS in READ mode and continue when ACK received
    printf("*    write addr=0x%02X (7b: 0x%02x)\n", pixy_address | kI2C_Read, pixy_address>>1);
    if ( i2c_pixy.write(pixy_address | kI2C_Read)) 
    {
        printf("*    ack rcvd on addr\n");
        ack = 1;
        for (i=0; i<len; i++) {
            thread_sleep_for(1);
           if (i == (len-1) ) {
               ack = 0;
           }
            printf("*    i=%2d, ack=%d, data=", i, ack);
            data[i] = i2c_pixy.read(ack);     // 1 = acknowledge received data. Last read should have ACK=0
            printf("0x%02X\n", data[i]);
            rxn++;
        }
    } else {
        printf("*    NO ack rcvd on addr\n");
    }

    // Send STOP
    i2c_pixy.stop();

    printf("*    stop - len=%d, rxn=%d\n", len, rxn);
    printf("* end.\n");

    return rxn;
}
