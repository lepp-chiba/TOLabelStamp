#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <M5Stack.h>
#include <string>
#include <cctype>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define LABEL_MODE    0
#define EDIT_MODE     1

#define EDIT_LABEL_MODE     2
#define EDIT_NUMBER_MODE    3
#define EDIT_TIME_MODE      4
#define EDIT_FONTSIZE_MODE  5
#define EMERGENCY_MODE 6

using namespace std;

string label = "LABEL";//default
int filmnumber = 1;
int exptime = 1000;
int fontsize = 1;
int mode = 0;
int editcursol = 2;

uint8_t BatteryLevel;

string LABEL;

char cha = 'A';
bool isButtonPressed = false;
bool isPlusButtonPressed = false;
bool isMinusButtonPressed = false;
int lastTime = 0;

char EM[128];

void flipDisplay() {
  uint8_t* buffer = display.getBuffer();
  // 画面を左右反転
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 64; col++) {
      uint8_t temp = buffer[row * 128 + col];
      buffer[row * 128 + col] = buffer[row * 128 + 127 - col];
      buffer[row * 128 + 127 - col] = temp;
    }
  }
  // 反転した画素データをディスプレイにセット
  display.drawBitmap(0, 0, buffer, 128, 64, BLUE);
}

void testdrawchar(bool test) {
  display.clearDisplay();
  display.setTextSize(fontsize);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  
  int length = LABEL.size() * fontsize;

  if(fontsize == 2){
    int xcursor = 60 - 6*(LABEL.size());
    display.setCursor(xcursor, 26);
  } 
  if(fontsize == 1) {
    int xcursor = 62 - 3*(LABEL.size());
    display.setCursor(xcursor, 30); 
  }   
  display.cp437(true);        

  display.write(LABEL.c_str());
  flipDisplay();
  display.display();
  if(!test)M5.Speaker.beep();
  delay(exptime);
  display.clearDisplay();
  display.display();
  if(!test)M5.Speaker.mute();
}

//引数の次の文字を返す関数
char Char_Convert(char cha){
  char C_cha;
  if(isalpha(int(cha))){
    /*
    if(cha == 'z'){
      C_cha = 'A';//zならAにする
    }else */
    if(cha == 'Z'){
      C_cha = '0';//Zなら0にする
    }else{
      C_cha = cha + 1;//それら以外なら1文字進める
    }
  }else if(isDigit(cha)){
    if(cha == '9'){
      C_cha = ']';//9なら]にする（BackSpace）
    }else{
      C_cha = cha + 1;//9以外なら1進める
    }
  }else if(cha == ']')  {
    C_cha = 'A';
  }
  
  return C_cha;
}

void UI_LABEL_MODE(void){
  
  M5.Lcd.setCursor(0,0);
  M5.Lcd.printf("Label Mode");
  M5.Lcd.setCursor(220,0);
  M5.Lcd.printf("%3d%%\n\n",BatteryLevel);
  M5.Lcd.printf("Label :%s\n",label.c_str());
  M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
  M5.Lcd.printf("Time  :%4d ms\n",exptime);
  M5.Lcd.printf("Size  :%d\n",fontsize);
  M5.Lcd.printf("=> %s",LABEL.c_str());

  M5.Lcd.setCursor(30,210);
  M5.Lcd.printf("MODE");
  M5.Lcd.setCursor(120,210);
  M5.Lcd.printf("LABEL");
  M5.Lcd.setCursor(220,210);
  M5.Lcd.printf("TEST");
}

