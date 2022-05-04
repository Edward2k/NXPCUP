#ifndef _PIXY_H_
#define _PIXY_H_

#include <mbed.h>

extern I2C i2c_pixy;

extern uint8_t pixy_tx_buf[];
extern uint8_t pixy_rx_buf[];


extern uint8_t pixy_send(uint8_t *data, uint8_t len);
extern uint8_t pixy_recv(uint8_t *data, uint8_t len);

extern void pixy_detect();
extern void pixy_init(void);
extern void pixy_test(void);

#endif