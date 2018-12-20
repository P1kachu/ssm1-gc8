#pragma once
#include "definitions.h"
#include "utils.h"

/*
 * http://www.4bc.org/vanagon/SSM_params.html
 * http://www.alcyone.org.uk/ssm/ecureverse.html
 */

void stop_read() {
        byte txbuf[4] = {0x12, 0x00, 0x00, 0x00};
        for (int i = 0; i < 4; ++i) {
                HWSerial.write(txbuf[i]);
        }
        HWSerial.flush();
        delay(50);
}

boolean get_romid(byte * buffer) {
        byte read_command[4] = {0x78, 0x00, 0x00, 0x00};
        byte romid_command[4] = {0x00, 0x46, 0x48, 0x49};
        byte romid[3] = {0};
        stop_read();

        HWSerial.clear();
        for (int i = 0; i < 4; ++i) {
                HWSerial.write(read_command[i]);
        }
        delay(50);
        HWSerial.flush();
        read_until_no_more();


        int retries = 0;
        while (retries < 8) {
                read_until_no_more();
                int nbytes = HWSerial.readBytes(romid, 3);

                if ((nbytes == 3) && (romid[0] != 0x00))
                        break;

                for (int i = 0; i < 4; ++i) {
                        HWSerial.write(romid_command[i]);
                }
                delay(50);
                HWSerial.flush();
                ++retries;

#if PRINT_DEBUG_MESSAGES_ON_USB
                USBSerial.println("Testing ROM ID once again");
#endif

        }

        stop_read();

        if (romid[0] == 0x00) {
                // Couldn't query ROM ID
                return false;
        }

        for (int i = 0; i < 3; ++i) {
                buffer[i] = romid[i];
        }

        return true;
}

void query_romid() {
        byte romid[3] = {0};
        if (get_romid(romid)) {

#if PRINT_DEBUG_MESSAGES_ON_USB
                USBSerial.print("Received ROMID: ");
                USBSerial.print(romid[0], HEX);
                USBSerial.print(romid[1], HEX);
                USBSerial.println(romid[2], HEX);
#endif

                read_until_no_more();
        }
}
byte read_data_from_address_ex(short addr, bool read_once_only) {
        HWSerial.clear();
        byte read_command[4] = {0x78, byte(addr >> 8), byte(addr & 0xff), 0x00};
        byte answer[3] = {0};

        for (int i = 0; i < 4; ++i) {
                HWSerial.write(read_command[i]);
        }
        HWSerial.flush();

        if (!read_once_only && HWSerial.available()) {
                // TODO: test
                HWSerial.readBytes(answer, 3);
                digitalWrite(LED_PIN, HIGH);
                delay(5);
                digitalWrite(LED_PIN, LOW);
                delay(5);
                return answer[2];
        }

        int tries = NUM_READ_TRIES;
        while (answer[0] != read_command[1] || answer[1] != read_command[2]) {
                if (!HWSerial.available()) {
                        tries--;
                        if (!tries) {
                                HWSerial.clear();

                                for (int i = 0; i < 4; ++i) {
                                        HWSerial.write(read_command[i]);
                                }
                                tries = NUM_READ_TRIES;
                        }
                }
                HWSerial.readBytes(answer, 3);
                digitalWrite(LED_PIN, HIGH);
                delay(5);
                digitalWrite(LED_PIN, LOW);
                delay(5);
        }

        if (read_once_only) {
              stop_read();
        }
        return answer[2];
}

byte read_data_from_address(short addr) {
        return read_data_from_address_ex(addr, true);
}

int read_speed() {
        int value = read_data_from_address(SPEED_ADDR) * 2;
        return value;
}

int read_rpm() {
        int value = read_data_from_address(RPM_ADDR) * 25;
        return value;
}

int read_throttle_percentage() {
        int value = (read_data_from_address(THROTTLE_ADDR) * 100) / 256;
        return value;
}

int read_coolant_temp() {
        int value = read_data_from_address(COOLANT_ADDR) - 50;return value;
}

int read_boost_solenoid_thingy() {
        int value = (read_data_from_address(BOOST_SOLENOID_ADDR) * 100) / 256;
        return value;
}

int read_manifold_pressure() {
        int value = read_data_from_address(MANIFOLD_PRESSURE_ADDR);
        return value;
}

int read_airflow() {
        int value = (read_data_from_address(AIRFLOW_SENSOR_ADDR) * 100) / 256;
        return value;
}

float read_battery_voltage() {
        float value = read_data_from_address(BATTERY_VOLTAGE_ADDR) * 0.08;
        return value;
}


