#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
//jegliche Benutzen Bibliotheken einbinden

TMRpcm audio;
RTC_DS3231 rtc;
SSD1306AsciiWire oled;
//initialisiert Audioaufnahme, die Echtzeituhr und das OLED-Display

const int mic_pin = A8;
const int speaker_pin = 9;
const int sample_rate = 16000;
const int button_1 = 4;
const int button_2 = 2;

int mic_mode = 0;
char file_name[50] = "rec.wav";
//Variablen für Aufnahme und Wiedergabe

int taster_plus = A2;
int taster_minus = A0;
int taster_weiter = A1;


int angehzeit_h = 0;
int angehzeit_min = 0;
int angehdatum_y = 2024;
int angehdatum_m = 11;
int angehdatum_d = 4;
int modul = 1;
String angehzeit = "";
String angehdatum = "";

//Variablen für die Zeiteinheit


#define SD_CSPin 48
#define I2C_ADDRESS 0x3C

void wait_min(int mins) {
  int count = 0;
  int secs = mins * 60;
  while (1) {
    Serial.print('.');
    delay(1000);
    count++;
    if (count == secs) {
      count = 0;
      break;
    }
  }
  Serial.println();
  return ;
} //Eine wartefunktion für Minuten statt Millisekunden

int start_rec() {
  if (SD.exists(file_name) == true){
    SD.remove(file_name);
  } //Es gibt immer nur eine Audiodatei gleichzeitig, ergo wird die alte gelöscht, wenn eine neue gemacht werden soll
  audio.startRecording(file_name, sample_rate, mic_pin); //startet Aufnahme
  Serial.println("Start Recording");
  mic_mode = 1; //Mikrophon im Modus 1 bedeutet, dass aufgenommen wird
  return 1;
}

int stop_rec() {
  audio.stopRecording(file_name); //stoppt die Aufnahme
  Serial.println("Stop Recording");
  mic_mode = 0; //Mikrophon im Modus 0 bedeutet, dass nicht aufgenommen wird
  return 0;
}

void play_rec() {
  audio.play(file_name); //spielt aufnahme ab
  while (audio.isPlaying() == 1) {
      delay(1); 
    } //Während Audio spielt, wird pausiert
}

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
}// erneuert die Anzeige auf dem OLED-Display mit aktuellen Angaben

void setup() {
  // put your setup code here, to run once:
  //initialises the serial connection between the arduino and any connected serial device(e.g. computer, phone, raspberry pi...)
  Serial.begin(9600);
  audio.speakerPin = speaker_pin;
  //Sets up the pins
  pinMode(mic_pin, INPUT);
  pinMode(button_1, INPUT);
  pinMode(button_2, INPUT);
  pinMode(speaker_pin, OUTPUT);
  Serial.println("loading... SD card");
  if (!SD.begin(SD_CSPin)) {
    Serial.println("An Error has occurred while mounting SD");
  }
  while (!SD.begin(SD_CSPin)) {
    Serial.print(".");
    delay(500);
  }
  audio.CSPin = SD_CSPin;

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
} //kein tatsächlich spannender Code, ungefähr alle Module werden einmal initialisiert und zur benutzung vorbereitet

void loop() {
  if (digitalRead(button_1) == HIGH) {
    if (mic_mode == 0) {
      start_rec();
    } else if (mic_mode == 1){
      stop_rec();
    } else {
      Serial.println("Unknown Microphone Mode");
    }
  } //Knopf 1 wird gedrückt. Wenn nicht aufgenommen wird, wird aufnahme gestartet, wenn aufgenommen wird, wird gestoppt (Funktioniert über den Mikrophonmodus)

  if (digitalRead(button_2) == HIGH) {
    play_rec();
  } //Spielt aufnahme ab. 
  
  Serial.print("Mic Mode: ");
  Serial.println(mic_mode);
    //Weckerzeit
  angehzeit = String(angehzeit_h) + ":" + String(angehzeit_min) + ":00";
  angehdatum = String(angehdatum_d) + "." + String(angehdatum_m) + "." + String(angehdatum_y);
  
  //Zeit/Datum aktuallisieren
  DateTime aktuell = rtc.now(); // rtc.now() -> aktuelle Zeit holen
  char Datum[] = "DD.MM.YYYY"; //Datum speichern
  char Zeit[] = "hh:mm:ss"; // Zeit speichern
  aktuell.toString(Datum);
  aktuell.toString(Zeit);
  Serial.println(Datum);
  Serial.println(Zeit);
  
  //Alarm
  //if (String(Datum) == angehdatum && String(Zeit) == angehzeit)
  //{
  if (aktuell.year() == angehdatum_y &&
     aktuell.month() == angehdatum_m &&
     aktuell.day() == angehdatum_d &&
     aktuell.hour() == angehzeit_h &&
     aktuell.minute() == angehzeit_min &&
     aktuell.second() == 00) {
    //Hier eingeben was beim Alarm passieren soll
    Serial.println("ALARM! ALAAAAAAARM!");
    play_rec();
    
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
    } // diese große funktion geht nur mit unterschiedlichen Monatslängen und Schaltjahren um.
    refresh();
  }
 //Ende
 delay(500);
}
