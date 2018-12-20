/*

   EGC-8 ECU

## Dumping the ECU

Read command:  78 MSB LSB 00
Write command: AA MSB LSB 00

Need to read to be able to write (78123400 then AA1234XX)

Memory Map
0000-00FF IO
0100-0FFF unused
1000-14FF RAM
1500-1FFF unused
2000-2800 Memory mapped IO (addresses 2000,2200,2400,2600,2800)
2801-3FFF unused
4000-411F unknown (ECU writes values here, but never reads)
4120-7FFF unused
8000-8FFF ROM data
9000-FFB5 ROM code
FFD0-FFFF Interrupt vectors
*/

void dump_address_space(int begin, int end) {
        for (int i = begin; i <= end; ++i) {
                byte b1 = 1;
                byte b2 = 2;
                digitalWrite(LED_PIN, HIGH);
                int timeout = 2;
                while (b1 != b2 && timeout) {
                        b1 = read_data_from_address(i);
                        stop_read();
                        b2 = read_data_from_address(i);
                        stop_read();
                        if (b1 != b2) {
                                /*
                                 * Some addresses are used as RAM by the ECU, so some
                                 * values keep changing. If the value changes more than
                                 * 3 times, then we treat this as RAM and send one of
                                 * the values.
                                 */
                                timeout--;
                                USBSerial.print("DEBUG at ");
                                USBSerial.print(i, HEX);
                                USBSerial.print(": ");
                                USBSerial.print(b1, HEX);
                                USBSerial.print(" != ");
                                USBSerial.println(b2, HEX);
                        }
                }
                digitalWrite(LED_PIN, LOW);
                USBSerial.print(i, HEX);
                USBSerial.print(":");
                USBSerial.print(b1, HEX);
                if (!timeout) { // "RAM"
                        USBSerial.print(":");
                        USBSerial.println(b2, HEX);
                } else { // Fix byte
                        USBSerial.println(":");
                }
        }
}


void monitor_special_addresses(int force_one) {
        short short_array[] = {
                0x0627, // 0
                0x0690,
                0x0691, // 2
                0x06b4,
                0x1d40, // 4
                0x1d41,
                0x1d42, // 6
                0x066f,
                0x07a8, // 8
                0x07de,
                0x074c, // 10
                0x060b,
                0x606, // 12
                0x808, 
                0x662, // 14
                0x660 // 14
        };

        const char* string_array[] = {
                "bitvector                   ",
                "potential speed             ",
                "after potential speed       ",
                "speed_limiter_related_byte_2",
                "speed_limiter_related_byte_8",
                "speed_limiter_related_byte_7",
                "speed_limiter_related_byte_3",
                "potential speed 2           ",
                "bit_3_vector_1              ",
                "nsl_bitvector               ",
                "nsl_bitvector2              ",
                "compared_to_potential_speed ",
                "potential rpm               ",
                "potential rpm 2             ",
                "potential rpm 3             ",
                "potential rpm 4             "
        };

        int n = sizeof(short_array) / sizeof(short_array[0]);
        for (int i = 0; i < n; ++i) {
                if (force_one > -1) {
                        i = force_one;
                }
                byte b1 = read_data_from_address_ex(short_array[i], true);
                USBSerial.print(string_array[i]);
                USBSerial.print(" --> 0x");
                USBSerial.print(short_array[i], HEX);
                USBSerial.print(":0x");
                USBSerial.print(b1, HEX);
                USBSerial.print(" (");
                USBSerial.print(b1);
                USBSerial.println(")");
                byte b2 = read_speed();
                USBSerial.print("speed                        --> 0x");
                USBSerial.print(SPEED_ADDR, HEX);
                USBSerial.print(":0x");
                USBSerial.print(b2, HEX);
                USBSerial.print(" (");
                USBSerial.print(b2);
                USBSerial.println(")");

                if (force_one > -1) {
                        break;
                }
        }
}
