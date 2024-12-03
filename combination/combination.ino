#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
TMRpcm audio;
const int mic_pin = A8;
const int speaker_pin = 8;
const int sample_rate = 16000;
const int button_1 = 4;
const int button_2 = 2;

int mic_mode = 0;



char file_name[50] = "rec.wav";

#define SD_CSPin 48
// delay function for with serial log.
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
}

int start_rec() {
  if (SD.exists(file_name) == true){
    SD.remove(file_name);
  }
  audio.startRecording(file_name, sample_rate, mic_pin);
  Serial.println("Start Recording");
  mic_mode = 1;
  return 1;
}

int stop_rec() {
  audio.stopRecording(file_name);
  Serial.println("Stop Recording");
  mic_mode = 0;
  return 0;
}

void play_rec() {
  audio.play(file_name);
}

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
}
 
void loop() {
  if (digitalRead(button_1) == HIGH) {
    if (mic_mode == 0) {
      start_rec();
    } else if (mic_mode == 1){
      stop_rec();
    } else {
      Serial.println("Unknown Microphone Mode");
    }
  }

  if (digitalRead(button_2) == HIGH) {
    play_rec();
    while (audio.isPlaying() == 1) {
      delay(1); 
    }
  }
  
  Serial.print("Mic Mode: ");
  Serial.println(mic_mode);
  delay(500); 
}
