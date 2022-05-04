#include "MK66F18.h"
#include "PwmOut.h"
#include "mbed.h"
#include "board_fmuk66.h"
#include "mbed_thread.h"
#include "FastPWM.h"
#include "pixy.h"

// Used libraries:
// - Debug output on the Segger probe (just install and it should work on the Segger RTT terminal)
//      - http://os.mbed.com/users/0x6d61726b/code/mbed-os-retarget-segger-rtt/
// - FastPWM - This should fix the incredible stupid Mbed implementation of PWM, where the PWM counter is reset when setting a new pulsewidth, 
//   in effect changing the period time with same rate as the setting update rate. Say you update the pulsewidth every 5 ms, your period will be 5 ms instead of the 20ms we'd expect.
//   In fact, the Mbed implementation broke my (plastic gear) servo. It's missing quite a few gear teeth now.
//   NOTE: change the defined() in FastPWM_KSDK.cpp to "#if defined(TARGET_KPSDK_MCUS) || defined(TARGET_FMUK66) || defined(TARGET_K66F)" or the code won't compile.
//      - http://os.mbed.com/users/Sissors/code/FastPWM/

void buzzer_test(void);
void servo_test(void);
void throttle_test(void);
void not_so_blinky(void);

// main() runs in its own thread in the OS
int main()
{
    board_init();
    board_debug_intro();
    SystemCoreClockUpdate();
    printf("kCLOCK_BusClk = %d\n", CLOCK_GetFreq(kCLOCK_BusClk));
    printf("SystemCoreClock = %d\n", SystemCoreClock);
    
    uint32_t rng_data[1];
    RNGA_GetRandomData(RNG, rng_data, 1);
    printf("RNG: %08X\n", rng_data[0]);
    
    // FIXME: Can't use I2C due to an mbed runtime pinmap error
    //pixy_detect();

    led_green = 0;
    nsafety_led = 1;

    buzzer_test();
    servo_test();
    throttle_test();
    
    not_so_blinky();
}


// @brief   There used to be a lot more going on in here
void not_so_blinky(void)
{
    while(1) {
        ui_led_green = !ui_led_green;
        thread_sleep_for(250);
    }
}


void buzzer_test(void)
{
    // TODO: the frequencies are all wrong but it sounds great.

    const uint16_t tones[] = {4000, 3000, 2000, 1000};
    uint16_t i;
    double period;

    for (i=0; i<sizeof(tones)-1; i++) {
        period = 1.0/tones[i];
        buzzer.period(period);
        buzzer.pulsewidth(period/2);      // 50%
        thread_sleep_for(100);
    }

    buzzer.pulsewidth(0.0);               // 0%
}


// FIXME: change to FastPWM
void servo_test(void)
{
    uint16_t pw;
    uint16_t loop;

    printf("* servo_test(): ");

    const uint16_t servo_min = 1250;
    const uint16_t servo_center = 1500;
    const uint16_t servo_max = 1750;
    const uint16_t delay = 5;           // CAUTION: a delay of 1ms caused erratic servo behaviour which destroyed my plastic-gear servo - See comment on top of board_fmuk66.h about Mbed and PWM

    PwmOut fmu_ch2(FMU_CH2_PIN);
    fmu_ch2.period_ms(20);
    fmu_ch2.pulsewidth_us(servo_center);

    // Go slowly
    for (loop=0; loop<2; loop++) {
        printf("%d ", loop);
        for (pw=servo_center; pw<=servo_max; pw++) {
            fmu_ch2.pulsewidth_us(pw);
            thread_sleep_for(delay);
        }
        for (; pw>=servo_min; pw--) {
            fmu_ch2.pulsewidth_us(pw);
            thread_sleep_for(delay);
        }
        for (; pw<=servo_center; pw++) {
            fmu_ch2.pulsewidth_us(pw);
            thread_sleep_for(delay);
        }
    }

    // Full servo speed
    for (loop=0; loop<4; loop++) {
        fmu_ch2.pulsewidth_us(servo_min);
        thread_sleep_for(1000);
        
        fmu_ch2.pulsewidth_us(servo_max);
        thread_sleep_for(1000);
    }

    // Back to center
    fmu_ch2.pulsewidth_us(servo_center);

    printf("\n* end.\n");
}

// @brief   Test throttle PWM signal
// FIXME: no reverse? -> probably caused by the buggy mbed PWM which caused the ESC to reprogram
void throttle_test(void)
{
    uint16_t pw;
    uint16_t loop;

    printf("* throttle_test(): ");

    // There values are in microseconds
    const uint16_t throttle_rev_max  = 1250;    // Maximum reverse (pedal to the metal)
    const uint16_t throttle_rev_min  = 1400;    // Minimum reverse (just moving)
    const uint16_t throttle_brake    = 1500;    // Brake
    const uint16_t throttle_fw_min   = 1525;    // Minimum forward (just moving)
    const uint16_t throttle_fw_max   = 1750;    // Maximum forward (floor it)

    //PwmOut fmu_ch4(FMU_CH4_PIN);
    FastPWM fmu_ch4(FMU_CH4_PIN, 64);           // prescaler 64 works for a period of 20ms
    fmu_ch4.period(0.02);                       // period 20ms is spot on
    //fmu_ch4.pulsewidth_us(1500/1.25);           // pulsewidth is not correct, 1200 gives about 1500us so just divide it by 1.25
    fmu_ch4.pulsewidth(0.0015);                 // pulsewidth in seconds is spot on

    // Start with brake mode (ESC should stop beeping since there is a servo signal present)
    fmu_ch4.pulsewidth((double)throttle_brake/1000000U);

    // Give the ESC some time to play it's startup tune
    thread_sleep_for(4000);

#if (0)
    // DEBUG DEBUBG DEBUG DEBUG - should go between 1000 and 2000 us        - Aye it works mate!
    while (1) {
        for (pw=1500; pw<=2000; pw++) {
            fmu_ch4.pulsewidth((double)pw/1000000U);
            thread_sleep_for(1);
        }
        for (pw=2000; pw>=1000; pw--) {
            fmu_ch4.pulsewidth((double)pw/1000000U);
            thread_sleep_for(1);
        }
        for (pw=1000; pw<=1500; pw++) {
            fmu_ch4.pulsewidth((double)pw/1000000U);
            thread_sleep_for(1);
        }
    }
#endif

    // Warning LED
    for(loop=0; loop<20; loop++) {
        ui_led_red = 0;
        thread_sleep_for(100);
        ui_led_red = 1;
        thread_sleep_for(100);
    }

    // WARNING: Motor will run in this loop!
    for (loop=0; loop<4; loop++) {

        fmu_ch4.pulsewidth((double)throttle_fw_min/1000000U);
        thread_sleep_for(2000);

        fmu_ch4.pulsewidth((double)throttle_brake/1000000U);
        thread_sleep_for(2000);

        fmu_ch4.pulsewidth((double)throttle_rev_min/1000000U);
        thread_sleep_for(2000);
    }

    // End with brake mode
    fmu_ch4.pulsewidth((double)throttle_brake/1000000U);
    thread_sleep_for(1000);
    printf("* end.\n");
    ui_led_red = 0;
    thread_sleep_for(1000);
}