void UI_EDIT_MODE(int cursol){
  if(cursol == EDIT_LABEL_MODE){
      M5.Lcd.setCursor(0,0);
      M5.Lcd.printf("EDIT Mode     \n\n");

      M5.Lcd.setTextColor(BLACK, WHITE);
      M5.Lcd.printf("Label :%s\n",label.c_str());
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
      M5.Lcd.printf("Time  :%4d ms\n",exptime);
      M5.Lcd.printf("Size  :%d\n",fontsize);
      M5.Lcd.setCursor(30,210);
      M5.Lcd.printf("MODE");
      M5.Lcd.setCursor(120,210);
      M5.Lcd.printf("ENTER");
      M5.Lcd.setCursor(215,210);
      M5.Lcd.printf("  V  ");
  }else if(cursol == EDIT_NUMBER_MODE){
      M5.Lcd.setCursor(0,0);
      M5.Lcd.printf("EDIT Mode     \n\n");

      M5.Lcd.printf("Label :%s\n",label.c_str());
      M5.Lcd.setTextColor(BLACK, WHITE);
      M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.printf("Time  :%4d ms\n",exptime);
      M5.Lcd.printf("Size  :%d\n",fontsize);
      M5.Lcd.setCursor(30,210);
      M5.Lcd.printf("MODE");
      M5.Lcd.setCursor(120,210);
      M5.Lcd.printf("ENTER");
      M5.Lcd.setCursor(215,210);
      M5.Lcd.printf("  V  ");
  }else if(cursol==EDIT_TIME_MODE){
      M5.Lcd.setCursor(0,0);
      M5.Lcd.printf("EDIT Mode     \n\n");

      M5.Lcd.printf("Label :%s\n",label.c_str());
      M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
      M5.Lcd.setTextColor(BLACK, WHITE);
      M5.Lcd.printf("Time  :%4d ms\n",exptime);
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.printf("Size  :%d\n",fontsize);
      M5.Lcd.setCursor(30,210);
      M5.Lcd.printf("MODE");
      M5.Lcd.setCursor(120,210);
      M5.Lcd.printf("ENTER");
      M5.Lcd.setCursor(215,210);
      M5.Lcd.printf("  V  ");
  }else if(cursol == EDIT_FONTSIZE_MODE){
      M5.Lcd.setCursor(0,0);
      M5.Lcd.printf("EDIT Mode     \n\n");

      M5.Lcd.printf("Label :%s\n",label.c_str());
      M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
      M5.Lcd.printf("Time  :%4d ms\n",exptime);
      M5.Lcd.setTextColor(BLACK, WHITE);
      M5.Lcd.printf("Size  :%d\n",fontsize);
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setCursor(30,210);
      M5.Lcd.printf("MODE");
      M5.Lcd.setCursor(120,210);
      M5.Lcd.printf("ENTER");
      M5.Lcd.setCursor(215,210);
      M5.Lcd.printf("  V  ");
  }
}

void UI_LABEL_EDIT_MODE(void){
  M5.Lcd.setCursor(0,0);
  M5.Lcd.printf("LABEL EDIT    \n\n");

  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.printf("Label :%s\n",label.c_str());
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
  M5.Lcd.printf("Time  :%4d ms\n",exptime);
  M5.Lcd.printf("Size  :%d\n",fontsize);
  M5.Lcd.setCursor(30,210);
  M5.Lcd.printf("EXIT");
  M5.Lcd.setCursor(120,210);
  M5.Lcd.printf("ENTER");
  M5.Lcd.setCursor(212,210);
  M5.Lcd.printf("  V  ");
}

void UI_NUMBER_EDIT_MODE(void){
  M5.Lcd.setCursor(0,0);
  M5.Lcd.printf("NUMBER EDIT\n\n");

  M5.Lcd.printf("Label :%s\n",label.c_str());
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("Time  :%4d ms\n",exptime);
  M5.Lcd.printf("Size  :%d\n",fontsize);
  M5.Lcd.setCursor(30,210);
  M5.Lcd.printf("EXIT");
  M5.Lcd.setCursor(120,210);
  M5.Lcd.printf("  -  ");
  M5.Lcd.setCursor(212,210);
  M5.Lcd.printf("  +  ");
}

void UI_TIME_EDIT_MODE(void){
  M5.Lcd.setCursor(0,0);
  M5.Lcd.printf("TIME EDIT\n\n");

  M5.Lcd.printf("Label :%s\n",label.c_str());
  M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.printf("Time  :%4d ms\n",exptime);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("Size  :%d\n",fontsize);
  M5.Lcd.setCursor(30,210);
  M5.Lcd.printf("EXIT");
  M5.Lcd.setCursor(120,210);
  M5.Lcd.printf("  -  ");
  M5.Lcd.setCursor(212,210);
  M5.Lcd.printf("  +  ");
}

void UI_FONTSIZE_EDIT_MODE(void){
  M5.Lcd.setCursor(0,0);
  M5.Lcd.printf("FONTSIZE EDIT\n\n");

  M5.Lcd.printf("Label :%s\n",label.c_str());
  M5.Lcd.printf("FilmNo:%03d\n",filmnumber);
  M5.Lcd.printf("Time  :%4d ms\n",exptime);
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.printf("Size  :%d\n",fontsize);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(30,210);
  M5.Lcd.printf("EXIT");
  M5.Lcd.setCursor(120,210);
  M5.Lcd.printf("  -  ");
  M5.Lcd.setCursor(212,210);
  M5.Lcd.printf("  +  ");
}

void Werining(char* errormessage){
  M5.Lcd.clearDisplay();
  M5.Lcd.display();
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setTextColor(BLACK,WHITE);
  M5.Lcd.setCursor(30,110);
  M5.Lcd.setTextSize(4);
  M5.Lcd.printf("%s",errormessage);

  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(30,210);
  M5.Lcd.printf("Cancel");
  M5.Lcd.setCursor(120,210);
  M5.Lcd.printf(" Yes ");
  M5.Lcd.setCursor(212,210);
  M5.Lcd.printf("  No ");
}


