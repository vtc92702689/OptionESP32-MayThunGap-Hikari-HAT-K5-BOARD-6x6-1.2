#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <OneButton.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Khởi tạo đối tượng màn hình OLED U8G2


StaticJsonDocument<200> jsonDoc;

const char* jsonString = R"({
  "main": {
    "main1": {
      "text": "Cài Đặt",
      "key": "CD",
      "children": {
        "CD1": {
          "text": "Text1",
          "value": "GT1",
          "function": "CN1",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD2": {
          "text": "Text2",
          "value": "GT2",
          "function": "CN2",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD3": {
          "text": "Text3",
          "value": "GT3",
          "function": "CN3",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD4": {
          "text": "Text4",
          "value": "GT4",
          "function": "CN4",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD5": {
          "text": "Text5",
          "value": "GT5",
          "function": "CN5",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD6": {
          "text": "Text6",
          "value": "GT6",
          "function": "CN6",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD7": {
          "text": "Text7",
          "value": "GT7",
          "function": "CN7",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD8": {
          "text": "Text8",
          "value": "GT8",
          "function": "CN8",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD9": {
          "text": "Text9",
          "value": "GT9",
          "function": "CN9",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD10": {
          "text": "Text10",
          "value": "GT10",
          "function": "CN10",
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        }
      }
    },
    "main2": {
      "text": "Kiểm Tra",
      "key": "KT",
      "children": {}
    },
    "main3": {
      "text": "Giới Thiệu",
      "key": "GT",
      "children": {}
    }
  }
})";


// Khai báo các nút
/*const int btnMenu = 32;
const int btnSet = 33;
const int btnUp = 34;
const int btnDown = 35;*/

OneButton btnMenu(32, false,false); 
OneButton btnSet(33, false,false);
OneButton btnUp(34, false,false);
OneButton btnDown(35, false,false);

void wrapText(const char* text, int16_t x, int16_t y, int16_t lineHeight, int16_t maxWidth) {   // Hàm wrapText để hiển thị văn bản xuống dòng nếu dài quá
  int16_t cursorX = x;  // Vị trí x bắt đầu in
  int16_t cursorY = y;  // Vị trí y bắt đầu in
  const char* wordStart = text;  // Vị trí bắt đầu của từ trong chuỗi
  const char* currentChar = text;  // Ký tự hiện tại đang xử lý

  while (*currentChar) {     // Vòng lặp qua từng ký tự trong chuỗi
    if (*currentChar == ' ' || *(currentChar + 1) == '\0') {    
      char word[64];   // Tạo chuỗi tạm để chứa từ hiện tại
      int len = currentChar - wordStart + 1;
      strncpy(word, wordStart, len);
      word[len] = '\0';

      int16_t textWidth = u8g2.getStrWidth(word);  // Kiểm tra nếu từ có vừa với chiều rộng màn hình
      if (cursorX + textWidth > maxWidth) {
        cursorX = x;  // Nếu từ quá dài, xuống dòng
        cursorY += lineHeight;  // Tăng vị trí y để xuống dòng
      }

      u8g2.drawStr(cursorX, cursorY, word);  // Vẽ từ lên màn hình
      cursorX += textWidth;  // Cập nhật vị trí x cho từ tiếp theo
      
      if (*currentChar == ' ') {
        cursorX += u8g2.getStrWidth(" ");  // Thêm khoảng trắng nếu ký tự là ' '
      }

      wordStart = currentChar + 1; // Di chuyển đến từ tiếp theo
    }
    currentChar++;  // Chuyển ký tự hiện tại sang ký tự tiếp theo
  }
}


