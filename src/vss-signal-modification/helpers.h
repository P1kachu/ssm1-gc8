#include "definitions.h"

#if OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

static void oled_print(const char *string) {
        u8g2.setFont(u8g2_font_inb30_mr);
        u8g2.clearBuffer();
        u8g2.drawStr(0, 50, string);
        u8g2.sendBuffer();
}

static void oled_print_splashscreen() {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_profont17_tr);
        u8g2.drawStr(8, 15, "P1kachu!");
        u8g2.setFont(u8g2_font_inb38_mr);
        u8g2.drawStr(5, 60, "~~~~");
        u8g2.sendBuffer();
        delay(1000);
}
#endif
