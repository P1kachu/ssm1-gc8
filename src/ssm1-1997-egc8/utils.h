#pragma once
#include "definitions.h"

void read_until_no_more() {

#if PRINT_DEBUG_MESSAGES_ON_USB
        int read_byte;
        if (HWSerial.available() > 0) {
                USBSerial.print("UART received:");
                while (HWSerial.available() > 0) {
                        read_byte = HWSerial.read();
                        USBSerial.print(" ");
                        if (read_byte < 0x10) {
                                USBSerial.print("0");
                        }
                        USBSerial.print(read_byte, HEX);
                }
                USBSerial.println("");
        } else {
                //                USBSerial.println("Nothing available on UART");
        }
#else
        HWSerial.clear();
#endif
}

void print_received_data(byte answer[3]) {

#if PRINT_DEBUG_MESSAGES_ON_USB
        USBSerial.print("Received answer: ");
        for (int i = 0; i < 3; ++i) {
                USBSerial.print(" ");
                if (answer[i] < 0x10) {
                        USBSerial.print("0");
                }
                USBSerial.print(answer[i], HEX);
        }
        USBSerial.println(" ");
#endif
}


void simulate_tester() {
        byte test[4] = {0};
        byte rom_id_command[4] = {0x00, 0x46, 0x48, 0x49};
        byte read_command[4] = {0x78, 0x00, 0x00, 0x00};
        byte ecu_stop_command[4] = {0x12, 0x00, 0x00, 0x00};

        if(!HWSerial.readBytes(test, 4)) { return; }
        bool is_rom_id_command = true;
        bool is_read_command = test[0] == read_command[0];
        bool is_stop_command = test[0] == ecu_stop_command[0];

        for (int i = 0; i < 4; ++i) {
                if (test[i] != rom_id_command[i]) {
                        is_rom_id_command = false;
                }
        }

        HWSerial.clear();

        if (is_rom_id_command) {
                digitalWrite(LED_PIN, HIGH);
                USBSerial.println("ROM ID COMMAND");
                byte custom_rom_id[3] = {'P', '1', 'K'};

                for (int i = 0; i < 3; ++i) {
                        HWSerial.write(custom_rom_id[i]);
                }
                HWSerial.flush();
                digitalWrite(LED_PIN, LOW);
        } else if (is_read_command) {
                digitalWrite(LED_PIN, HIGH);
                USBSerial.println("READ COMMAND");
                byte answer[3] = {test[1], test[2], 0x55};

                for (int i = 0; i < 3; ++i) {
                        HWSerial.write(answer[i]);
                }
                HWSerial.flush();
                digitalWrite(LED_PIN, LOW);
        } else if (is_stop_command) {
                digitalWrite(LED_PIN, HIGH);
                USBSerial.println("STOP COMMAND");
                digitalWrite(LED_PIN, LOW);
        } else {
                USBSerial.print("Received over UART: ");
                for (int i = 0; i < 4; ++i) {
                        USBSerial.print(" ");
                        if (test[i] < 0x10) {
                                USBSerial.print("0");
                        }
                        USBSerial.print(test[i], HEX);
                }
                USBSerial.println("");
                byte answer[4] = {'F', 'A', 'I', 'L'};

                for (int i = 0; i < 4; ++i) {
                        HWSerial.write(answer[i]);
                }
                return;
        }
}