void setup() {
  
  Serial.begin(9600);
  //Wire.begin(5, 4);
  //Wire.beginTransmission(0x3C);
  //Wire.write(0xA1);
  //display.setRotation(2);
  M5.begin(true, false, true);
  M5.Power.begin();
  M5.Speaker.begin();
  M5.Speaker.setVolume(3);
  M5.Speaker.beep();        // ビープ開始
  delay(100);               // 100ms待つ(beep()のデフォルト)
  M5.Speaker.mute();        //　ビープ停止
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(3);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); 
  display.display();
}

void loop() {
  M5.update();  // 本体のボタン状態更新
  
  

  string S = to_string(filmnumber);
  S.insert(0,3-S.size(),'0');
  LABEL = label + "-" + S;
  BatteryLevel = M5.Power.getBatteryLevel();
  //ラベルモード
  if(mode == LABEL_MODE){
    UI_LABEL_MODE();
    //LABEL MODEでのボタン挙動
    if(M5.BtnA.wasPressed()){
      M5.Lcd.clearDisplay();
      M5.Lcd.display();
      mode = EDIT_MODE;
      delay(50);
    }
    if(M5.BtnB.wasPressed()){
      testdrawchar(false);
      filmnumber ++;
    }
    if(M5.BtnC.wasPressed()){
      testdrawchar(true);//test irradiation
    }

  }
 
  //編集モード
  else if(mode == EDIT_MODE){
    UI_EDIT_MODE(editcursol);
    //ラベル編集モードへ
    if(editcursol == EDIT_LABEL_MODE){
      if(M5.BtnA.wasPressed()){
        M5.Lcd.clearDisplay();
        M5.Lcd.display();
        mode = LABEL_MODE;
        delay(50);
      }
      if(M5.BtnB.wasPressed()){
        label = "A";
        cha = 'A';
        M5.Lcd.clearDisplay();
        M5.Lcd.display();
        mode = EDIT_LABEL_MODE;
        delay(50);
      }
      if(M5.BtnC.wasPressed()){
        editcursol=EDIT_NUMBER_MODE;
        delay(50);
      }
    }
    
    //フィルム番号編集モードへ
    else if(editcursol == EDIT_NUMBER_MODE){
      if(M5.BtnA.wasPressed()){
        mode = LABEL_MODE;
        delay(50);
      }
      if(M5.BtnB.wasPressed()){
        M5.Lcd.clearDisplay();
        M5.Lcd.display();
        mode = EDIT_NUMBER_MODE;
        delay(50);
      }
      if(M5.BtnC.wasPressed()){
        editcursol=EDIT_TIME_MODE;
        delay(50);
      }
    }
    
    //照射時間編集モードへ
    else if(editcursol == EDIT_TIME_MODE){
      
      if(M5.BtnA.wasPressed()){
        mode = LABEL_MODE;
        delay(50);
      }
      if(M5.BtnB.wasPressed()){
        M5.Lcd.clearDisplay();
        
        mode = EDIT_TIME_MODE;
        M5.Lcd.display();
        delay(50);
      }
      if(M5.BtnC.wasPressed()){
        editcursol=EDIT_FONTSIZE_MODE;
        delay(50);
      }
    }
    
    //フォントサイズ編集モードへ
    else if(editcursol == EDIT_FONTSIZE_MODE){
      
      if(M5.BtnA.wasPressed()){
        mode = LABEL_MODE;
        delay(50);
      }
      if(M5.BtnB.wasPressed()){
        M5.Lcd.clearDisplay();
        M5.Lcd.display();
        mode = EDIT_FONTSIZE_MODE;
        delay(50);
      }
      if(M5.BtnC.wasPressed()){
        editcursol=EDIT_LABEL_MODE;
        delay(50);
      }
    }
    
  }

  //ラベル編集モード
  else if(mode == EDIT_LABEL_MODE){
    
    UI_LABEL_EDIT_MODE();
    if(M5.BtnA.wasPressed()){
      label.pop_back();
      
      M5.Lcd.clearDisplay();
      M5.Lcd.display();
      mode = EDIT_MODE;
      delay(50);
    }
    if(M5.BtnB.wasPressed()){
      if(cha == ']'){
        label.pop_back();
        label.pop_back();
        M5.Lcd.clearDisplay();
        M5.Lcd.display();
      }
      if((fontsize == 2 && label.size() < 6) || (fontsize == 1 && label.size() < 10)) 
      {
        label = label + 'A';
        M5.Lcd.clearDisplay();
        M5.Lcd.display();
      }else if((fontsize == 2 && label.size()==6)||(fontsize == 1 && label.size() == 10)){
        mode = EDIT_MODE;
        M5.Lcd.clearDisplay();
        M5.Lcd.display();
      }
      cha = 'A';
    }
    if(M5.BtnC.wasPressed()){
      cha = Char_Convert(cha);
      label.back() = cha;
    }
    if(M5.BtnC.pressedFor(500)){
      isButtonPressed = true;
    }
    if(M5.BtnC.wasReleased()){
      isButtonPressed = false;
    }
    if (isButtonPressed) {
      if (millis() - lastTime >= 20) {
        cha = Char_Convert(cha);
        label.back() = cha;
        lastTime = millis();
      }
    }
  }

  //フィルムナンバー編集モード
  else if(mode == EDIT_NUMBER_MODE){
    UI_NUMBER_EDIT_MODE();
    if(M5.BtnA.wasPressed()){
      M5.Lcd.clearDisplay();
      M5.Lcd.display();
      mode = EDIT_MODE;
      delay(50);
    }
    if(M5.BtnC.wasPressed()){
      if(filmnumber < 1000) filmnumber ++;
    }
    if(M5.BtnB.wasPressed()){
      if(filmnumber > 0)  filmnumber --;
    }

    if(M5.BtnB.pressedFor(500)){
      isMinusButtonPressed = true;
    }
    if(M5.BtnB.wasReleased()){
      isMinusButtonPressed = false;
    }
    if (isMinusButtonPressed) {
      if (millis() - lastTime >= 20) {
        if(filmnumber > 0)  filmnumber --;
        lastTime = millis();
      }
    }

    if(M5.BtnC.pressedFor(500)){
      isPlusButtonPressed = true;
    }
    if(M5.BtnC.wasReleased()){
      isPlusButtonPressed = false;
    }
    if (isPlusButtonPressed) {
      if (millis() - lastTime >= 20) {
        if(filmnumber < 1000) filmnumber ++;
        lastTime = millis();
      }
    }


  }

  //照射時間編集モード
  else if(mode == EDIT_TIME_MODE){
    UI_TIME_EDIT_MODE();
    if(M5.BtnA.wasPressed()){
      M5.Lcd.clearDisplay();
      M5.Lcd.display();
      mode = EDIT_MODE;
      delay(50);
    }
    if(M5.BtnC.wasPressed()){
      if(exptime < 9900)  exptime += 100;
    }
    if(M5.BtnB.wasPressed()){
      if(exptime > 100) exptime -= 100;
    }

    if(M5.BtnB.pressedFor(500)){
      isMinusButtonPressed = true;
    }
    if(M5.BtnB.wasReleased()){
      isMinusButtonPressed = false;
    }
    if (isMinusButtonPressed) {
      if (millis() - lastTime >= 20) {
        if(exptime > 100) exptime -= 100;
        lastTime = millis();
      }
    }

    if(M5.BtnC.pressedFor(500)){
      isPlusButtonPressed = true;
    }
    if(M5.BtnC.wasReleased()){
      isPlusButtonPressed = false;
    }
    if (isPlusButtonPressed) {
      if (millis() - lastTime >= 20) {
        if(exptime < 9900)  exptime += 100;
        lastTime = millis();
      }
    }


  }

  //フォントサイズ編集モード
  else if(mode == EDIT_FONTSIZE_MODE){
    UI_FONTSIZE_EDIT_MODE();
    if(M5.BtnA.wasPressed()){
      M5.Lcd.clearDisplay();
      M5.Lcd.display();
      mode = EDIT_MODE;
      delay(50);
    }
    if(M5.BtnC.wasPressed()){
      if(fontsize < 2) fontsize +=1;
    }
    if(M5.BtnB.wasPressed()){
      if(fontsize > 1)  fontsize -=1;
    }

    if(M5.BtnB.pressedFor(500)){
      isMinusButtonPressed = true;
    }
    if(M5.BtnB.wasReleased()){
      isMinusButtonPressed = false;
    }
    if (isMinusButtonPressed) {
      if (millis() - lastTime >= 20) {
        if(fontsize > 1)  fontsize -=1;
        lastTime = millis();
      }
    }

    if(M5.BtnC.pressedFor(500)){
      isPlusButtonPressed = true;
    }
    if(M5.BtnC.wasReleased()){
      isPlusButtonPressed = false;
    }
    if (isPlusButtonPressed) {
      if (millis() - lastTime >= 20) {
        if(fontsize < 2) fontsize +=1;
        lastTime = millis();
      }
    }


  }

}




