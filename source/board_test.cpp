#include <mbed.h>
#include "board_fmuk66.h"
#include "board_test.h"

double get_pwm(double value, uint16_t center, uint16_t differ) {
    if (value < -1.0) value = -1.0;
    if (value >  1.0) value =  1.0;

    double pwm_value = center + value * differ;
    printf("pwm_value: %f\n", pwm_value);
    return pwm_value;
}

void set_servo(double servo) {
    double servo_pwm = get_pwm(servo, 2025, 325);
    fmu_ch2.pulsewidth(servo_pwm/1000000U);
    
    // CAUTION: When using PwmOut() a delay of 1ms caused erratic servo behaviour which destroyed my plastic-gear servo - See comment on top of board_fmuk66.h about Mbed and PWM. 
    //          DO NOT SET LOWER THAN, say, 10, when using PwmOut() instead of FastPWM.
    const uint16_t sleep_delay = 10;
    thread_sleep_for(sleep_delay);
}

void set_speed(double speed) {
    double speed_pwm = get_pwm(speed, 1600, 400);
    fmu_ch4.pulsewidth(speed_pwm/1000000U);
    
    const uint16_t sleep_delay = 10;
    thread_sleep_for(sleep_delay);
}

// FIXME: change to FastPWM (see throttle_test on how to do that. Needs a scope to verify correct servo timing)
void servo_test(void)
{
    printf("* servo_test(): \n");
    fmu_ch2.period(0.02);
    set_servo(0.0); // center servo
    
    uint16_t loop;  
    double pw;       

    // Go slowly
    printf("*    Slow:\n");
    for (loop=0; loop<2; loop++) {
        printf("*    C -> R\n");
        for (pw=0.0; pw<=1.0; pw+=0.002) {
            set_servo(pw);
        }
        printf("*    R -> L\n");
        for (; pw>=-1.0; pw-=0.002) {
            set_servo(pw);
        }
        printf("*    L -> C\n");
        for (; pw<=0.0; pw+=0.002) {
            set_servo(pw);
        }
    }

    // Full servo speed
    printf("*    Hard:\n");
    for (loop=0; loop<4; loop++) {
        printf("*    L\n");
        set_servo(-1.0);
        thread_sleep_for(1000);
        printf("*    R\n");
        set_servo( 1.0);
        thread_sleep_for(1000);
    }

    // Back to center
    printf("*    C\n");
    set_servo( 0.0);

    printf("\n* end.\n");
}

// @brief   Test throttle PWM signal
// FIXME: no reverse? -> probably caused by the buggy mbed PWM which caused the ESC to reprogram
void throttle_test(void)
{
    printf("* throttle_test():\n");
    // FastPWM fmu_ch4(FMU_CH4_PIN, 64); // prescaler 64 works for a period of 20ms
    fmu_ch4.period(0.02);

    // Start with brake mode (ESC should stop beeping since there is a servo signal present)
    set_speed(0.0);

    // Give the ESC some time to play it's startup tune
    thread_sleep_for(4000);
    
    uint16_t loop;  

    // Warning LED
    for(loop=0; loop<20; loop++) {
        ui_led_red = 0;
        thread_sleep_for(100);
        ui_led_red = 1;
        thread_sleep_for(100);
    }

    // WARNING: Motor will run in this loop!
    for (loop=0; loop<4; loop++) {
        printf("*    half speed = 0.5\n");
        set_speed(0.5);
        thread_sleep_for(5000);

        printf("*    brake = 0.0\n");
        set_speed(0.0);
        thread_sleep_for(5000);

        printf("*    half reverse = -0.5\n");
        set_speed(-0.5);
        thread_sleep_for(5000);
    }

    // End with brake mode
    set_speed(0.0);
    thread_sleep_for(1000);
    printf("* end.\n");
    ui_led_red = 0;
    thread_sleep_for(1000);
}



// @brief       Manually test the PWM timings with a scope. Turns out everything is 0.75x off.
void pulsewidth_test(void)
{
    const uint16_t esc_min = 1000;                // Min
    const uint16_t esc_mid = 1500;                // Mid (neutral) position in us
    const uint16_t esc_max = 2000;                // Max

    // 1500 us turned out to be 1130 us on the scope
    // Calculate a Multiply Factor 
    double mf = 0.75;

    printf("* pulsewidth_test():\n");

    /* period   actual      (With prescale=64)
    *  0.028    0.0210
    *  0.027    0.0203
    *  0.0266   0.0200     <------- 0.0200 / 0.75 = 0.0266
    *  0.0265   0.0199
    *  0.026    0.0195
    *  0.025    0.0188
    *  0.020    0.0150
    *  0.010    0.0075
    */
    fmu_ch4.period(0.02 / mf);

    /* width(us)    actual  (With prescale=64)
    *  1340         1000    <------
    *  1360         1020
    *  1400         1040
    *  1500         1120   
    *  2000         1500    <------  1500 / 0.75 = 2000
    *  2500         1880
    *  2680         2000    <------
    *  2700         2020
    */
    fmu_ch4.pulsewidth((double)1500 / 1000000U * mf);

    while(1);
}
