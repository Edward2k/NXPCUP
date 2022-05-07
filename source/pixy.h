#ifndef _PIXY_H_
#define _PIXY_H_

#include <mbed.h>
#include "board_fmuk66.h"


#define PIXY_NO_CHECKSUM_SYNC  0xc1ae

extern I2C i2c_pixy;

class Pixy2 {
public: 
    Pixy2() {pixy_interface->frequency(400000);}
    void detect();
    void init();
    void test();
    void prepare_msg(uint8_t type, uint8_t length, uint8_t *bufPayload);
    uint8_t send_msg();

private:
    I2C *pixy_interface = &i2c_pixy;
    uint8_t send(uint8_t *data, uint8_t len);
    uint8_t recv(uint8_t *data, uint8_t len);

    uint8_t m_maxlen = 16;
    uint8_t *m_bufPayload;
    uint8_t m_type;
    uint8_t m_length;
        
};


#endif