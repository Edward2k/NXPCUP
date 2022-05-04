#ifndef _BOARD_FMUK66_H_
#define _BOARD_FMUK66_H_

#include <mbed.h>
#include "MK66F18.h"
#include <PwmOut.h>
#include <I2C.h>
#include "./mbed-os/targets/TARGET_Freescale/TARGET_MCUXpresso_MCUS/TARGET_K66F/drivers/fsl_rnga.h"
#include "FastPWM.h"


extern const double pw_mf;      // FIXME: Multiply Factor for pulsewidth. See pulsewidth_test() 

// Pinout, GPIO
#define UI_LED_BLUE_PIN         PTC8    // RGB LED
#define UI_LED_GREEN_PIN        PTC9    // RGB LED
#define UI_LED_RED_PIN          PTD1    // RGB LED
#define LED_AMBER_PIN           PTD13   // single LED
#define LED_GREEN_PIN           PTD14   // single LED
#define nSAFETY_LED_PIN         PTC0    // on GPS unit, inverted
#define SAFETY_SWITCH_PIN       PTE28   // on GPS unit
extern DigitalOut ui_led_blue;
extern DigitalOut ui_led_green;
extern DigitalOut ui_led_red;
extern DigitalOut led_amber;
extern DigitalOut led_green;
extern DigitalOut nsafety_led;
extern DigitalIn  safety_switch;

// Pinout, PWM
#define FMU_CH1_PIN             PTC1
#define FMU_CH2_PIN             PTA6
#define FMU_CH3_PIN             PTD4
#define FMU_CH4_PIN             PTD5
#define FMU_CH5_PIN             PTE11
#define FMU_CH6_PIN             PTE12
#define BUZZER_PIN              PTA11
extern FastPWM fmu_ch1;
extern FastPWM fmu_ch2;
extern FastPWM fmu_ch3;
extern FastPWM fmu_ch4;
extern FastPWM fmu_ch5;
extern FastPWM fmu_ch6;
//extern FastPWM buzzer;
extern PwmOut buzzer;
#define PWM_STEERING            fmu_ch2
#define PWM_THROTTLE            fmu_ch4


// Pinout, UARTs        FIXME: didn't get a single thing out of the UARTs (using RTT for debug console now)
#define DEBUG_TX_PIN            PTD9    // LPUART0
#define DEBUG_RX_PIN            PTD8    // LPUART0
#define IR_TX_PIN               PTA2    // UART0
#define IR_RX_PIN               PTA1    // UART0
#define RC_TX_PIN               PTC4    // UART1 (is S-BUS an async serial or totally different protocol?)
#define RC_RX_PIN               PTC3    // UART1
#define GPS_TX_PIN              PTD3    // UART2
#define GPS_RX_PIN              PTD2    // UART2
#define TELEMETRY_TX_PIN        PTC15   // UART4
#define TELEMETRY_RX_PIN        PTC14   // UART4
// extern BufferedSerial debug_uart;
// extern BufferedSerial ir_uart;
// //extern BufferedSerial rc_uart;
// extern BufferedSerial gps_uart;
// extern BufferedSerial telemetry_uart;
// // UART file handles for fprintf()
// extern FILE* debug_con_f;
// extern FILE* ir_con_f;
// extern FILE* gps_con_f;
// extern FILE* telemetry_con_f;

// Pinout, I2C
#define I2C_INTERNAL_SDA        PTC11
#define I2C_INTERNAL_SCL        PTC10
#define I2C0_EXTERNAL_SDA       PTE25
#define I2C0_EXTERNAL_SCL       PTE24
extern I2C i2c_internal;
extern I2C i2c_pixy;

// Proto's
void board_init(void);
void board_debug_intro(void);
void buzz(uint16_t freq, uint16_t duration);


#endif 
