#include <Arduino.h>
#include <OneButton.h>
#include "ota.h"
#include "func.h"  // Bao gồm file header func.h để sử dụng các hàm từ func.cpp


//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Khởi tạo đối tượng màn hình OLED U8G2
//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Khởi tạo đối tượng màn hình OLED U8G2

// Thông tin mạng WiFi và OTA



StaticJsonDocument<200> jsonDoc;

const char* jsonString = R"()";
void tinhToanCaiDat();
void loadSetup();

OneButton btnMenu(0, true,false);
OneButton btnSet(2, false,false);
OneButton btnUp(12, false,false);
OneButton btnDown(15, false,false);
OneButton btnRun(23,false,false);
OneButton btnEstop(13,false,false);


void btnMenuClick() {
  //Serial.println("Button Clicked (nhấn nhả)");
  if (displayScreen == "ScreenCD") {
    if (keyStr == "CD") {
      writeFile(jsonDoc,"/config.json");
    }
    showList(menuIndex);  // Hiển thị danh sách menu hiện tại
    displayScreen = "MENU";
  } else if (displayScreen == "ScreenEdit") {
    loadJsonSettings();
    displayScreen = "ScreenCD";
  } else if (displayScreen == "index" && mainStep == 0) {
    trangThaiHoatDong = 0;
    showList(menuIndex);  // Hiển thị danh sách menu hiện tại
    displayScreen = "MENU";
  } else if (displayScreen == "MENU" && mainStep == 0){
    displayScreen= "index";
    trangThaiHoatDong = 1;
    showText("RUNNING", "ESP32-OPTION");
  } else if (displayScreen == "testIO"){
    loadJsonSettings();
    displayScreen = "ScreenCD";
    trangThaiHoatDong = 0;
  } else if (displayScreen == "testOutput"){
    loadJsonSettings();
    displayScreen = "ScreenCD";
    trangThaiHoatDong = 0;
  } else if (displayScreen == "screenTestMode" && testModeStep == 0){
    loadJsonSettings();
    displayScreen = "ScreenCD";
    trangThaiHoatDong = 0;
  } else if (displayScreen == "OTA"){

  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnMenuLongPressStart() {
  if (displayScreen == "OTA") {
  }
}
// Hàm callback khi nút đang được giữ
void btnMenuDuringLongPress() {
  //Serial.println("Button is being Long Pressed (BtnMenu)");
}

void btnSetClick() {
  if (displayScreen == "MENU") {
    pIndex = 1;
    loadJsonSettings(); // Hiển thị giá trị thiết lập
    displayScreen = "ScreenCD"; // Chuyển màn hình sau khi xử lý dữ liệu thành công
  } else if (displayScreen == "ScreenCD" && editAllowed){
    if (keyStr == "CD"){
      columnIndex = maxLength-1;
      showEdit(columnIndex);
      displayScreen = "ScreenEdit";
    } else if (keyStr == "CN") {
      if (setupCodeStr == "CN1"){
        trangThaiHoatDong = 201;   //Trạng thái hoạt động 201 là trạng thái TestMode
        testModeStep = 0;
        chayTestMode = true;
        showText("TEST MODE", String("Step " + String(testModeStep)).c_str());
        displayScreen = "screenTestMode";
      } else if (setupCodeStr == "CN2"){
        trangThaiHoatDong = 202;   //Trạng thái hoạt động 202 là trạng thái TEST IO INPUT
        showText("TEST I/O", "TEST I/O INPUT");
        displayScreen = "testIO";
      } else if ((setupCodeStr == "CN3")){
        trangThaiHoatDong = 203;  //Trạng thái hoạt động 203 là trạng thái TEST IO OUTPUT
        testOutputStep = 0;
        displayScreen = "testOutput";
        hienThiTestOutput = true;
      } else if ((setupCodeStr == "CN5")){
        setupOTA();
        displayScreen = "OTA";
        trangThaiHoatDong = 204;  //Trạng thái hoạt động 204 là trạng thái OTA UPDATE
      } else {
        columnIndex = maxLength - 1;
        showEdit(columnIndex);
        displayScreen = "ScreenEdit";
      }
    }
  } else if (displayScreen == "ScreenEdit")  {
    if (keyStr == "CD"){
      if (columnIndex - 1 < 0){
        columnIndex = maxLength-1;
      } else {
        columnIndex --;
      }
      showEdit(columnIndex);
    }
  } else if (displayScreen == "testOutput"){
    daoTinHieuOutput = true;
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnSetLongPressStart() {
  if (displayScreen == "ScreenEdit"){
    if (keyStr == "CD"){
      jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"] = currentValue;
      log("Đã lưu giá trị:" + String(currentValue) + " vào thẻ " + keyStr + "/" + setupCodeStr);
      loadJsonSettings();
      loadSetup();
      tinhToanCaiDat();
      displayScreen = "ScreenCD";
    } else if (keyStr == "CN"){
      if (setupCodeStr == "CN4" && currentValue == 1){
        reSet();
        showText("RESET","Tắt máy khởi động lại!");
        trangThaiHoatDong = 200;  //Trạng thái hoạt động 200 là reset, không cho phép thao tác nào
        displayScreen = "RESET";
      }
    }
  }
}

// Hàm callback khi nút đang được giữ
void btnSetDuringLongPress() {
  //showSetup("Setup", "OFF", "Dang giu nut");
}

void btnUpClick() {
  if (displayScreen == "MENU") {
    if (menuIndex + 1 > 3) {
      menuIndex = 1;  // Khi chỉ số vượt quá giới hạn, quay lại đầu danh sách
    } else {
      menuIndex++;    // Tăng menuIndex lên 1
    }
    showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
  } else if (displayScreen == "ScreenCD") {
    if (pIndex + 1 > totalChildren) {
      pIndex = 1;
    } else {
      pIndex ++;
    }
    loadJsonSettings(); // Hiển thị giá trị thiết lập
  } else if (displayScreen == "ScreenEdit") {
    if (keyStr == "CD"){
      editValue("addition");
      log("Value:" + valueStr);
    } else if (keyStr == "CN") {
      editValue("addition");
      log("Value:" + valueStr);
    }
  } else if (displayScreen == "testOutput"){
    if (testOutputStep == maxTestOutputStep){
      testOutputStep = 0;
      hienThiTestOutput = true;
    } else {
      testOutputStep ++;
      hienThiTestOutput = true;
    }
  } else if (displayScreen == "screenTestMode"){
    if (testModeStep < maxTestModeStep){
      testModeStep ++;
    } else {
      testModeStep = 0;
    }
    chayTestMode = true;
    showText("TEST MODE", String("Step " + String(testModeStep)).c_str());
  }
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
  if (displayScreen == "MENU") {
    if (menuIndex - 1 < 1) {
      menuIndex = 3;  // Khi chỉ số nhỏ hơn giới hạn, quay lại cuối danh sách
    } else {
      menuIndex--;    // Giảm menuIndex đi 1
    }
    showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
  } else if (displayScreen == "ScreenCD"){
    if (pIndex - 1 < 1) {
      pIndex = totalChildren;
    } else {
      pIndex --;
    }
    loadJsonSettings(); // Hiển thị giá trị thiết lập
  } else if (displayScreen == "ScreenEdit"){
    if (keyStr == "CD"){
      editValue("subtraction");
      log("Value:" + valueStr);
    } else if (keyStr == "CN"){
      editValue("subtraction");
      log("Value:" + valueStr);
    }
  } else if (displayScreen == "testOutput"){
    if (testOutputStep == 0){
      testOutputStep = maxTestOutputStep;
      hienThiTestOutput = true;
    } else {
      testOutputStep --;
      hienThiTestOutput = true;
    }
  } else if (displayScreen == "screenTestMode"){
    if (testModeStep > 0){
      /*testModeStep --;
      chayTestMode = true;
      showText("TEST MODE", String("Step " + String(testModeStep)).c_str());*/
    }
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnDownLongPressStart() {
  trangThaiHoatDong = 199;
  //Serial.println("Button Long Press Started (btnDown)");
}

// Hàm callback khi nút đang được giữ
void btnDownDuringLongPress() {
  //Serial.println("Button is being Long Pressed (btnDown)");
}

//KHAI BÁO CHÂN IO Ở ĐÂY

const int sensorKep = 36;
const int sensorDao = 39;
const int sensorViTriCang = 34;
const int sensorTayKep = 33;


const int outRelayXoay = 4;
const int outRelayKep = 16;
const int outRelayTayKep= 17;

//KHAI BÁO THÔNG SỐ TRƯƠNG TRÌNH

int timeDelayXoay = 1000;
int timeDelayChan = 1000;
int timeDelayTraLai = 1000;
int timeDelayKep = 1000;
int timeDelayNhaKep = 1000;
bool trangThaiCuoiCungCamBienViTri = true ;



//TRƯƠNG TRÌNH NGƯỜI DÙNG LẬP TRÌNH


void testMode(){
  switch (testModeStep){
  case 0:
    if(chayTestMode){
      maxTestModeStep = 2;
      chayTestMode = false;
    }
    break;

  case 1:
    if (chayTestMode){
      /* code */
    }
    break;
  case 2:
    /* code */
    break;
  default:
    /* code */
    break;
  }
}

void testInput(){
  static bool trangthaiCuoiIO1;
  if (digitalRead(sensorKep)!= trangthaiCuoiIO1){
    trangthaiCuoiIO1 = digitalRead(sensorKep);
    showText("IO 36" , String(trangthaiCuoiIO1).c_str());
  }
  static bool trangthaiCuoiIO2;
  if (digitalRead(sensorDao)!= trangthaiCuoiIO2){
    trangthaiCuoiIO2 = digitalRead(sensorDao);
    showText("IO 39" , String(trangthaiCuoiIO2).c_str());
  }
  static bool trangthaiCuoiIO3;
  if (digitalRead(sensorViTriCang)!= trangthaiCuoiIO3){
    trangthaiCuoiIO3 = digitalRead(sensorViTriCang);
    showText("IO 34" , String(trangthaiCuoiIO3).c_str());
  }
  static bool trangthaiCuoiIO4;
  if (digitalRead(sensorTayKep)!= trangthaiCuoiIO4){
    trangthaiCuoiIO4 = digitalRead(sensorTayKep);
    showText("IO 33" , String(trangthaiCuoiIO4).c_str());
  }
}
void testOutput(){
  switch (testOutputStep){
    case 0:
      if (hienThiTestOutput){
        maxTestOutputStep = 2;
        bool tinHieuHienTai = digitalRead(outRelayXoay);
        showText("IO 4", String(tinHieuHienTai).c_str());
        hienThiTestOutput = false;
      } else if (daoTinHieuOutput){
        bool tinHieuHienTai = digitalRead(outRelayXoay);
        digitalWrite(outRelayXoay,!tinHieuHienTai);
        hienThiTestOutput = true;
        daoTinHieuOutput = false;
      }
      break;
    case 1:
      if (hienThiTestOutput){
        bool tinHieuHienTai = digitalRead(outRelayKep);
        showText("IO 16", String(tinHieuHienTai).c_str());
        hienThiTestOutput = false;
      } else if (daoTinHieuOutput){
        bool tinHieuHienTai = digitalRead(outRelayKep);
        digitalWrite(outRelayKep,!tinHieuHienTai);
        hienThiTestOutput = true;
        daoTinHieuOutput = false;
      }
      break;
    case 2:
      if (hienThiTestOutput){
        bool tinHieuHienTai = digitalRead(outRelayTayKep);
        showText("IO 17", String(tinHieuHienTai).c_str());
        hienThiTestOutput = false;
      } else if (daoTinHieuOutput){
        bool tinHieuHienTai = digitalRead(outRelayTayKep);
        digitalWrite(outRelayTayKep,!tinHieuHienTai);
        hienThiTestOutput = true;
        daoTinHieuOutput = false;
      }
      break;
    default:
      break;
  }
}


void tinhToanCaiDat(){
  
}

void loadSetup(){
  timeDelayXoay = jsonDoc["main"]["main1"]["children"]["CD1"]["configuredValue"];
  timeDelayChan = jsonDoc["main"]["main1"]["children"]["CD2"]["configuredValue"];
  timeDelayTraLai = jsonDoc["main"]["main1"]["children"]["CD3"]["configuredValue"];
  timeDelayKep = jsonDoc["main"]["main1"]["children"]["CD4"]["configuredValue"];
  timeDelayNhaKep = jsonDoc["main"]["main1"]["children"]["CD5"]["configuredValue"];
}

void khoiDong(){
  delay(500);
  displayScreen = "index";
  showText("HELLO","Xin Chào");
  mainStep = 0;
  trangThaiHoatDong = 0;
  loadSetup();
  tinhToanCaiDat();
  trangThaiHoatDong = 1;
}

void mainRun(){
  bool trangThaiHienTaiCamBienViTri = digitalRead(sensorViTriCang);
  if (trangThaiCuoiCungCamBienViTri != trangThaiHienTaiCamBienViTri){
    if(trangThaiHienTaiCamBienViTri){
      digitalWrite(outRelayXoay,LOW);
      delay(timeDelayNhaKep);
      digitalWrite(outRelayKep,LOW);
      digitalWrite(outRelayTayKep,LOW);
      trangThaiHoatDong = 1;
      mainStep = 0;
    } else if (trangThaiCuoiCungCamBienViTri != trangThaiHienTaiCamBienViTri && !digitalRead(sensorKep)) {
      delay(timeDelayKep);
      digitalWrite(outRelayKep,HIGH);
    }
    trangThaiCuoiCungCamBienViTri = trangThaiHienTaiCamBienViTri ;
  }
  switch (mainStep){
  case 0:
    
    break;
  case 1:
    /* code */
    if (digitalRead(sensorKep)){   
      digitalWrite(outRelayKep,HIGH);
      delay(timeDelayXoay);
      digitalWrite(outRelayXoay,HIGH);
      mainStep++;
    }
    break;
  
  case 2:
    if(digitalRead(sensorTayKep)){
      digitalWrite(outRelayTayKep,HIGH);
      mainStep++;
    }
    break;
  case 3:
    if (digitalRead(sensorDao)){
      digitalWrite(outRelayKep,LOW);
      delay(timeDelayTraLai);
      digitalWrite(outRelayKep,HIGH);
      delay(70);
      digitalWrite(outRelayXoay,LOW);
    }
    break;
  default:
    break;
  }
}



void setup() {

  Serial.begin(115200);     // Khởi tạo Serial và màn hình

  u8g2.begin();  // Khởi tạo màn hình OLED
  u8g2.enableUTF8Print(); // Kích hoạt hỗ trợ UTF-8

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

  pinMode(sensorKep,INPUT);
  pinMode(sensorDao,INPUT);
  pinMode(sensorViTriCang,INPUT);
  pinMode(sensorTayKep,INPUT);


  pinMode(outRelayKep,OUTPUT);
  pinMode(outRelayXoay,OUTPUT);
  pinMode(outRelayTayKep,OUTPUT);


  if (!LittleFS.begin()) {
    showSetup("Error", "E003", "LittleFS Mount Failed");
    Serial.println("LittleFS Mount Failed");
    return;
  }

  

  // Kiểm tra xem file config.json có tồn tại không
  if (!LittleFS.exists(configFile)) {
    DeserializationError error = deserializeJson(jsonDoc, jsonString); // Phân tích chuỗi JSON
    if (error) {
        showSetup("Error", "E005", "JsonString Error");
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    showSetup("Error", "E007", "JsonString Load Mode");
    Serial.println("Read Data From JsonString");
    loadSetup();
    Serial.println("File config.json does not exist.");
    return;
  }

  readConfigFile();

  Serial.println("Load toàn bộ dữ liệu thành công");
  khoiDong();
}

void loop() {
  switch (trangThaiHoatDong){
  case 0:
    btnMenu.tick();
    btnSet.tick();
    btnUp.tick();
    btnDown.tick();
    break;
  case 1:
    btnMenu.tick();
    btnDown.tick();
    if (digitalRead(sensorKep)){
      mainStep = 1;
      trangThaiHoatDong = 2;
    }
    break;
  case 2:
    btnDown.tick();
    mainRun();
    break;
  case 199:   //về gốc
    digitalWrite(outRelayKep,HIGH);
    delay(1000);
    digitalWrite(outRelayXoay,LOW);
    delay(1000);
    digitalWrite(outRelayKep,LOW);
    digitalWrite(outRelayTayKep,LOW);
    mainStep = 0;
    trangThaiHoatDong = 1;
  case 200:        //ESTOP dừng khẩn cấp
    btnMenu.tick();
    break;
  case 201:         // Func Test Mode
    btnMenu.tick();
    btnUp.tick();
    btnDown.tick();
    testMode();
    break;
  case 202:        // Func Test Input
    btnMenu.tick();
    testInput();
    break;
  case 203:      // Func Test Output
    btnMenu.tick();
    btnSet.tick();
    btnUp.tick();
    btnDown.tick();
    testOutput();
    break;
  case 204:
    handleOTA(); // Xử lý OTA khi điều kiện đúng
    break;  
  default:
    break;
  }
}
