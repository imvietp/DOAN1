#include <Wire.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
//khai báo chân
SoftwareSerial bluetooth(A0, A1); //Tx hc06 nối rx-A0; Rx hc06 nối tx-A1
#define ECHO_PIN 2         //CHÂN ĐẦU VÀO CẢM BIẾN
#define TRIGGER_PIN 3      //CHÂN ĐẦU RA CẢM BIẾN
#define MOTOR_PIN1 4       //RELAY KICH BOM
#define MANUAL_MODE_PIN 1  //SLIDE CHUYỂN TRANG THÁI
#define MOTOR_PIN2 5       //RELAY KICH XA
#define ledpump A2         //LED NÚT BƠM
#define ledval A3          //LED NÚT XẢ
#define ledauto A4         //LED CHẾ ĐỘ TỰ ĐỘNG
#define ledmanual A5       //LED CHẾ ĐỘ TỰ CHỈNH
int BUTTON_PIN = 6;        //NUT BOM
int BUTTON_PIN1 = 7;       //NUT XA

//khai báo biến  
int buttonStatepump = 0; 
int buttonState = 0; // initial state of the button
bool sta = 0; 
char kytu;
String chuoi; 
int n=0;
int temp = 0;
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//thiết lập ngõ vào và ngõ ra
void setup() {
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MANUAL_MODE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUTTON_PIN1, INPUT);
//  pinMode(PUMP_MODE_PIN, INPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(ledpump, OUTPUT);
  pinMode(ledval, OUTPUT);
  pinMode(ledauto, OUTPUT);
  pinMode(ledmanual, OUTPUT);
  
  Serial.begin(9600);
  bluetooth.begin(9600);
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  //
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
  attachInterrupt(digitalPinToInterrupt(MANUAL_MODE_PIN),nutbom,RISING);
}


 // do muocnuoc
int getWaterLevel() {
  long duration;
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  return (15-((duration / 2) / 29));
}

void manualmode()
{
nutbom();
nutxa(); 

}
// ham nut bom
void nutbom() {
  //bool sta = 0; 
  if(digitalRead(BUTTON_PIN)==0)
  {
    delay(20);
    if(digitalRead(BUTTON_PIN)==0)
    {
    sta =!sta;
    digitalWrite(MOTOR_PIN1, sta);
    digitalWrite(ledpump, sta);
    }
   
    while(digitalRead(BUTTON_PIN)==0);
  }
    }
// ham nut bom
void nutxa() {
  //bool sta = 0; 
  if(digitalRead(BUTTON_PIN1)==0)
  {
    delay(20);
    if(digitalRead(BUTTON_PIN1)==0)
    {
    temp =!temp;
    digitalWrite(MOTOR_PIN2, temp);
    digitalWrite(ledval, temp);
    }
   
    while(digitalRead(BUTTON_PIN1)==0);
  }
    }
//hàm auto
void automode(){
  int waterLevel = getWaterLevel();
    if (waterLevel > 10) 
      {
      digitalWrite(MOTOR_PIN1, HIGH);
     
      digitalWrite(MOTOR_PIN2, LOW);
      
      }
    
    else if(waterLevel >= 6 & waterLevel <= 10  )
         {
           digitalWrite(MOTOR_PIN1, LOW);
           
           digitalWrite(MOTOR_PIN2, LOW);
           
         }
    else if(waterLevel < 6 )
         {
          
           digitalWrite(MOTOR_PIN2, HIGH);
         
           digitalWrite(MOTOR_PIN1, LOW);
          
         }
         
}

//ham chính
void loop() {
buttonState = digitalRead(MANUAL_MODE_PIN); // read the button state

bool stat= 0;
if (buttonState == 0) { // if the button is pressed
 //int waterLevel = getWaterLevel();
 automode();
 digitalWrite(ledauto, HIGH);
 digitalWrite(ledmanual, LOW);
 digitalWrite(ledpump, LOW);
 digitalWrite(ledval, LOW);
}
 
   else if (buttonState == 1) {

    digitalWrite(ledmanual, HIGH);
    digitalWrite(ledauto, LOW);
    do
    {
    manualmode();
    }
    while(buttonState == 0);
   }
   int waterLevel = getWaterLevel();
bluetooth.print(waterLevel);
bluetooth.print(";");
// Serial.println(waterLevel);

//nhận tín hiệu của bluetooth
if (bluetooth.available()) {
    kytu=bluetooth.read();
    chuoi=chuoi+kytu;
    Serial.write(kytu);
    
        
        if(chuoi.indexOf("t")>=0) {
          while(chuoi.indexOf("h")<0) {
            automode();
            kytu=bluetooth.read();
            chuoi=chuoi+kytu;            
          }
        digitalWrite(MOTOR_PIN1,LOW);
        digitalWrite(MOTOR_PIN2,LOW);
        chuoi="";
        }       
        if(chuoi.indexOf("x")>=0){
        digitalWrite(MOTOR_PIN1,HIGH);
        chuoi="";
        }
        if(chuoi.indexOf("y")>=0){
        digitalWrite(MOTOR_PIN1,LOW);
        chuoi="";
        }
        if(chuoi.indexOf("a")>=0){
        digitalWrite(MOTOR_PIN2,HIGH);
        chuoi="";
        }
        if(chuoi.indexOf("b")>=0){
        digitalWrite(MOTOR_PIN2,LOW);
        chuoi="";
        }        
  }

  updateDisplay();
   

}



//CẬP NHẬT LCD1602
void updateDisplay() {

  lcd.setCursor(7,0);  //Set cursor to first column of second row
  lcd.print("                "); //Print blanks to clear the row
  lcd.setCursor(7, 0);
  lcd.print(getWaterLevel());
  lcd.print("cm");
  delay(500);
  lcd.setCursor(0, 1);
  if (buttonState == LOW) {
    lcd.print("                "); //Print blanks to clear the row
    lcd.setCursor(0, 1);
    lcd.print("Mode: Auto");
    
  }
  else if (buttonState == HIGH){
    lcd.print("                "); //Print blanks to clear the row
    lcd.setCursor(0, 1);
    lcd.print("Mode: Manual");
    
  }
}
