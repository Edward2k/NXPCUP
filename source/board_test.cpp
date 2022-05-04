#include <mbed.h>
#include "board_fmuk66.h"
#include "board_test.h"


// FIXME: change to FastPWM (see throttle_test on how to do that. Needs a scope to verify correct servo timing)
void servo_test(void)
{
    uint16_t pw;
    uint16_t loop;

    printf("* servo_test(): \n");

    const uint16_t servo_min = 1250;
    const uint16_t servo_center = 1500;
    const uint16_t servo_max = 1750;
    const uint8_t  step_size = 1;

    // CAUTION: When using PwmOut() a delay of 1ms caused erratic servo behaviour which destroyed my plastic-gear servo - See comment on top of board_fmuk66.h about Mbed and PWM. 
    //          DO NOT SET LOWER THAN, say, 10, when using PwmOut() instead of FastPWM.
    const uint16_t delay = 5;           

    fmu_ch2.period(0.02);
    fmu_ch2.pulsewidth((double)servo_center/1000000U * pw_mf);

    // Go slowly
    printf("*    Slow:\n");
    for (loop=0; loop<2; loop++) {
        printf("*    C -> R\n");
        for (pw=servo_center; pw<=servo_max; pw+=step_size) {
            fmu_ch2.pulsewidth((double)pw/1000000U * pw_mf);
            thread_sleep_for(delay);
        }
        printf("*    R -> L\n");
        for (; pw>=servo_min; pw-=step_size) {
            fmu_ch2.pulsewidth((double)pw/1000000U * pw_mf);
            thread_sleep_for(delay);
        }
        printf("*    L -> C\n");
        for (; pw<=servo_center; pw+=step_size) {
            fmu_ch2.pulsewidth((double)pw/1000000U * pw_mf);
            thread_sleep_for(delay);
        }
    }

    // Full servo speed
    printf("*    Hard:\n");
    for (loop=0; loop<4; loop++) {
        printf("*    L\n");
        fmu_ch2.pulsewidth((double)servo_min/1000000U * pw_mf);
        thread_sleep_for(1000);
        printf("*    R\n");
        fmu_ch2.pulsewidth((double)servo_max/1000000U * pw_mf);
        thread_sleep_for(1000);
    }

    // Back to center
    printf("*    C\n");
    fmu_ch2.pulsewidth((double)servo_center/1000000U * pw_mf);

    printf("\n* end.\n");
}

// @brief   Test throttle PWM signal
// FIXME: no reverse? -> probably caused by the buggy mbed PWM which caused the ESC to reprogram
void throttle_test(void)
{
    uint16_t pw;
    uint16_t loop;

    printf("* throttle_test():\n");

    // There values are in microseconds
    const uint16_t throttle_rev_max  = 1250;    // Maximum reverse (pedal to the metal)
    const uint16_t throttle_rev_min  = 1400;    // Minimum reverse (just moving)
    const uint16_t throttle_brake    = 1500;    // Brake
    const uint16_t throttle_fw_min   = 1550;    // Minimum forward (just moving)
    const uint16_t throttle_fw_max   = 1750;    // Maximum forward (floor it)

//    FastPWM fmu_ch4(FMU_CH4_PIN, 64);         // prescaler 64 works for a period of 20ms
//  fmu_ch4.period(0.02);                       // period 20ms is spot on
    fmu_ch4.period(0.02 / pw_mf);               // FIXME: suddenly it's 16.67ms
//  fmu_ch4.pulsewidth(0.0015);                 // pulsewidth in seconds is spot on
    fmu_ch4.pulsewidth(0.0015 / pw_mf);         // FIXME: also not correct

    // Start with brake mode (ESC should stop beeping since there is a servo signal present)
    fmu_ch4.pulsewidth((double)throttle_brake/1000000U / pw_mf);

    // Give the ESC some time to play it's startup tune
    thread_sleep_for(4000);

    // Warning LED
    for(loop=0; loop<20; loop++) {
        ui_led_red = 0;
        thread_sleep_for(100);
        ui_led_red = 1;
        thread_sleep_for(100);
    }

    // WARNING: Motor will run in this loop!
    for (loop=0; loop<4; loop++) {
        printf("*    fw_min=%d  mf'd=%f\n", throttle_fw_min, (double)throttle_fw_min/1000000U / pw_mf);
        fmu_ch4.pulsewidth((double)throttle_fw_min/1000000U / pw_mf);
        thread_sleep_for(5000);

        printf("*    brake=%d, mf'd=%f\n", throttle_brake, (double)throttle_brake/1000000U / pw_mf);
        fmu_ch4.pulsewidth((double)throttle_brake/1000000U / pw_mf);
        thread_sleep_for(5000);

        printf("*    rev_min=%d, mf=%f\n", throttle_rev_min, (double)throttle_rev_min/1000000U / pw_mf);
        fmu_ch4.pulsewidth((double)throttle_rev_min/1000000U / pw_mf);
        thread_sleep_for(5000);
    }

    // End with brake mode
    printf("*    brake=%d\n", throttle_brake);
    printf("*    brake=%d, mf'd=%f\n", throttle_brake, (double)throttle_brake/1000000U / pw_mf);
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
