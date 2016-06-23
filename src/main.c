#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <breezystm32/breezystm32.h>
#include <turbotrig/turbovec.h>

#include "estimator.h"
#include "mavlink.h"
#include "mavlink_param.h"
#include "mavlink_receive.h"
#include "mavlink_stream.h"
#include "mavlink_util.h"
#include "mode.h"
#include "param.h"
#include "sensors.h"
#include "controller.h"
#include "mixer.h"
#include "rc.h"

void printvec(int32_t* v)
{
  printf("[%d, %d, %d]\n", v[0], v[1], v[2]);
}

void printquat(int32_t* q)
{
  printf("[%d, %d, %d, %d]\n", q[0], q[1], q[2], q[3]);
}

void pfvec(vector_t v)
{
  printf("[%d, %d, %d]\n", (int32_t)(v.x*1000000), (int32_t)(v.y*1000000), (int32_t)(v.z*1000000));
}

void pfquat(quaternion_t v)
{
  printf("[%d, %d, %d, %d]\n", (int32_t)(v.w*1000000), (int32_t)(v.x*1000000), (int32_t)(v.y*1000000), (int32_t)(v.z*1000000));
}

void setup(void)
{
  // Make sure all the perhipherals are done booting up before starting
  delay(500);

  // Load Default Params
  // Read EEPROM to get initial params
  init_params();

  /***********************/
  /***  Hardware Setup ***/
  /***********************/

  // Initialize I2c
  i2cInit(I2CDEV_2);

  // Initialize PWM and RC
  init_PWM();
  init_rc();

  // Initialize MAVlink Communication
//  init_mavlink();

  // Initialize Sensors
  init_sensors();


  /***********************/
  /***  Software Setup ***/
  /***********************/

  // Initialize Motor Mixing
  init_mixing();

  // Initialize Estimator
  init_estimator();
  init_mode();
  _armed_state = ARMED;
  delay(1000);
}

uint32_t counter = 0;
uint32_t average_time = 0;

void loop(void)
{
  /*********************/
  /***  Pre-Process ***/
  /*********************/
  // get loop time
  static uint32_t prev_time;
  static int32_t dt = 0;
  uint32_t now = micros();

  /*********************/
  /***  Control Loop ***/
  /*********************/
  // update sensors - an internal timer runs this at a fixed rate
  uint32_t before = micros();
  if (update_sensors(now)) // 434 us
  {
    uint32_t after = micros();
//    // loop time calculation
    dt = after-before;
    average_time+=dt;
    counter++;

//    // If I have new IMU data, then perform control
    run_estimator(now); // 193 us (gyro only, float-based)
//    run_controller(now); // 6us
//    mix_output();
  }

//  if(counter > 1000){
//    printf("average time = %d\n", average_time/counter);
//    counter = 0;
//    average_time = 0;
//  }


  /*********************/
  /***  Post-Process ***/
  /*********************/
  // internal timers figure out what to send
//  mavlink_stream(now);

  // receive mavlink messages
//  mavlink_receive();

  // update the armed_states, an internal timer runs this at a fixed rate
//  check_mode(now); // 0 us

  // get RC, an internal timer runs this every 20 ms (50 Hz)
//  receive_rc(now); // 1 us

  // update commands (internal logic tells whether or not we should do anything or not)
//  mux_inputs(); // 3 us

}



