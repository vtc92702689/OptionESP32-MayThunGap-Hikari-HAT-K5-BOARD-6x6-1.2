#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Preferences.h>
#include <ArduinoJson.h>


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// put function declarations here:
void wrapText(const char* text, int16_t x, int16_t y, int16_t lineHeight, int16_t maxWidth) {
  int16_t cursorX = x;
  int16_t cursorY = y;
  const char* wordStart = text;
  const char* currentChar = text;

  while (*currentChar) {
    // Di chuyển đến cuối từ hiện tại
    if (*currentChar == ' ' || *(currentChar + 1) == '\0') {
      // Tạo một chuỗi tạm cho từ
      char word[64];
      int len = currentChar - wordStart + 1;
      strncpy(word, wordStart, len);
      word[len] = '\0';

      // Kiểm tra xem từ có vừa với chiều rộng còn lại không
      int16_t textWidth = u8g2.getStrWidth(word);
      if (cursorX + textWidth > maxWidth) {
        // Xuống dòng nếu từ không vừa
        cursorX = x;
        cursorY += lineHeight;
      }

      // In từ lên màn hình
      u8g2.drawStr(cursorX, cursorY, word);
      cursorX += textWidth;
      
      // Nếu là ký tự khoảng trắng, in khoảng trắng
      if (*currentChar == ' ') {
        cursorX += u8g2.getStrWidth(" ");
      }

      // Chuyển đến từ tiếp theo
      wordStart = currentChar + 1;
    }
    currentChar++;
  }
}

void showSetup(const char* setUpCode, const char* value, const char* text) {
  u8g2.begin();
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_crox3hb_tf);
	u8g2.drawStr(0, 18, strcat(setUpCode,";"));  //   Hiển thị mã cài đặt
  u8g2.setFont(u8g2_font_crox3h_tf);  //font không đậm
  u8g2.drawStr(82, 18, value);  //  Hiển thị thông số cài đặt
	u8g2.drawLine(0, 23, 128, 23); // Vẽ một line thẳng

	wrapText(text, 0, 46, 18, 128);   // Tọa độ bắt đầu (0, 15), chiều cao mỗi dòng (12), chiều rộng tối đa (128)

  u8g2.sendBuffer();
}

void setup() {
  // Khởi tạo màn hình
  Serial.begin(115200);
  u8g2.begin();
}

void loop() {
  showSetup("U99", "9998", "Cai dat nay de test");
  delay(2000);
  showSetup("U97", "9997", "Cai dat nay de test s2");
  delay(2000);
  showSetup("U96", "9996", "Cai dat nay de test s3");
}