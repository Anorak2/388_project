#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);
    if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) {
            int dist = ser_read(devid);
            u_int16_t temp = ser_read(devid);
            dist = (temp<<8| dist);
            printf("%d\n", dist);
            
            if(dist > 200){
            gpio_write(RED_LED, OFF);
            gpio_write(GREEN_LED, ON);
            }
            else if(dist > 100){
                gpio_write(GREEN_LED, ON);
                gpio_write(RED_LED, ON);
            }
            else if(dist > 60){
                gpio_write(RED_LED, ON);
                gpio_write(GREEN_LED, OFF);
            }
            else if(dist > 0){
                gpio_write(RED_LED, ON);
                gpio_write(GREEN_LED, OFF);
                delay(50);
                gpio_write(RED_LED, OFF);
                gpio_write(GREEN_LED, OFF);
                delay(50);
            }
        }
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.
    if (ser_isready(devid)){
        char data[100];
        ser_readline(devid, 100, data);
        int angle = atoi(data);
        return angle;
    }
}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
    int SERVO_PULSE_MAX = 2400;
    int SERVO_PULSE_MIN = 544;
    int SERVO_PERIOD = 20000;
    float val = (pos*((SERVO_PULSE_MAX-SERVO_PULSE_MIN)/180))+ SERVO_PULSE_MIN;
    gpio_write(PIN_19, ON);
    delay_usec(val);
    gpio_write(PIN_19, OFF);
    delay_usec(SERVO_PERIOD-val);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%d", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            
            //if(angle>0){
            //    steering(gpio, 180);
            //}
            //else {
            //    steering(gpio,0);
            //}
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            steering(gpio, angle);
        }

    }
    return 0;
}
