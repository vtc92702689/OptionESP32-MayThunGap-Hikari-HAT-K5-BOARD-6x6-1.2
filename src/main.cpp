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
      "text": "CAI DAT",
      "key": "CD",
      "children": {
        "CD1": {
          "text": "Text1",
          "value": 50,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD2": {
          "text": "Text2",
          "value": 100,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD3": {
          "text": "Text3",
          "value": 200,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD4": {
          "text": "Text4",
          "value": 200,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD5": {
          "text": "Text5",
          "value": 300,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD6": {
          "text": "Text6",
          "value": 400,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD7": {
          "text": "Text7",
          "value": 500,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD8": {
          "text": "Text8",
          "value": 600,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD9": {
          "text": "Text9",
          "value": 700,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD10": {
          "text": "Text10",
          "value": 500,
          "function": 1,
          "belonging": "",
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        }
      }
    },
    "main2": {
      "text": "KIEM TRA",
      "key": "KT",
      "children": {}
    },
    "main3": {
      "text": "GIOI THIEU",
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


int btnSetDebounceMill = 0;
int btnSetPressMill = 2000;
int pIndex = 1;
int menuIndex = 1;
OneButton btnMenu(32, false,false); 
OneButton btnSet(33, false,false);
OneButton btnUp(34, false,false);
OneButton btnDown(35, false,false);

const char* menu1;
const char* menu2;
const char* menu3;
const char* displayScreen;

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

void showList(int indexNum){
  /*JsonObject sensors = jsonDoc["main"]
  size_t sensorCount = sensors.size();

  for (int i = 0; i < sensorCount; i++)*/

  menu1 = jsonDoc["main"]["main1"]["text"];
  menu2 = jsonDoc["main"]["main2"]["text"];
  menu3 = jsonDoc["main"]["main3"]["text"];

  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)

  
  u8g2.drawStr(12, 16, menu1);  // Hiển thị danh mục 1
  u8g2.drawStr(12, 32, menu2);  // Hiển thị danh mục 2
  u8g2.drawStr(12, 48, menu3);  // Hiển thị danh mục 3

  u8g2.drawStr(0, indexNum * 16, ">");  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
  displayScreen = "MENU";
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
  //Serial.println("Button Clicked (nhấn nhả)");
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnMenuLongPressStart() {
  //Serial.println("Button Long Press Started (BtnMenu)");
}

// Hàm callback khi nút đang được giữ
void btnMenuDuringLongPress() {
  //Serial.println("Button is being Long Pressed (BtnMenu)");
}

void btnSetClick() {
  if (displayScreen == "MENU") {
    // Đặt pIndex bằng 1 hoặc giá trị bạn muốn
    int pIndex = 1;

    // Truy xuất key của mục menu hiện tại
    const char* code = jsonDoc["main"]["main" + String(menuIndex)]["key"];

    // Tạo setupCode dựa trên key và pIndex
    String setupCode = String(code) + String(pIndex);  // Chuyển đổi thành String

    // Truy xuất value và text từ JSON
    const char* value = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["value"];
    const char* text = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["text"];

    // Hiển thị giá trị thiết lập
    showSetup(setupCode.c_str(), value, text);  // Chuyển đổi về const char*

    // Chuyển màn hình sau khi xử lý
    displayScreen = "Screen2";
  }
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
  if (menuIndex + 1 > 3) {
    menuIndex = 1;  // Khi chỉ số vượt quá giới hạn, quay lại đầu danh sách
  } else {
    menuIndex++;    // Tăng menuIndex lên 1
  }
  showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnUpLongPressStart() { 
  //Serial.println("Button Long Press Started (btnUp)");
}

// Hàm callback khi nút đang được giữ
void btnUpDuringLongPress() {
  //Serial.println("Button is being Long Pressed (btnUp)");
}

void btnDownClick() {
  if (menuIndex - 1 < 1) {
    menuIndex = 3;  // Khi chỉ số nhỏ hơn giới hạn, quay lại cuối danh sách
  } else {
    menuIndex--;    // Giảm menuIndex đi 1
  }
  showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnDownLongPressStart() {
  //Serial.println("Button Long Press Started (btnDown)");
}

// Hàm callback khi nút đang được giữ
void btnDownDuringLongPress() {
  //Serial.println("Button is being Long Pressed (btnDown)");
}

void setup() {

  Serial.begin(115200);     // Khởi tạo Serial và màn hình
  u8g2.begin();  // Khởi tạo màn hình OLED
 
  DeserializationError error = deserializeJson(jsonDoc, jsonString);    // Phân tích chuỗi JSON
  if (error) {
    showSetup("Error","E002","Json Error");
    /*Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.f_str());
    return;*/
  }
  btnMenu.attachClick(btnMenuClick);
  btnMenu.attachLongPressStart(btnMenuLongPressStart);
  btnMenu.attachDuringLongPress(btnMenuDuringLongPress);

  btnSet.attachClick(btnSetClick);
  btnSet.attachLongPressStart(btnSetLongPressStart);
  btnSet.attachDuringLongPress(btnSetDuringLongPress);

  btnUp.attachClick(btnUpClick);
  btnUp.attachLongPressStart(btnUpLongPressStart);
  btnUp.attachDuringLongPress(btnUpDuringLongPress);

  btnDown.attachClick(btnDownClick);  
  btnDown.attachLongPressStart(btnDownLongPressStart);
  btnDown.attachDuringLongPress(btnDownDuringLongPress);

  btnMenu.setDebounceMs(btnSetDebounceMill);
  btnSet.setDebounceMs(btnSetDebounceMill);
  btnUp.setDebounceMs(btnSetDebounceMill);
  btnDown.setDebounceMs(btnSetDebounceMill);

  btnMenu.setPressMs(btnSetPressMill);
  btnSet.setPressMs(btnSetPressMill);
  btnUp.setPressMs(btnSetPressMill);
  btnDown.setPressMs(btnSetPressMill);
  


} 

void loop() {
  btnMenu.tick();
  btnSet.tick();
  btnUp.tick();
  btnDown.tick(); 
  showList(menuIndex);
}