void showSetup(const char* setUpCode, const char* value, const char* text) {   // Hàm hiển thị màn hình với thông tin mã cài đặt và giá trị
  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf);  // Thiết lập font chữ đậm

  char tempSetUpCode[64];    // Tạo một chuỗi tạm chứa mã cài đặt và dấu ";"
  snprintf(tempSetUpCode, sizeof(tempSetUpCode), "%s;", setUpCode);  // Nối mã cài đặt với dấu ";"
  
  u8g2.drawStr(0, 18, tempSetUpCode);  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)
  u8g2.drawStr(82, 18, value);  // Hiển thị giá trị cài đặt (tại vị trí x=82, y=18)
  u8g2.drawLine(0, 23, 128, 23);  // Vẽ một đường ngang trên màn hình (tọa độ từ x=0 đến x=128)

  wrapText(text, 0, 42, 18, 128);  // Bắt đầu tại tọa độ x=0, y=46, mỗi dòng cách nhau 18 điểm, tối đa chiều rộng 128 điểm
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

void btnMenuClick() {
  Serial.println("Button Clicked (nhấn nhả)");
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnMenuLongPressStart() {
  Serial.println("Button Long Press Started (bắt đầu nhấn giữ)");
}

// Hàm callback khi nút đang được giữ
void btnMenuDuringLongPress() {
  Serial.println("Button is being Long Pressed (đang giữ)");
}

void btnSetClick() {
  const char* text = jsonDoc["main"]["main1"]["text"];
  const char* key = jsonDoc["main"]["main1"]["key"];
  //showSetup(key,text,"doc thanh cong");
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnSetLongPressStart() {
  showSetup("Setup", "OFF", "BatDauNhanGiu");
}

// Hàm callback khi nút đang được giữ
void btnSetDuringLongPress() {
  showSetup("Setup", "OFF", "Dang giu nut");
}

void btnUpClick() {
  Serial.println("Button Clicked (nhấn nhả)");
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnUpLongPressStart() {
  Serial.println("Button Long Press Started (bắt đầu nhấn giữ)");
}

// Hàm callback khi nút đang được giữ
void btnUpDuringLongPress() {
  Serial.println("Button is being Long Pressed (đang giữ)");
}

void btnDownClick() {
  Serial.println("Button Clicked (nhấn nhả)");
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnDownLongPressStart() {
  Serial.println("Button Long Press Started (bắt đầu nhấn giữ)");
}

// Hàm callback khi nút đang được giữ
void btnDownDuringLongPress() {
  Serial.println("Button is being Long Pressed (đang giữ)");
}

void setup() {

  Serial.begin(115200);     // Khởi tạo Serial và màn hình
  u8g2.begin();  // Khởi tạo màn hình OLED
 
  StaticJsonDocument<200> jsonData;
  DeserializationError error = deserializeJson(jsonDoc, jsonString);    // Phân tích chuỗi JSON
  if (error) {
    showSetup("Error","E002","Json Error");
    /*Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;*/
  }

  btnMenu.attachClick(btnMenuClick);
  btnMenu.attachLongPressStart(btnMenuLongPressStart);
  btnMenu.attachDuringLongPress(btnMenuDuringLongPress);

  btnSet.attachClick(btnSetClick);
  btnSet.attachLongPressStart(btnSetLongPressStart);
  btnSet.attachDuringLongPress(btnSetDuringLongPress);

   const char* main1Text = jsonDoc["main"]["main1"]["text"];
  const char* main1Key = jsonDoc["main"]["main1"]["key"];
  const char* child1Text = jsonDoc["main"]["main1"]["children"]["CD1"]["text"];
  const char* child1Value = jsonDoc["main"]["main1"]["children"]["CD1"]["value"];
  const char* child1Function = jsonDoc["main"]["main1"]["children"]["CD1"]["function"];

  // In ra các giá trị đã truy cập
  Serial.println(main1Text);         // In ra "Cài Đặt"
  Serial.println(main1Key);          // In ra "CD"
  Serial.println(child1Text);        // In ra "Text1"
  Serial.println(child1Value);       // In ra "GT1"
  Serial.println(child1Function);     // In ra "CN1"
}

void loop() {
  btnSet.tick();
}
