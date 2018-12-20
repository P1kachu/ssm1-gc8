#include "definitions.h"
#include "helpers.h"

volatile int edges = 0;
volatile int car_speed = 0;
volatile int signal_value = 0;
volatile int previous_signal_value = 0;
volatile int current_refresh_rate = 0;
volatile bool slb_enabled = false;
IntervalTimer timer;

void setup() {
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, LOW);

        // Analog pins to read from VSS and send signal back to the ECU
        pinMode(VSS_TX, OUTPUT);
        pinMode(VSS_RX, INPUT);

        for(int i = 0; i < 5; ++i) {
                delay(30);
                digitalWrite(LED_PIN, HIGH);
                delay(30);
                digitalWrite(LED_PIN, LOW);
        }

#if TEST
        Serial.begin(9600);
        Serial.println("Setup done");
#endif

#if OLED
        Wire.setClock(400000);
        u8g2.begin();
        oled_print_splashscreen();
#endif
}

void get_speed() {
        /*
         * // TODO: Update with new tires (55 ratio reverted to 50 original ratio)
         * Driving around gave approximately 500 edges per 100 meters,
         * which means 5 for 1m
         * So we convert the number of edges to a distance (in meters) by
         * dividing the number of edges by 5, which gives us x meters/REFRESH_PER_SECOND
         * We then multiply it by REFRESH_PER_SECONDS to get a value in m/s,
         * and finish by multiplying by 3.6 to get km/h
         * REFRESHES_PER_SECOND is limited by the number of teeth. We can't
         * have precise values at low speed if refresh too often.
         */

        car_speed = edges * current_refresh_rate * 3.6 / 4 ;

#if TEST
        Serial.print(signal_value);
        Serial.print(" ");
        Serial.print(current_refresh_rate);
        Serial.print(" ");
        Serial.print(edges);
        Serial.print(" ");
        Serial.println(car_speed);
#endif

#if VARIABLE_REFRESH_RATE
        int previous_refresh_rate = current_refresh_rate;
        if (car_speed > (REFRESHES_PER_SECOND_AT_LOW_SPEED * REFRESHES_MULT_AT_HIGH_SPEED)) {
                current_refresh_rate = car_speed / REFRESHES_MULT_AT_HIGH_SPEED;
        } else {
                current_refresh_rate = REFRESHES_PER_SECOND_AT_LOW_SPEED;
        }
        if (previous_refresh_rate != current_refresh_rate) {
                timer.begin(get_speed, ONE_SECOND / current_refresh_rate);
        }
#endif

#if OLED
        if (slb_enabled) {
                oled_print(speeds[car_speed]);
        } else {
                oled_print(" -.- ");
        }
#endif

        edges = 0;
}

void record_signal() {

        signal_value = analogRead(VSS_RX);

        if (signal_value > SIGNAL_MEAN) {
                digitalWrite(LED_PIN, HIGH);
        } else {
                digitalWrite(LED_PIN, LOW);
        }

        if (abs(signal_value - previous_signal_value) > SIGNAL_MEAN) {
                edges++;
        }

        previous_signal_value = signal_value;
        
}

void loop() {
        signal_value = analogRead(VSS_RX);
        previous_signal_value = signal_value;
        int ctr = 0;
        int tmp_signal_value = 0;

        // Will be called in "background"
        timer.begin(get_speed, ONE_SECOND / REFRESHES_PER_SECOND_AT_LOW_SPEED);
        current_refresh_rate = REFRESHES_PER_SECOND_AT_LOW_SPEED;

        // Avoid the loop call overhead (I don't know how fast the teensy goes)
        while (true) {
                record_signal();
                if (ctr % TRIGGER == 0) {
                        if (tmp_signal_value) {
                                tmp_signal_value = 0;
                                digitalWrite(LED_PIN, LOW);
                        } else {
                                tmp_signal_value = 1023;
                                digitalWrite(LED_PIN, HIGH);
                        }
                        ctr = 0;
                }
                if(car_speed > 6) {
                        slb_enabled = true;
                        analogWrite(VSS_TX, tmp_signal_value);
                } else {
                        slb_enabled = false;
                        analogWrite(VSS_TX, signal_value);
                }
                ++ctr;
        }
}
