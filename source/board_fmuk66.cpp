#include "mbed.h"
#include "board_fmuk66.h"

const double pw_mf = 0.75;              // FIXME: Multiply Factor for pulsewidth. See pulsewidth_test() 

// GPIO
DigitalOut ui_led_blue(UI_LED_BLUE_PIN);
DigitalOut ui_led_green(UI_LED_GREEN_PIN);
DigitalOut ui_led_red(UI_LED_RED_PIN);
DigitalOut led_amber(LED_AMBER_PIN);
DigitalOut led_green(LED_GREEN_PIN);
DigitalOut nsafety_led(nSAFETY_LED_PIN);
DigitalIn  safety_switch(SAFETY_SWITCH_PIN);

// PWM
FastPWM fmu_ch1(FMU_CH1_PIN, 64);       // prescaler 64 works for a period of 20ms  FIXME: now it's too fast
FastPWM fmu_ch2(FMU_CH2_PIN, 64);
FastPWM fmu_ch3(FMU_CH3_PIN, 64);
FastPWM fmu_ch4(FMU_CH4_PIN, 64);
FastPWM fmu_ch5(FMU_CH5_PIN, 64);
FastPWM fmu_ch6(FMU_CH6_PIN, 64);
//FastPWM buzzer(BUZZER_PIN, 8);          // prescaler 8 for 1kHz? - 
PwmOut buzzer(BUZZER_PIN);

// Serial - FIXME: couldn't get any of them to work. Using RTT terminal instead
// BufferedSerial debug_uart(DEBUG_TX_PIN, DEBUG_RX_PIN, 115200);                // LPUART0
// BufferedSerial ir_uart(IR_TX_PIN, IR_RX_PIN, 115200);                         // UART0
// BufferedSerial rc_uart(RC_TX_PIN, RC_RX_PIN, 115200);                         // UART1 (?)
// BufferedSerial gps_uart(GPS_TX_PIN, GPS_RX_PIN, 115200);                      // UART2
// BufferedSerial telemetry_uart(TELEMETRY_TX_PIN, TELEMETRY_RX_PIN, 115200);    // UART4
// FILE* debug_con_f = fdopen(&debug_uart, "r+");
// FILE* ir_con_f = fdopen(&ir_uart, "r+");
// FILE* gps_con_f = fdopen(&gps_uart, "r+");
// FILE* telemetry_con_f = fdopen(&telemetry_uart, "r+");

// I2C
I2C i2c_internal(I2C_INTERNAL_SDA, I2C_INTERNAL_SCL);       // This one works
I2C i2c_pixy(PTE25, PTE24);                                 // FIXED: 0x80010130 pinmap not found for peripheral - added pins to PeripheralPins.c (see patch in root)

// Proto's
void board_init_gpio(void);
void board_init_pwm(void);
void board_init_serial(void);
void board_init_rng(void);
void board_init_i2c(void);
void board_init_rng(void);

// @brief   Board initialization
void board_init(void)
{
    board_init_gpio();
    board_init_pwm();
    board_init_serial();
    board_init_rng();           // Remember to setup peripheral clock or you'll Hardfault
    board_init_i2c();           // FIXME: mbed pinmap error
}

// @brief   Initialize GPIO pins
void board_init_gpio(void)
{
    ui_led_blue = 0;
    ui_led_green = 0;
    ui_led_red = 0;
    led_amber = 0;
    led_green = 0;
    nsafety_led = 1;
}

// @brief   Initialize PWM outputs
void board_init_pwm(void)
{
//    const uint16_t servo_min = 1000;                // Min
//    const uint16_t servo_mid = 1500;                // Mid (neutral) position in us
//    const uint16_t servo_max = 2000;                // Max

    fmu_ch1.period(0.02);                           // Period in sec.       0.02=20ms
    fmu_ch1.pulsewidth(0.0);

    fmu_ch2.period(0.02);
    fmu_ch2.pulsewidth(0.0);

    fmu_ch3.period(0.02);
    fmu_ch3.pulsewidth(0.0);

    fmu_ch4.period(0.02);
    fmu_ch4.pulsewidth(0.0);

    fmu_ch5.period(0.02);
    fmu_ch5.pulsewidth(0.0);

    fmu_ch6.period(0.02);
    fmu_ch6.pulsewidth(0.0);

    buzzer.period(0.001);                           // freq = 1kHz
    buzzer.write(0.0);                              // dutycycle = 0.0 (off)
}

// @brief   Initialize serial ports 
void board_init_serial(void)
{
    // Nothing to init here
}

void board_debug_intro(void)
{
    printf("* ==============================================================\n");
    printf("* %s - %s %s \n", __BASE_FILE__, __DATE__, __TIME__);
    printf("* Down the rabbit hole we go!!\n");
    printf("* kCLOCK_BusClk = %d\n", CLOCK_GetFreq(kCLOCK_BusClk));
    printf("* SystemCoreClock = %d\n", SystemCoreClock);
    printf("* Aaah! They are coming!!1^$#&.....\n");
    
}


// @brief   Initialize random number generator. We don't use the entropy register for now. 
//          Read from RNG->OR when RNG->SR->OREG_LVL is 1 (valid RNG number)
void board_init_rng(void)
{

    uint32_t regAddr = SIM_BASE + CLK_GATE_ABSTRACT_REG_OFFSET(kCLOCK_Rnga0);
    (*(volatile uint32_t *)regAddr) |= (1UL << CLK_GATE_ABSTRACT_BITS_SHIFT(kCLOCK_Rnga0));

    /* RNGA initialization */
    RNGA_Init(RNG);
    //RNGA_Init(((RNG_Type *)0x40029000U));
    /* Set user seed */
    RNGA_Seed(RNG, 0xcafeb0efUL);
    //RNGA_Seed(((RNG_Type *)0x40029000U), 0xcafeb0efUL);
}


void board_init_i2c(void)
{
    // Nothing to do
}

// @brief   Create a tone with the GPS buzzer
void buzz(uint16_t freq, uint16_t duration_ms)
{
    // Convert frequency to us period    
    buzzer.period_us(1000000U/freq);

    // Set dutycyle to 0.5 (starts buzzing)
    buzzer.write(0.5);

    // 
    thread_sleep_for(duration_ms);
    
    // Buzz off
    buzzer.write(0.0);
}