#include <ESP32Servo.h>
#include <FastLED.h>
#include "RTClib.h"
#define DATA_PIN2 D8
#define DATA_PIN D9
int year;
int day;
int month;
int hour;
int timeZone = 10;
int moonNum = 0;
int range = 0;
int fill = 0;
int hourCorrect = 0;
int arr[4][3] = {{255,255,0},{204,255,204},{204,255,255},{255,153,51}};
RTC_DS3231 rtc;
Servo myservo;
int servoPin = D7;
#define NUM_LEDS 8
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
void setup() {
  Serial.begin();
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 500, 2500); // attaches the servo on pin 18 to the servo object
  FastLED.addLeds<WS2812,DATA_PIN,GRB>(leds2,NUM_LEDS);
  FastLED.addLeds<WS2812,DATA_PIN2,GRB>(leds,NUM_LEDS);
	FastLED.setBrightness(50);
 if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        //Serial.flush();
        abort();
    }
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  //opened this will synch RTC with computer
    //rtc.adjust(DateTime(2024, 3, 19, 19, 0, 0));
    //if(rtc.lostPower()) {
        // this will adjust to the date and time at compilation
    //    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //}

     
    rtc.disable32K();
    printTime();
    DateTime now = rtc.now();
    year = now.year();
    day = now.day();
    month = now.month();
    hour = now.hour();
    moonNum = moonPhases(year, month, day);
    
   Serial.print("Moon Number: ");
   Serial.println(moonNum);
    

}
  void loop(){
   
	printTime();
  delay(5000);
  Serial.println((char*)moonPhaseText(moonNum));
  showMoon();
  Serial.print("Moon Number: ");
   Serial.println(moonNum);
  myservo.write(range); 
  delay(500);
  }
  void printTime(){//call this to print the current time on RTC
     
    DateTime now = rtc.now();
    char date[] = "DD.MM.YYYY, ";
    Serial.print(now.toString(date));
    char time[] = "hh:mm:ss";
    rtc.now().toString(time);
    Serial.println(time);
}
int moonPhases(int year, int month, int day)
{
    double jd = julianDat(year, month, day);  // calculate Julian Date
  
    float dr = PI / 180.0;
  
    float rd = 1 / dr;
  
    unsigned long meeDT = pow(jd - 2382148, 2) / 41048480 / 86400;
  
    double meeT = (jd + meeDT - 2451545.0) / 36525;
  
    unsigned long meeT2 = pow(meeT, 2);
  
    unsigned long meeT3 = pow(meeT, 3);
  
    double meeD = 297.85 + (445267.1115 * meeT) - (0.0016300 * meeT2) + (meeT3 / 545868);
  
    meeD = proper_ang(meeD) * dr;
  
    double meeM1 = 134.96 + (477198.8676 * meeT) + (0.0089970 * meeT2) + (meeT3 / 69699);
  
    meeM1 = proper_ang(meeM1) * dr;
  
    double meeM = 357.53 + (35999.0503 * meeT);
  
    meeM = proper_ang(meeM) * dr;
  
    double elong = meeD * rd + 6.29 * sin(meeM1);
  
    elong = elong - 2.10 * sin(meeM);
  
    elong = elong + 1.27 * sin(2 * meeD - meeM1);
  
    elong = elong + 0.66 * sin(2 * meeD);
  
    elong = proper_ang(elong);
  
    elong = round(elong);
  
    delay(1000);
  
    double moonNum = ((elong + 6.43) / 360) * 28;
  
    moonNum = floor(moonNum);
  
    if (moonNum == 28)
    {
      moonNum = 0;
    }
  
    return moonNum;
}
double proper_ang(double big)
{
    double tmp = 0;
  
    if (big > 0)
    {
      tmp = big / 360.0;
      tmp = (tmp - floor(tmp)) * 360.0;
    }
    else
    {
      tmp = ceil(abs(big / 360.0));
      tmp = big + tmp * 360.0;
    }
  
    return tmp;
}
double julianDat(int year, int month, int day)
{
  double  zone = -(timeZone * 60 / 1440.0);

  if (month <= 2)
  {
    year -= 1;
    month += 12;
  }
  
  double day2 = day + zone + 0.5;
  double A = floor(year / 100.0);
  double B = 2 - A + floor(A / 4.0);
  double JD = floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day2 + B - 1524.5;

  return JD;
}
char* moonPhaseText(int moonNum)
 { 
    char outputStr[50];
    
    char moonPhase[50] = " new Moon";
    char tempStr[50] = " new Moon";

   
    if ((moonNum > 03) && (moonNum < 11)) sprintf(moonPhase,"%s", " First Quarter");  //moonPhase = " First Quarter";
    if ((moonNum > 10) && (moonNum < 18)) sprintf(moonPhase,"%s", " Full Moon");      //moonPhase = " Full Moon";
    if ((moonNum > 17) && (moonNum < 25)) sprintf(moonPhase,"%s", " Last Quarter");   //moonPhase = " Last Quarter";

    if ((moonNum == 01) || (moonNum == 8) || (moonNum == 15) || (moonNum == 22))  
    {
      sprintf(outputStr,"1 day past %s", moonPhase);
    }
    
    if ((moonNum == 02) || (moonNum == 9) || (moonNum == 16) || (moonNum == 23))
    {
      sprintf(outputStr,"2 days past %s", moonPhase);
    }
    
    if ((moonNum == 03) || (moonNum == 10) || (moonNum == 17) || (moonNum == 24)) 
    {
      sprintf(outputStr,"3 days past %s", moonPhase);
    }
   
    if ((moonNum == 04) || (moonNum == 11) || (moonNum == 18) || (moonNum == 25))
    {
      sprintf(outputStr,"3 days before %s", moonPhase);
    }
    
    if ((moonNum == 05) || (moonNum == 12) || (moonNum == 19) || (moonNum == 26))
    {
      sprintf(outputStr,"2 days before %s", moonPhase);
    }
    
    if ((moonNum == 06) || (moonNum == 13) || (moonNum == 20) || (moonNum == 27)) 
    {
      sprintf(outputStr,"1 day before %s", moonPhase);
    }
    
   Serial.print(outputStr);
    return outputStr;

 }
 void showMoon(){
 DateTime now = rtc.now();
  if ( now.hour() < 19 || now.hour() > 23){
  fill = 1;
  } else {
    hourCorrect = now.hour() - 19;
    
    if(moonNum < 16){
       range = map( moonNum,1,15,0,180);
      fill = 2;
}else {
  range = map(moonNum, 16, 29, 0, 180);
  fill = 3;
}
 range = constrain(range,0,180);     
    }
  	myservo.write(range);    // tell servo to go to position in variable 'pos'
    Serial.print("Range");
    Serial.println(range);
    Serial.print("hourcorrect: ");
    Serial.println(hourCorrect);
		delay(500);
 switch (fill) {

    case 1:
      Serial.print("No lights");
      fill_solid(leds2, 8, CRGB(0,0,0) );
	    fill_solid(leds, 8, CRGB(0,0,0) );
      break;

    case 2:
    Serial.print("LeftLightsLED2s");
      fill_solid(leds2, 8, CRGB(arr[hourCorrect][0],arr[hourCorrect][1], arr[hourCorrect][2]) );
	    fill_solid(leds, 8, CRGB(0,0,0));
      break;
      
    case 3:
      Serial.print("RightLightsLEDs");
      fill_solid(leds2, 8, CRGB(0,0,0) );
	    fill_solid(leds, 8,CRGB(arr[hourCorrect][0],arr[hourCorrect][1], arr[hourCorrect][2])  );
      break;


    default:
      Serial.print("Sensor error");
  }
  FastLED.show();
  delay(1000);
}


