/*--------------------------------------------------------------------------------------
ver.1 - initial version by Nicu FLORICA (niq_ro) to display a clock with RTC (DS1307/DS3231)
ver.2 - added data on screen
ver.3 - added manual adjust using 2 buttons
ver.3a - big characters
ver.3a2 - 'beat' seconds small
ver.3b - change characters (thin or bold)
ver.3c - changed DMD library with DMD2 library to be added brigness control
ver.3d - replaced fat font from Arial_black_16 to Font_6x14
ver.4 - added 3rd button for brightnees control and also to decrease values in menu
ver.5 - added DHT22 sensor
--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
#      Installation:                                                                                                                                 #
# Arduino Uno     DS1307/DS3231                                                                                                                         #
#    A5 ---------> SCL                                                                                                                               #
#    A4 ---------> SDA                                                                                                                               #
#    5V ---------> VCC                                                                                                                               #
#   GND ---------> GND                                                                                                                               #
######################################################################################################################################################

######################################################################################################################################################
# DMD.h/DMD2.h - Function and support library for the Freetronics DMD, a 512 LED matrix display panel arranged in a 32 x 16 layout.                         #
# Copyright (C) 2011 Marc Alexander (info <at> freetronics <dot> com) http://www.freetronics.com/dmd and http://www.freetronics.com/dmd2                                              #
#                                                                                                                                                    #
#      Installation:                                                                                                                                 #
# Arduino Uno    P10 Panel                                                                                                                           #
#    13 ---------> S / CLK                                                                                                                           #
#    11 ---------> R                                                                                                                                 #
#     9 ---------> nOE / OE                                                                                                                          #
#     8 ---------> L / SCLK                                                                                                                          #
#     7 ---------> B                                                                                                                                 #
#     6 ---------> A                                                                                                                                 #
#   GND ---------> GND                                                                                                                               #
#                            
--------------------------------------------------------------------------------------*/

#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD2.h>       // https://github.com/freetronics/DMD2/
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial_black_16.h>
#include <fonts/Font_6x14.h> //-> This font only contains numbers from 0-9. Used only to display time, font downloaded from https://www.youtube.com/watch?v=-WQuz01Ml80

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>  // http://tronixstuff.com/2011/03/16/tutorial-your-arduinos-inbuilt-eeprom/
#include "DHT.h"
#define DHTPIN 5     // what pin we're connected to

// Set Width to the number of displays wide you have
const int WIDTH = 1;
SoftDMD dmd(WIDTH,1);  // DMD controls the entire display
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display
int lumina = 5;         // value for brightness (5..255)
int lum = 0;
byte reglaj = 0;  // 0 - normal mode, 1 - adjust the brightness

int x,y;
int pauza = 500;

int ora = 0;
byte ora1 = 0;
byte ora2 = 0;
byte ora10 = 11;
byte ora20 = 11;
int minut = 0;
int minutant = 70;
byte minut1 = 0;
byte minut2 = 0;
byte minut10 = 11;
byte minut20 = 11;
int zi = 0;
byte zi1 = 0;
byte zi2 = 0;
int luna = 0;
byte luna1 = 0;
byte luna2 = 0;
int an = 0;
int an2 = 0;
byte an21 = 0;
byte an22 = 0;

int secunda = 0;
int secundaant = 65;
byte secunda2 = 54;
//byte secunda3 = 40;

int orat = 0;
int minutt = 0;
int zit = 0;
int lunat = luna;
int ant = an;
int maxday = 0;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_DS1307 rtc;  // also work fine with DS3231

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define meniu 2
#define plus  3
#define minus 4

byte stare = 0;  // 0 for clock, etc...
int pauzica = 250; // pause for debounce for buttons

byte tip = 0;   // 0 = thin (Arial14 font)
                // 1 = fat (Arial_black_16 font or best Font_6x14

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

