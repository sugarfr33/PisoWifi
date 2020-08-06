
//LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
//SD
#include "SdFat.h"
#include "sdios.h"

#include <SPI.h>

// SD card chip select pin
const uint8_t chipSelect = SS;

// file system
SdFat sd;
// open test file
//SdFile rdfile("fgets.txt", O_RDWR);

// print stream
ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------
// store error strings in flash memory
#define error(s) sd.errorHalt(F(s))

int _coinInserted = 0;

#define _anodeLed 8
#define _cathodeLed 7
int _led_on_off = 2000;
int _isOkay = 0;
int _isEndOfLine = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(3, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }
  //Setup_InitSD();

  //makeTestFile();

  //demoFgets();
}

// the loop function runs over and over again forever
void loop() {
  Loop_Lcd();
}

void Loop_Lcd(){
  if(digitalRead(3) == 0)
  {
    _coinInserted = _coinInserted + 5;
    lcd.clear();
    delay(100);
  }
  if(digitalRead(2) == 0) //<-- to view codes after inserted coins
  {
    if(_coinInserted > 0)
      _isOkay = 1;
  }
  if(digitalRead(4) == 0) //<-- reset from the start
  {
    if(_coinInserted > 0)
      InitVariableAfterCodeShow();
  }
  if(_coinInserted == 0){
      lcd.setCursor(2,0);
      lcd.print("SINGKO WIFI");
      lcd.setCursor(0,1);
      lcd.print("-[INSERT COINS]-");
  }else{
    if (_isOkay == 0){
      lcd.setCursor(0,0);
      lcd.print("INSERTED COINS");
      lcd.setCursor(0,1);
      lcd.print("PHP: ");
      lcd.print(_coinInserted);
      
    }else if(_isOkay == 1){
      _isOkay = 3;
      demoFgets();
    }
  }
}

void Setup_LightLED(){
  pinMode(_anodeLed, OUTPUT);
  pinMode(3, INPUT);
}

void Loop_PressBtnLED(){
  if(digitalRead(3) == 0){
    //while(digitalRead(3) == 0);

    digitalWrite(_anodeLed, HIGH);
    
  }
}

void demoFgets() {

  char line[25];
  int c;
  uint32_t pos;
  String str;
  
  if(_coinInserted < 10){
    str = "05";
  }else{
    str=String(_coinInserted);
  }

  char b[3];
  str.toCharArray(b,3);
  
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("VOUCHER CODE");
  lcd.setCursor(0,1);

  // open test file
  SdFile rdfile("fgets.txt", O_RDWR);

  // check for open error
  if (!rdfile.isOpen()) {
    DisplayErrorToLcd(2);
    error("demoFgets");
  }

  rdfile.rewind();
  // read lines from the file to get position
  while (1) {
    pos = rdfile.curPosition();
    if (rdfile.fgets(line, sizeof(line)) < 0) {
      DisplayErrorToLcd(1);
      //error("Line not found");
      break;
    }

    if (strstr(line, b)) { // <= find line that contains "Line C"
      for(int i=2;i<strlen(line)-1;i++){
        lcd.print(line[i]);
      }
      break;
    }

    if (strstr(line, "ENDOFLINE")) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("NO CODES");
      lcd.setCursor(0,1);
      lcd.print("PHP: ");
      _isEndOfLine = 1;
      break;
    }

  }
  

  // rewrite line with 'C'
  if (!rdfile.seekSet(pos)) {
    DisplayErrorToLcd(3);
    error("seekSet");
  }
  
  // replace the text of voucher code
  if(_isEndOfLine == 0){
    if(strlen(line)-1 == 4) {
      rdfile.println("WALA");
    }else if(strlen(line)-1 == 5) {
      rdfile.println("WALAN");
    }else if(strlen(line)-1 == 6) {
      rdfile.println("WALANA");
    }else if(strlen(line)-1 == 7) {
      rdfile.println("WALANAA");
    }else if(strlen(line)-1 == 8) {
      rdfile.println("WALANAAA");
    }else if(strlen(line)-1 == 9) {
      rdfile.println("WALANAAAA");
    }
  }
  

  if(_isEndOfLine == 1){
    lcd.print(_coinInserted);
  }else{
    int voucherTimerShowLimit = 1;
    while (voucherTimerShowLimit <= 99){
      lcd.setCursor(14,0);
      lcd.print(voucherTimerShowLimit);
      if(digitalRead(2) == 0) //<-- back to [please insert coins]
      {
        if(_coinInserted > 0)
          voucherTimerShowLimit = 99;
      }
      delay(1000);
      voucherTimerShowLimit++;
    }
    //Initialize again variable
    InitVariableAfterCodeShow();
  }
  // close so rewrite is not lost
  rdfile.close();
}

void InitVariableAfterCodeShow(){
  _coinInserted = 0;
  _isOkay = 0;
  _isEndOfLine = 0;
  lcd.clear();
}

void DisplayErrorToLcd(int errorcode){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ERROR");
  lcd.setCursor(0,1);
  lcd.print(errorcode);
}
