#include "mbed.h"
#include "MK66F18.h"
#include "board_fmuk66.h"
//#include "mbed_thread.h"
#include "mbed_thread.h"
#include "pixy.h"
#include "board_test.h"
#include "pixyfeatures.h"


// Used libraries:
// - Debug output on the Segger probe (just install and it should work on the Segger RTT terminal)
//      - http://os.mbed.com/users/0x6d61726b/code/mbed-os-retarget-segger-rtt/
// - FastPWM - This should fix the incredible stupid Mbed implementation of PWM, where the PWM counter is reset when setting a new pulsewidth, 
//   in effect changing the period time with same rate as the setting update rate. 
//   Say you update the pulsewidth every 5 ms, your period will be 5 ms instead of the 20ms we'd expect.
//   In fact, the Mbed implementation broke my (plastic gear) servo. It's missing quite a few gear teeth now.
//   NOTE: change the defined() in FastPWM_KSDK.cpp to "#if defined(TARGET_KPSDK_MCUS) || defined(TARGET_FMUK66) || defined(TARGET_K66F)" or the code won't compile.
//      - http://os.mbed.com/users/Sissors/code/FastPWM/


void wait_for_safety_switch(void) {
    printf("* --- Press safety switch to continue ---\n");
    buzz(1800, 100);

    // Wait for release (if it is still pressed)
    while( safety_switch);
    // Debounce
    thread_sleep_for(100);

    // Flash safety LED while waiting for switch
    while( !safety_switch ) {
        nsafety_led = 0;
        thread_sleep_for(100);
        nsafety_led = 1;
        thread_sleep_for(100);
    }  
}


void test_drive() {
    Pixy2 *pixy = new Pixy2();
    Pixy2Features pf = Pixy2Features(*pixy);
    while (1) {
        pf.getMainFeatures();
        float speed =  (pf.vectors[0].m_y1 - pf.vectors[0].m_y0)/50.0f;
        float steer =  (pf.vectors[0].m_x1 - pf.vectors[0].m_x0)/50.0f;
        set_servo(-steer);
        set_speed(0.65f);
        thread_sleep_for(20);
    }
}

//
//      M A I N
//
int main()
{
    uint32_t rng_data[1];

    // General init 
    board_init();               // FMUK66 board setup
    SystemCoreClockUpdate();
    printf("\x1B[2J");          // Clear screen on RTT terminal

    //pulsewidth_test();

    while (1) {
        wait_for_safety_switch();
        printf("\x1B[2J");          // Clear screen on RTT terminal
        board_debug_intro();

        // Just a test of the random number generator
        RNGA_GetRandomData(RNG, rng_data, 1);
        printf("* RNG: %08X\n", rng_data[0]);
        wait_for_safety_switch();
        test_drive();

        wait_for_safety_switch();
        Pixy2 *pixy = new Pixy2();
        pixy->init();
        pixy->test();
        Pixy2Features pf = Pixy2Features(*pixy);
        pf.getAllFeatures();
        printf("Counted %d vectors\n", pf.numVectors);
        for (int i = 0; i < pf.numVectors; i++) {
            pf.vectors[i].print(); 
            printf("\n");       
        }
        printf("* End Pixy test\n");

        wait_for_safety_switch();
        servo_test();

        wait_for_safety_switch();
        throttle_test();
    }
}