int h;
float t;
int h1,h2,h3;
int tempC1;
int tz, tu, ts;

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{ 
  
  dmd.setBrightness(lumina);
  //dmd.selectFont(Arial_Black_16);
  dmd.selectFont(Font_6x14);
  dmd.begin();
 
   #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
   #endif
  Serial.begin(9600);

  dht.begin();
  
//tip = EEPROM.read(401);
lum = EEPROM.read(402);

if ((lum < 0) or (lum > 16))
{
 EEPROM.put(402,0); // 0 -> 0, 16 -> 255 of 255 PWM (for brightness)
 lum = EEPROM.read(402);
}
  lumina = 16*lum-1;
    if (lum == 0)
  lumina = 5;
 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

pinMode(meniu, INPUT);
pinMode(plus, INPUT);
pinMode(minus, INPUT);
digitalWrite(meniu, HIGH);
digitalWrite(plus, HIGH);
digitalWrite(minus, HIGH);

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
   //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // MANUAL ADJUST

h = dht.readHumidity();
t = dht.readTemperature();
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{

if (stare == 0)
  {
DateTime now = rtc.now();
ora = now.hour(), DEC;
ora1 = ora/10;
ora2 = ora%10;
minut = now.minute(), DEC;
minut1 = minut/10;
minut2 = minut%10;
zi =  now.day(), DEC;
zi1 = zi/10;
zi2 = zi%10;
luna = now.month(), DEC;
luna1 = luna/10;
luna2 = luna%10;
an = now.year(), DEC;
an2 = an - 2000;
an21 = an2/10;
an22 = an2%10;

secunda = now.second(), DEC; 
/*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
 //   Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();   
    Serial.println();
*/
if (secunda < secunda2)
{
if (reglaj == 0)
{
 if (digitalRead(meniu) == LOW)
  {
  delay (pauzica);
  orat = ora;
  stare = 1;
  Serial.print("set mode = ");
  Serial.println(stare);
  } 
if (digitalRead(plus) == LOW)
  {
  delay (pauzica);
  tip = tip+1;
  dmd.clearScreen();   //true is normal (all pixels off), false is negative (all pixels on)
  Serial.print("character = ");
  Serial.println(tip);
   ora10 = 11;
   ora20 = 11;
   minut10 = 11;
   minut20 = 11;
  }
if (digitalRead(minus) == LOW) 
  {
  reglaj = 1;
  //dmd.clearScreen();   //true is normal (all pixels off), false is negative (all pixels on)
  Serial.print("change the brightness = ");
  Serial.println(lumina);
  dmd.drawLine(lum*2,0,lum*2,0);
  delay (pauzica);
  }  
}
  else
  {  
if (digitalRead(plus) == LOW)
  {
  dmd.drawLine(lum*2,0,lum*2,0, GRAPHICS_OFF);
  dmd.drawLine(31,0,31,0, GRAPHICS_OFF);   
  lum = lum+1;
  delay (3*pauzica);
  }
if (digitalRead(minus) == LOW)
  {
  dmd.drawLine(lum*2,0,lum*2,0, GRAPHICS_OFF);
  dmd.drawLine(31,0,31,0, GRAPHICS_OFF);  
  lum = lum-1;
  delay (3*pauzica);
  }
  if (lum < 0)
     lum = 0;
  if (lum >16)
     lum = 16;
  if (lum < 16)
  dmd.drawLine(lum*2,0,lum*2,0);
  else
  dmd.drawLine(31,0,31,0);
  lumina = 16*lum-1;
    if (lum == 0)
     lumina = 5;
  dmd.setBrightness(lumina);  
  }
 if (digitalRead(meniu) == LOW)
  {
  dmd.clearScreen();   //true is normal (all pixels off), false is negative (all pixels on)
  Serial.println("exit in normal mode");
  EEPROM.put(402,lum); // 0 -> 0, 16 -> 255 of 255 PWM (for brightness)
  lum = EEPROM.read(402);
  lumina = 16*lum-1;
    if (lum == 0)
  lumina = 5;  
  delay (pauzica*3);
  reglaj = 0;
  stare = 0;
   ora10 = 11;
   ora20 = 11;
   minut10 = 11;
   minut20 = 11;
  //dmd.clearScreen(); 
  dmd.drawLine(lum*2,0,lum*2,0, GRAPHICS_OFF);
  dmd.drawLine(31,0,31,0, GRAPHICS_OFF);
  } 
  if ((tip%2) == 0)
  {
    dmd.selectFont(Arial14);
  }
  else
  {
  //  dmd.selectFont(Arial_Black_16);
    dmd.selectFont(Font_6x14);
   }
// https://www.asciitable.com/ (48 = '0', ... 57 ='9')
//if (minut != minutant) dmd.clearScreen();
x = 0;  
y = 1;

if (ora1 != ora10)
{
if (ora1 == 0) dmd.drawChar(x,y, ' ');
else
{
dmd.drawChar(x,y, ' ');
dmd.drawChar(x,y, 48+ora1);
}
}
if (ora2 != ora20)
{
dmd.drawChar(x+7,y, ' ');
dmd.drawChar(x+7,y, 48+ora2);
}
if (millis()/1000%2 == 1)
{
dmd.drawLine(x+15,y+2,x+15,y+3);
dmd.drawLine(x+15,y+7,x+15,y+8);
dmd.drawLine(x+15+tip%2,y+2,x+15+tip%2,y+3);
dmd.drawLine(x+15+tip%2,y+7,x+15+tip%2,y+8);
}
else
{
dmd.drawLine(x+15,y+2,x+15,y+3, GRAPHICS_OFF);
dmd.drawLine(x+15,y+7,x+15,y+8, GRAPHICS_OFF);  
dmd.drawLine(x+15+tip%2,y+2,x+15+tip%2,y+3, GRAPHICS_OFF);
dmd.drawLine(x+15+tip%2,y+7,x+15+tip%2,y+8, GRAPHICS_OFF);
}
if (minut1 != minut10)
{
dmd.drawChar(x+18,y, ' ');  
dmd.drawChar(x+18,y, 48+minut1);
}
if (minut2 != minut20)
{
dmd.drawChar(x+25,y, ' '); 
dmd.drawChar(x+25,y, 48+minut2);
}
}
delay(pauza);
minutant = minut;
ora10 = ora1;
ora20 = ora2;
minut10 = minut1;
minut20 = minut2;

if (reglaj == 0)
{
if (secunda == secunda2)
{
  dmd.clearScreen();
  h = dht.readHumidity();
  t = dht.readTemperature();
  h1 = h/100;
  h2 = (h%100)/10;
  h3 = (h%100)%10;
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  tempC1 = (float)(10*abs(t));
        tz = tempC1/100;
        tu = (tempC1%100)/10;
        ts = (tempC1%100)%10;
} 
if (secunda >= secunda2)
{
x = 1;  
y = 1;
 if (digitalRead(meniu) == LOW)
  {
  orat = ora;
  delay (pauzica);
  stare = 1;
  Serial.println(stare);
  }
if (minut%2 == 0)
{
// day and month
//dmd.clearScreen();   //true is normal (all pixels off), false is negative (all pixels on)
dmd.selectFont(SystemFont5x7);
if (zi < 10) dmd.drawChar(x+1,y, '0');
else
dmd.drawChar(x,y, 48+zi1);
dmd.drawChar(x+6,y, 48+zi2);
dmd.drawChar(x+11,y, '.');
if (luna < 10) dmd.drawChar(x+16,y, '0');
else
dmd.drawChar(x+16,y, 48+luna1);
dmd.drawChar(x+22,y, 48+luna2);
dmd.drawChar(x+27,y, '.');
// year
dmd.drawChar(x+4,y+8, '2');
dmd.drawChar(x+10,y+8, '0');
dmd.drawChar(x+16,y+8, 48+an21);
dmd.drawChar(x+22,y+8, 48+an22);
}
else
{
dmd.selectFont(SystemFont5x7);
/*
       if ((t < 0) and (tz != 0))
           dmd.drawChar(x+1,y, ' ');
        if (tz != 0)  
           dmd.drawChar(x+7,y, 48+tz);
        else
        if (t < 0)
           dmd.drawChar(x+7,y, ' ');
        dmd.drawChar(x+13,y, 48+tu);
        dmd.drawChar(x+19,y, '.');
        dmd.drawChar(x+25,y, 48+ts);
        dmd.drawChar(x+31,y, 'C');
*/
        if (tz != 0)  
           dmd.drawChar(x+1,y, 48+tz);
        else
           dmd.drawChar(x+1,y, ' ');
        dmd.drawChar(x+7,y, 48+tu);
        dmd.drawChar(x+12,y, '.');
        dmd.drawChar(x+16,y, 48+ts);
       // dmd.drawChar(x+22,y, char(223));
        dmd.drawCircle(24,3,1);
        dmd.drawChar(x+26,y, 'C');

// humidity
if (h < 100) dmd.drawChar(x+0,y+8, ' ');
else
dmd.drawChar(x+ 0,y+8, 48+h1);
dmd.drawChar(x+ 6,y+8, 48+h2);
dmd.drawChar(x+12,y+8, 48+h3);
dmd.drawChar(x+18,y+8, '%');  
}
//dmd.clearScreen();   //true is normal (all pixels off), false is negative (all pixels on)
}
//if (secunda == secunda2) dmd.clearScreen();
if (secunda == 59)
{ 
dmd.clearScreen();
   ora10 = 11;
   ora20 = 11;
   minut10 = 11;
   minut20 = 11;
}
}
} // stare = 0 - usual clock

// change to hour
if (stare == 1)
{
  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen(); 
  delay(pauzica);
  stare = 2;
  Serial.println(stare);
}

// change hour
if (stare == 2)
{
  dmd.drawChar(x,y, 'h');
  dmd.drawChar(x+5,y, 'o');
  dmd.drawChar(x+10,y, 'u');
  dmd.drawChar(x+15,y, 'r');
  dmd.drawChar(x+20,y, ':');  

if (digitalRead(plus) == LOW)
  {
  orat = orat+1;
  delay (3*pauzica);
  }
if (digitalRead(minus) == LOW)
  {
  orat = orat-1;
  delay (3*pauzica);
  }
if (digitalRead(meniu) == LOW)
  {
  stare = 3;
  delay (pauzica);
  Serial.println(stare);
  delay(pauzica);
  }
 if (orat > 23) orat = 23;
 if (orat < 0) orat = 0;
  if (orat < 10) dmd.drawChar(x+10,y+8, ' ');
  else
  dmd.drawChar(x+10,y+8, 48+orat/10);
  dmd.drawChar(x+16,y+8, 48+orat%10);
} // end change hour

// change to minutes
if (stare == 3)
{
  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen();
  minutt = minut;
  stare = 4;
  Serial.println(stare);
}

// change minutes
if (stare == 4)
{
  dmd.drawChar(x,y, 'm');
  dmd.drawChar(x+5,y, 'i');
  dmd.drawChar(x+10,y, 'n');
  dmd.drawChar(x+15,y, 's');
  dmd.drawChar(x+20,y, ':');  

if (digitalRead(plus) == LOW)
  {
  minutt = minutt+1;
  delay (3*pauzica);
  }
if (digitalRead(minus) == LOW)
  {
  minutt = minutt-1;
  delay (3*pauzica);
  }
if (digitalRead(meniu) == LOW)
  {
  stare = 5;
  delay (pauzica);
  Serial.println(stare);
  }
 if (minutt > 59) minutt = 59;
 if (minutt < 0) minutt = 0;
  if (minutt < 10) dmd.drawChar(x+10,y+8, ' ');
  else
  dmd.drawChar(x+10,y+8, 48+minutt/10);
  dmd.drawChar(x+16,y+8, 48+minutt%10);
} // end change minutes

// change to year
if (stare == 5)
{
  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen();
  ant = an2;
  stare = 6;
  Serial.println(stare);
}

// change years
if (stare == 6)
{
  dmd.drawChar(x,y, 'y');
  dmd.drawChar(x+5,y, 'e');
  dmd.drawChar(x+10,y, 'a');
  dmd.drawChar(x+15,y, 'r');
  dmd.drawChar(x+20,y, ':');  

if (digitalRead(plus) == LOW)
  {
  ant = ant+1;
  delay (3*pauzica);
  }
if (digitalRead(minus) == LOW)
  {
  ant = ant-1;
  delay (3*pauzica);
  }
if (digitalRead(meniu) == LOW)
  {
  stare = 7;
  delay (pauzica);
  Serial.println(stare);
  }
 if (ant > 99) ant = 99;
 if (ant < 25) ant = 25; 
  dmd.drawChar(x,y+8, '2');
  dmd.drawChar(x+6,y+8, '0');  
  dmd.drawChar(x+12,y+8, 48+ant/10);
  dmd.drawChar(x+18,y+8, 48+ant%10);
} // end change years

// change to months
if (stare == 7)
{
  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen();
  lunat = luna;
  stare = 8;
}

// change months
if (stare == 8)
{
  dmd.drawChar(x,y, 'm');
  dmd.drawChar(x+5,y, 'o');
  dmd.drawChar(x+10,y, 'n');
  dmd.drawChar(x+15,y, 't');
  dmd.drawChar(x+20,y, 'h');  

if (digitalRead(plus) == LOW)
  {
  lunat = lunat+1;
  delay (3*pauzica);
  }
if (digitalRead(minus) == LOW)
  {
  lunat = lunat-1;
  delay (3*pauzica);
  }
if (digitalRead(meniu) == LOW)
  {
  stare = 9;
  delay (pauzica);
  Serial.println(stare);
  }
 if (lunat > 12) lunat = 12;
 if (lunat <  1) lunat = 1;
  if (lunat < 10) dmd.drawChar(x+10,y+8, ' ');
  else
  dmd.drawChar(x+10,y+8, 48+lunat/10);
  dmd.drawChar(x+16,y+8, 48+lunat%10);
} // end change minutes

// change to day
if (stare == 9)
{
  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen();
  zit = zi;
  stare = 10;
}

// change days
if (stare == 10)
{
  dmd.drawChar(x,y, 'd');
  dmd.drawChar(x+5,y, 'a');
  dmd.drawChar(x+10,y, 'y');
  dmd.drawChar(x+15,y, ':');
//  dmd.drawChar(x+20,y, 'h');  

if (digitalRead(plus) == LOW)
  {
  zit = zit+1;
  delay (3*pauzica);
  }
if (digitalRead(minus) == LOW)
  {
  zit = zit-1;
  delay (3*pauzica);
  }
if (digitalRead(meniu) == LOW)
  {
  stare = 11;
  delay (pauzica);
  Serial.println(stare);
  }

  if (lunat == 4 || lunat == 5 || lunat == 9 || lunat == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (lunat ==2 && ant%4 == 0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (lunat ==2 && ant % 4 !=0) {
    maxday = 28;
  }
  
 if (zit > maxday) zit = maxday;
 if (zit < 1) zit = 1;
  if (zit < 10) dmd.drawChar(x+10,y+8, ' ');
  else
  dmd.drawChar(x+10,y+8, 48+zit/10);
  dmd.drawChar(x+16,y+8, 48+zit%10);
} // end change days

// change to normal
if (stare == 11)
{
  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen();
   // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    rtc.adjust(DateTime(2000+ant, lunat, zit, orat, minutt, 0));
   ora10 = 11;
   ora20 = 11;
   minut10 = 11;
   minut20 = 11;
  stare = 0;
}

} // end main loop
