/*--------------------------------------------------------------------------------------
ver.1 - initial version by Nicu FLORICA (niq_ro) to display a clock with RTC (DS1307/DS3231)
ver.2 - add data on screen
ver.3 - add manual adjust using 2 buttons
ver.3a - big characters
ver.3a2 - 'beat' seconds small
ver.3b - change characters (thin or bold)
ver.3c - changed DMD library with DMD2 library to be added brigness control
--------------------------------------------------------------------------------------*/
// changed by Nicu FLORICA (niq_ro) to display a clock with RTC (DS1307/DS3231)

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/
#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD2.h>       // https://github.com/freetronics/DMD2/
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial_black_16.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

// Set Width to the number of displays wide you have
const int WIDTH = 1;
SoftDMD dmd(WIDTH,1);  // DMD controls the entire display
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display
int lumina = 5;         // value for brightness (5..255)

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
#define plus 3

byte stare = 0;  // 0 for clock, etc...
int pauzica = 250; // pause for debounce for buttons

byte tip = 0;   // 0 = thin (Arial14 font)
                // 1 = fat (Arial_black_16 font)

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{

  dmd.setBrightness(lumina);
  dmd.selectFont(Arial_Black_16);
  dmd.begin();
 
   #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
   #endif

  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

pinMode(meniu, INPUT);
pinMode(plus, INPUT);
digitalWrite(meniu, HIGH);
digitalWrite(plus, HIGH);

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
   //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // MANUAL ADJUST
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{

if (stare == 0)
  {
if (digitalRead(meniu) == LOW)
 {
  orat = ora;
  delay (pauzica);
  stare = 1;
  Serial.println(stare);
 }
    
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
//analogWrite(9,secunda*4);
Serial.println(now.second(), DEC);
 
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


if (secunda < secunda2)
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
  
  if ((tip%2) == 0)
  {
    dmd.selectFont(Arial14);
  }
  else
  {
    dmd.selectFont(Arial_Black_16);
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
dmd.drawLine(x+15+tip%2,y+2,x+15+tip%2,y+3);
dmd.drawLine(x+15+tip%2,y+7,x+15+tip%2,y+8);
}
else
{
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

if (secunda == secunda2)
  dmd.clearScreen();
  
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

//delay(5000);
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
if (digitalRead(meniu) == LOW)
  {
  stare = 3;
  delay (pauzica);
  Serial.println(stare);
  delay(pauzica);
  }
 if (orat > 23) orat = 0;
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
if (digitalRead(meniu) == LOW)
  {
  stare = 5;
  delay (pauzica);
  Serial.println(stare);
  }
 if (minutt > 59) minutt = 0;
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
if (digitalRead(meniu) == LOW)
  {
  stare = 7;
  delay (pauzica);
  Serial.println(stare);
  }
 if (ant > 39) ant = 19;
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
if (digitalRead(meniu) == LOW)
  {
  stare = 9;
  delay (pauzica);
  Serial.println(stare);
  }
 if (lunat >12) lunat = 1;
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

 if (zit >maxday) zit = 1;
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
