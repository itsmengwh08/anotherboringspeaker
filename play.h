#include "AudioFileSourceID3.h"
#include "AudioFileSourceSD.h"
#include "AudioOutputI2S.h"
#include "AudioGeneratorMP3.h"


#define SD_CS          5  // Chip Select (CS)

// MAX98357A audio configuration
#define I2S_DOUT      22  // DIN (Data)
#define I2S_BCLK      26  // BCLK (Bit Clock)
#define I2S_LRC       25  // LRC (Left/Right Clock)

#define BUTTON_PIN_1  32  
#define BUTTON_PIN_2  33  

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S    *out;
AudioFileSourceID3 *id3;

int lastButtonState1 = LOW;
int lastButtonState2 = LOW;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);

  if (!SD.begin(SD_CS)) {
    Serial.println("No SD card found!");
    return;
  }
  Serial.println("SD card found.");

  out = new AudioOutputI2S(0, I2S_BCLK, I2S_LRC, I2S_DOUT); 
  // out->SetGain(1.0);
  out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
}

void playMusic(const char *filename) {
  if (mp3 && mp3->isRunning()) {
    mp3->stop();
    delete mp3;
    delete id3;
    delete file;
    Serial.println("Stopped.");
  }
  
  Serial.print("Playing file: ");
  Serial.println(filename);

  file = new AudioFileSourceSD(filename);
  id3  = new AudioFileSourceID3(file);
  mp3  = new AudioGeneratorMP3();
  mp3->begin(id3, out);
}

void loop() {
  
  if (mp3 && mp3->isRunning()) {
    if (!mp3->loop()) {
      Serial.println("Played");
      mp3->stop();
      delete mp3;
      delete id3;
      delete file;
      mp3 = NULL;
    }
  } else {
    int reading1 = digitalRead(BUTTON_PIN_1);
    int reading2 = digitalRead(BUTTON_PIN_2);

    
    if (reading1 == LOW && lastButtonState1 == HIGH) {
      playMusic("./divenha.mp3");
    }

    
    if (reading2 == LOW && lastButtonState2 == HIGH) {
      playMusic("./baikatuoitre.mp3");
    }

   
    lastButtonState1 = reading1;
    lastButtonState2 = reading2;
  }
}

void audio_info(const char *info){
    Serial.print("INFO:         ");
    Serial.println(info);
}
void audio_id3data(const char *data){
    Serial.print("ID3 data:     ");
    Serial.println(data);
}

void audio_eof_mp3(const char *info) { Serial.println("EOF MP3"); }
void audio_showstation(const char *info) { Serial.print("Station:      "); Serial.println(info); }
void audio_showstreamtitle(const char *info) { Serial.print("Stream title: "); Serial.println(info); }
