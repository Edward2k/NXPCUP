#include <mbed.h>
#include "fsl_i2c.h"
#include "pixy.h"
#include "board_fmuk66.h"

//
// NA - Welcome to printf() debugging - gdb causes Hardfaults when debugging while I2C is in progress
//

#define PIXY_ADDR ( 0x54<<1 )       // Mbed uses 8-bit addresses

// @brief       Initialize Pixy
void Pixy2::init()
{
    return;
}


// @brief       Run Pixy I2C test
void Pixy2::test()
{
    printf("* TONY THE PONY HE COMES!!!1\n");     // TO͇̹̺ͅƝ̴ȳ̳ TH̘Ë͖́̉ ͠P̯͍̭O̚​N̐Y̡ H̸̡̪̯ͨ͊̽̅̾̎Ȩ̬̩̾͛ͪ̈́̀́͘ ̶̧̨̱̹̭̯ͧ̾ͬC̷̙̲̝͖ͭ̏ͥͮ͟Oͮ͏̮̪̝͍M̲̖͊̒ͪͩͬ̚̚͜Ȇ̴̟̟͙̞ͩ͌͝S̨̥̫͎̭ͯ̿̔̀ͅ
    thread_sleep_for(2000);

    ui_led_green = 1;
    detect();
    ui_led_green = 0;
    buzz(2000, 25);
}


// @brief   Dump the Pixy hardware/software version data - See https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:porting_guide
void Pixy2::detect(void)
{
    uint8_t i, lenReceived, rxbuf[32]={0};
    uint8_t versionRequest[] = { 0xae, 0xc1, 0x0e, 0x00 };
  
    printf("* pixy_detect():\n");

    send(versionRequest, 4);   
    thread_sleep_for(10);
    lenReceived = recv(rxbuf, 6 + 16); // 6 bytes of header and checksum and 16 bytes of version data
    
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
uint8_t Pixy2::send(uint8_t *data, uint8_t len)
{
    int ack;
    uint8_t txn=0;      // nr of transmitted bytes
    uint8_t i;
    
    printf("* pixy_send():\n");
    printf("*    start - len=%d\n", len);
    // send START
    pixy_interface->start();

    // Send ADDRESS in WRITE mode and continue when ACK received
    printf("*    write addr=0x%02x (7b: 0x%02x)\n", PIXY_ADDR | kI2C_Write, PIXY_ADDR>>1);
    if ( pixy_interface->write(PIXY_ADDR | kI2C_Write)) 
    {
        printf("*    ack rcvd on addr\n");
        for (i=0; i<len; i++) {
            // printf("*    i=%2d, data=0x%02x, ", i, data[i]);
            if ( (pixy_interface->write(data[i]) )) {
                // printf("ACK\n");
                txn++;               
            } else {
                // Abort if no ACK received
                // printf("NAK\n");
                break;
            }
        }
    } else {
        printf("*    NO ack rcvd on addr\n");
    }
    pixy_interface->stop();

    printf("*    stop - len=%d, txn=%d\n", len, txn);
    printf("* end.\n");
    return txn;
}


// @brief   Receive data from Pixy
uint8_t Pixy2::recv(uint8_t *data, uint8_t len, uint16_t *cs)
{
    int ack;            // with write: 0=NAK, 1=ACK, 2=timeout
    uint8_t rxn=0;      // nr of received bytes
    uint8_t i;

    printf("* pixy_recv():\n");
    printf("*    start - len=%d\n", len);
    // send START
    pixy_interface->start();

    // send ADDRESS in READ mode and continue when ACK received
    printf("*    write addr=0x%02X (7b: 0x%02x)\n", PIXY_ADDR | kI2C_Read, PIXY_ADDR>>1);
    if (cs)
        *cs = 0;
    if ( pixy_interface->write(PIXY_ADDR | kI2C_Read)) 
    {
        printf("*    ack rcvd on addr\n");
        ack = 1;
        for (i=0; i<len; i++) {
            thread_sleep_for(1);
           if (i == (len-1) ) {
               ack = 0;
           }
            // printf("*    i=%2d, ack=%d, data=", i, ack);
            data[i] = pixy_interface->read(ack);     // 1 = acknowledge received data. Last read should have ACK=0
            // printf("0x%hhx\n", data[i]);
            rxn++;
            if (cs)
                *cs += data[i];
        }
    } else {
        printf("*    NO ack rcvd on addr\n");
    }

    // Send STOP
    pixy_interface->stop();

    printf("*    stop - len=%d, rxn=%d\n", len, rxn);
    printf("* end.\n");

    return rxn;
}

void Pixy2::prepare_msg(uint8_t type, uint8_t length, uint8_t *bufPayload) {
    this->m_length = length;
    this->m_type = type;
    this->m_bufPayload = bufPayload;
}

uint8_t Pixy2::send_msg() {
    uint8_t buf[m_maxlen];
    memset(buf, 0, m_maxlen);
    // write header info at beginnig of buffer
    printf("About to send msg");
    buf[0] = 0xae;//PIXY_CHECKSUM_SYNC & 0xff;
    buf[1] = 0xc1;//PIXY_CHECKSUM_SYNC >> 8;
    buf[2] = m_type;
    buf[3] = m_length;
    memcpy(buf + 4, m_bufPayload, m_maxlen-4);
    send(buf, m_length+4);
    return PIXY_RESULT_OK;
}

uint8_t Pixy2::recv_msg(uint8_t *data) {
  uint16_t csCalc, csSerial;
  int16_t res;
  
  res = sync();
  if (res<0)
    printf("SYNC FAILED!\n");

  if (m_cs)
  {
    printf("cs bby\n");
    res = recv(data, 4);
    if (res<0)
      return res;

    m_type = data[0];
    m_length = data[1];

    csSerial = *(uint16_t *)&data[2];

    res = recv(data, m_length, &csCalc);
    if (res<0)
      return res;

    if (csSerial!=csCalc)
    {
      printf("error: checksum (got : %d, expected %d)\n", csCalc, csSerial);
      return PIXY_RESULT_CHECKSUM_ERROR;
    }

  }
  else
  {   
    res = recv(data, 2);
    if (res<0)
      return res;

    m_type = data[0];
    m_length = data[1];

    res = recv(data, m_length);
    if (res<0)
      return res;
  }
  return PIXY_RESULT_OK;
}

int16_t Pixy2::sync()
{
  uint8_t i, j, c, cprev;
  int16_t res;
  uint16_t start;
  
  // parse bytes until we find sync
  for(i=j=0, cprev=0; true; i++)
  {
    res = recv(&c, 1);
    if (res>=PIXY_RESULT_OK)
    {
      // since we're using little endian, previous byte is least significant byte
      start = cprev;
      // current byte is most significant byte
      start |= c << 8;
      cprev = c;
      if (start==PIXY_CHECKSUM_SYNC)
      {
        m_cs = true;
        return PIXY_RESULT_OK;
      }
      if (start==PIXY_NO_CHECKSUM_SYNC)
      {
        m_cs = false;
        return PIXY_RESULT_OK;
      }
    }
	  // If we've read some bytes and no sync, then wait and try again.
	  // And do that several more times before we give up.  
	  // Pixy guarantees to respond within 100us.
    if (i>=4)
    {
      if (j>=4)
      {
#ifdef PIXY_DEBUG
        printf("error: no response\n");
#endif		  
        return PIXY_RESULT_ERROR;
      }
      thread_sleep_for(25); 
      j++;
      i = 0;
    }
  }
}
