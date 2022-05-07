#ifndef _PIXY_H_
#define _PIXY_H_

#include <mbed.h>
#include "board_fmuk66.h"


#define PIXY_NO_CHECKSUM_SYNC  0xc1ae
#define PIXY_CHECKSUM_SYNC     0xc1af

extern I2C i2c_pixy;

/**
* Simple codes
**/

#define PIXY_RESULT_OK 0
#define PIXY_RESULT_ERROR -1
#define PIXY_RESULT_BUSY -2
#define PIXY_RESULT_RESPONSE_ERROR -3
#define PIXY_RESULT_TIMEOUT -4
#define PIXY_RESULT_BUTTON_OVERRIDE -5
#define PIXY_RESULT_PROG_CHANGING -6


class Pixy2 {
public: 
    Pixy2() {pixy_interface->frequency(400000);}
    void detect();
    void init();
    void test();
    void prepare_msg(uint8_t type, uint8_t length, uint8_t *bufPayload);
    uint8_t send_msg();
    uint8_t recv_msg(uint8_t *recvbuffer); /* This buffer is NOT malloc'd for you */
    int8_t get_type() {return this->m_type;}
    uint8_t get_len() {return this->m_length;}

private:
    I2C *pixy_interface = &i2c_pixy;
    uint8_t send(uint8_t *data, uint8_t len);
    uint8_t recv(uint8_t *data, uint8_t len);

    uint8_t m_maxlen = 128;
    uint8_t *m_bufPayload;
    int8_t m_type;
    uint8_t m_length;
        
};


#endif