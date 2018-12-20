#include "definitions.h"
#include "ssm1.h"
#include "utils.h"
#include "reverse_engineering.h"

int driving_mode = DRIVING_MODE_NORMAL;

void setup() {
        // Because we like blinky thingies
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, LOW);

        // Serial to read from Subaru, pins 0 and 1
        HWSerial.begin(1953, SERIAL_8E1);

        // Serial to send data over USB
        USBSerial.begin(9600);

        // While no command was received, wait (reader over USB init)
        while(!USBSerial.available()) {
                delay(30);
                digitalWrite(LED_PIN, HIGH);
                delay(30);
                digitalWrite(LED_PIN, LOW);
        }

        stop_read();

#if PRINT_DEBUG_MESSAGES_ON_USB
        query_romid();
#endif

        char cmd[4] = {0};
        USBSerial.readBytes(cmd, 4);
        USBSerial.clear();

        if (!strncmp(cmd, "DUMP", 4)) {
                dump_address_space(0, 0xffff);
        }

#if TEST
        int index = 0;
        int values[] = {1, 7, 8, 9, 10, 11, 12, 13, 14};
        while (true) {
                for (int i = 0; i < 10000; ++i) {
                        monitor_special_addresses(values[index]);
                }

                index++;

                int arr_size = sizeof(values) / sizeof(values[0]);
                if (index > arr_size - 1) {
                        index = 0;
                }
        }
#endif


#if PRINT_DEBUG_MESSAGES_ON_USB
        USBSerial.println("Setup done");
#endif
}

void loop() {

        static int increment = 0;
        static int rpm, throttle, speed, airflow, manifold_pressure, coolant_temp;
        static float battery_voltage;

        if(USBSerial.available()) {
                char cmd[5] = {0};
                USBSerial.readBytes(cmd, 5);
                USBSerial.clear();

                if (!strncmp(cmd, "ROAD ", 5)) { // Road Mode
                        driving_mode = DRIVING_MODE_NORMAL;
                        USBSerial.println("DEBUG: Sport mode off");
                }

                if (!strncmp(cmd, "SPORT", 5)) { // Sport Mode
                        driving_mode = DRIVING_MODE_SPORT;
                        USBSerial.println("DEBUG: Sport mode on");
                }

                if (!strncmp(cmd, "SPEED", 5)) { // Speed limiter tests
                        driving_mode = DRIVING_MODE_SPEED_ONLY;
                        USBSerial.println("DEBUG: Speed mode on");
                }

                // Reset timer in case of change
                increment = 0;
        }


        if (driving_mode == DRIVING_MODE_SPEED_ONLY) {
                speed = read_speed();
                rpm = DATA_NOT_AVAILABLE_CONSTANT;
                throttle = DATA_NOT_AVAILABLE_CONSTANT;
                coolant_temp = DATA_NOT_AVAILABLE_CONSTANT;

        } else {
                if (increment % LEVEL_1_REFRESH == 0) {
                        speed = read_speed();
                        rpm = read_rpm();
                        throttle = read_throttle_percentage();
                }

                if (driving_mode == DRIVING_MODE_NORMAL) {
                        if (increment % LEVEL_2_REFRESH == 0) {
                                airflow = read_airflow();
                                manifold_pressure = read_manifold_pressure();
                        }
                        if (increment % LEVEL_3_REFRESH == 0) {
                        }
                        if (increment % LEVEL_4_REFRESH == 0) {
                        }
                        if (increment % LEVEL_5_REFRESH == 0) {
                                battery_voltage = read_battery_voltage();
                        }
                }

                if (increment % LEVEL_6_REFRESH == 0) {
                        coolant_temp = read_coolant_temp();
                }
        }


#if PRINT_DEBUG_MESSAGES_ON_USB
        USBSerial.println("SPE - RPM - TPS - ºC - MAF - Volts - MAP");
        USBSerial.print(speed); USBSerial.print(" -  ");
        USBSerial.print(rpm); USBSerial.print(" -  ");
        USBSerial.print(throttle); USBSerial.print(" -  ");
        USBSerial.print(coolant_temp); USBSerial.print(" -  ");
        USBSerial.print(airflow); USBSerial.print(" -  ");
        USBSerial.print(manifold_pressure); USBSerial.print(" -  ");
        USBSerial.println(battery_voltage);
#else
        USBSerial.print(speed); USBSerial.print("-");
        USBSerial.print(rpm); USBSerial.print("-");
        USBSerial.print(throttle); USBSerial.print("-");
        USBSerial.print(coolant_temp); USBSerial.print("-");
        USBSerial.print(airflow); USBSerial.print("-");
        USBSerial.print(manifold_pressure); USBSerial.print("-");
        USBSerial.print(battery_voltage); USBSerial.println("");
#endif
        increment++;

        UNUSED_VARIABLE(speed);
        UNUSED_VARIABLE(rpm);
        UNUSED_VARIABLE(throttle);
        UNUSED_VARIABLE(coolant_temp);
        UNUSED_VARIABLE(manifold_pressure);
        UNUSED_VARIABLE(airflow);
}

