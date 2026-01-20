#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <RF24.h> 

#include "cildemimec-project-1_inferencing.h"

Adafruit_MPU6050 mpu;
RF24 radio(4, 5); 

const byte address[6] = "00001";
int command = 0; 
int oncekiKomut = 0; // Durmayı algılamak için hafıza
const int vibrationPin = 27;

void setup() {
    Serial.begin(115200);
    Serial.println(">>> FREN BILDIRIM MODU <<<");

    pinMode(vibrationPin, OUTPUT);
    digitalWrite(vibrationPin, LOW);

    if (!mpu.begin()) {
        Serial.println("HATA: MPU6050 Yok!");
        while (1) delay(10);
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    if (!radio.begin()) {
        Serial.println("HATA: NRF24L01 Yok!");
        while (1) delay(10);
    }
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_LOW);
    radio.stopListening();
    
    // --- AÇILIŞ TİTREŞİMİ (Sistem Hazır) ---
    digitalWrite(vibrationPin, HIGH); delay(150); digitalWrite(vibrationPin, LOW);
    delay(100);
    digitalWrite(vibrationPin, HIGH); delay(150); digitalWrite(vibrationPin, LOW);
}

void loop() {
    sensors_event_t a, g, temp;
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };
    
    // 1. Veri Topla
    for (size_t i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i += 3) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);
        mpu.getEvent(&a, &g, &temp);
        buffer[i + 0] = a.acceleration.x;
        buffer[i + 1] = a.acceleration.y;
        buffer[i + 2] = a.acceleration.z;
        while (micros() < next_tick) { }
    }

    // 2. Tahmin Et
    signal_t signal;
    numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    ei_impulse_result_t result = { 0 };
    run_classifier(&signal, &result, false);

    String enGucluTahmin = "Idle";
    float enYuksekPuan = 0.0;
    
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (result.classification[ix].value > enYuksekPuan) {
            enYuksekPuan = result.classification[ix].value;
            enGucluTahmin = result.classification[ix].label;
        }
    }

    // 3. Komuta Çevir
    if ( (enGucluTahmin == "ileri" || enGucluTahmin == "Forward" || enGucluTahmin == "forward") && enYuksekPuan > 0.6) command = 1;
    else if ( (enGucluTahmin == "geri" || enGucluTahmin == "Back" || enGucluTahmin == "back") && enYuksekPuan > 0.6) command = 2;
    else if ( (enGucluTahmin == "sol" || enGucluTahmin == "Left" || enGucluTahmin == "left") && enYuksekPuan > 0.6) command = 3;
    else if ( (enGucluTahmin == "sag" || enGucluTahmin == "Right" || enGucluTahmin == "right") && enYuksekPuan > 0.6) command = 4;
    else command = 0;

    // --- 4. AKILLI TİTREŞİM (Sadece DUR komutunda) ---
    
    // Eğer şu anki komut DUR (0) ise VE bir önceki komut hareketliyse (yani yeni durduysan)
    if (command == 0 && oncekiKomut != 0) {
        digitalWrite(vibrationPin, HIGH); 
        delay(150); // Kısa bir fren hissi
        digitalWrite(vibrationPin, LOW);
    }
    
    // Hafızayı güncelle
    oncekiKomut = command;

    // 5. Telsizle Gönder
    radio.write(&command, sizeof(command));
    
    delay(10); 
}