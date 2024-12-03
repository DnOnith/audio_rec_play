//benötigte Bibliothek einbinden
#include <RTClib.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C

// Name des RTC-Moduls (rtc)
RTC_DS3231 rtc;

//Name des OLED-Moduls (oled)
SSD1306AsciiWire oled;

//Variablen für die taster
int taster_plus = A2;
int taster_minus = A0;
int taster_weiter = A1;

//Variablen
int angehzeit_h = 0;
int angehzeit_min = 0;
int angehdatum_y = 2024;
int angehdatum_m = 11;
int angehdatum_d = 4;
int modul = 1;
String angehzeit = "";
String angehdatum = "";

void refresh()
{
  //OLED Anzeige
  oled.clear();
  oled.print("Zeit:");
  oled.print("          ");
  oled.println("Datum:");
  if (angehzeit_h < 10)
    oled.print("0");
  oled.print(angehzeit_h);
  oled.print(":");
  if (angehzeit_min < 10)
    oled.print("0");
  oled.print(angehzeit_min);
  oled.print("      ");
  if (angehdatum_d < 10)
    oled.print("0");
  oled.print(angehdatum_d);
  oled.print(".");
  if (angehdatum_m < 10)
    oled.print("0");
  oled.print(angehdatum_m);
  oled.print(".");
  oled.println(angehdatum_y);
  switch (modul)
  {
    case 1:
      oled.println("(Stunde)");
      break;
    case 2:
      oled.println("(Minute)");
      break;
    case 3:
      oled.println("(Tag)");
      break;
    case 4:
      oled.println("(Monat)");
      break;
    case 5:
      oled.println("(Jahr)");
      break;
  }
}

void setup()
{
  // RTC-Modul Setup
  rtc.begin();
  Serial.begin(9600);
  //rtc.adjust(DateTime(2024, 11, 12, 14, 24, 40)); //Wenn noch nicht konfiguriert (YYYY, MM, DD, h, min, sec)

  //OLED-Modul Setup
  Wire.begin();
  Wire.setClock(400000L);
  oled.begin(&Adafruit128x64, I2C_ADDRESS);

  //Taster Setup
  pinMode(taster_plus, INPUT_PULLUP);
  pinMode(taster_minus, INPUT_PULLUP);
  pinMode(taster_weiter, INPUT_PULLUP);

  //OLED start
  oled.setFont(System5x7); // Auswahl der Schriftart
  refresh();
;}

void loop()
{
  //Weckerzeit
  angehzeit = String(angehzeit_h) + ":" + String(angehzeit_min) + ":00";
  angehdatum = String(angehdatum_d) + "." + String(angehdatum_m) + "." + String(angehdatum_y);
  
  //Zeit/Datum aktuallisieren
  DateTime aktuell = rtc.now(); // rtc.now() -> aktuelle Zeit holen
  char Datum[] = "DD.MM.YYYY"; //Datum speichern
  char Zeit[] = "hh:mm:ss"; // Zeit speichern
  aktuell.toString(Datum);
  aktuell.toString(Zeit);
  
  //Alarm
  if (String(Datum) == angehdatum && String(Zeit) == angehzeit)
  {
    //Hier eingeben was beim Alarm passieren soll
    Serial.println("Alarm! Alaarm!");
  }

  //Wenn taster_weiter gedrückt ist
  if (digitalRead(taster_weiter) == LOW)
    {
      if (modul !=5)
        modul +=1;
      else
        modul = 1;
      refresh();
      
      delay(100);
    }
  //Wenn taster_plus gedrückt ist
  if (digitalRead(taster_plus) == LOW)
  {
    switch (modul)
    {
      case 1:
        if (angehzeit_h == 23)
          angehzeit_h = 0;
        else
          angehzeit_h += 1;
        break;
      case 2:
        if (angehzeit_min == 59)
          angehzeit_min = 0;
        else
          angehzeit_min += 1;
        break;
      case 3:
        if ((angehdatum_y%4 == 0 && angehdatum_y%100 != 0 && angehdatum_m == 2) && angehdatum_d == 29)
          angehdatum_d = 1;
        else if ((angehdatum_y%4 != 0 || angehdatum_y%100 == 0 && angehdatum_m == 2) && angehdatum_d == 28)
          angehdatum_d = 1;
        else if ((angehdatum_m == 1 || angehdatum_m == 3 || angehdatum_m == 5 || angehdatum_m == 7 || angehdatum_m == 8 || angehdatum_m == 10 || angehdatum_m == 12) && angehdatum_d == 31)
          angehdatum_d = 1;
        else if ((angehdatum_m == 4 || angehdatum_m == 6 || angehdatum_m == 9 || angehdatum_m == 11) && angehdatum_d == 30)
          angehdatum_d = 1;
        else
          angehdatum_d += 1;
        break;
      case 4:
        if (angehdatum_m == 12)
          angehdatum_m = 1;
        else
          angehdatum_m += 1;
        if ((angehdatum_y%4 == 0 && angehdatum_y%100 != 0 && angehdatum_m == 2) && angehdatum_d > 29)
          angehdatum_d = 29;
        else if ((angehdatum_y%4 != 0 || angehdatum_y%100 == 0 && angehdatum_m == 2) && angehdatum_d > 28)
          angehdatum_d = 28;
        else if ((angehdatum_m == 4 || angehdatum_m == 6 || angehdatum_m == 9 || angehdatum_m == 11) && angehdatum_d > 30)
          angehdatum_d = 30;
        break;
      case 5:
        angehdatum_y += 1;
        break;
    }
    refresh();
    delay(100);
  }
  //Wenn taster_minus gedrückt ist
  if (digitalRead(taster_minus) == LOW)
  {
    switch (modul)
    {
      case 1:
        if (angehzeit_h == 0)
          angehzeit_h = 23;
        else
          angehzeit_h -= 1;
        break;
      case 2:
        if (angehzeit_min == 0)
          angehzeit_min = 59;
        else
          angehzeit_min -= 1;
        break;
      case 3:
        if (angehdatum_y%4 == 0 && angehdatum_y%100 != 0 && angehdatum_m == 2 && angehdatum_d == 1)
          angehdatum_d = 29;
        else if (angehdatum_y%4 != 0 || angehdatum_y%100 == 0 && angehdatum_m == 2 && angehdatum_d == 1)
          angehdatum_d = 28;
        else if (angehdatum_m == 1 || angehdatum_m == 3 || angehdatum_m == 5 || angehdatum_m == 7 || angehdatum_m == 8 || angehdatum_m == 10 || angehdatum_m == 12 && angehdatum_d == 1)
          angehdatum_d = 31;
        else if (angehdatum_m == 4 || angehdatum_m == 6 || angehdatum_m == 9 || angehdatum_m == 11 && angehdatum_d == 1)
          angehdatum_d = 30;
        else
          angehdatum_d -= 1;
        break;
      case 4:
        if (angehdatum_m == 0)
          angehdatum_m = 12;
        else
          angehdatum_m -= 1;
        break;
      case 5:
        angehdatum_y -= 1;
        break;
    }
    refresh();
    delay(100);
  }
 //Ende
}
