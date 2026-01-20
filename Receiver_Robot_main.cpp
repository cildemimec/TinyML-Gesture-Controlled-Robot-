#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

RF24 radio(4, 5); 
const byte address[6] = "00001"; // Eldivenle AYNI şifre

// MOTOR PİNLERİ (Senin Bağlantıların)
const int sol_ileri = 27; 
const int sol_geri = 26;
const int sag_ileri = 25; 
const int sag_geri = 33;

int gelenVeri = 0;
unsigned long sonSinyalZamani = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(sol_ileri, OUTPUT); pinMode(sol_geri, OUTPUT);
  pinMode(sag_ileri, OUTPUT); pinMode(sag_geri, OUTPUT);

  if (!radio.begin()) {
    Serial.println("HATA: Telsiz modulu yok!");
    while (1); // Hata varsa kilitle
  }
  
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); // Kulakları aç
  
  Serial.println("ARABA HAZIR! Hizli Mod devrede...");
}

// --- MOTOR HAREKETLERİ ---
void hareket(int komut) {
  if (komut == 1) { // İLERİ
    digitalWrite(sol_ileri, HIGH); digitalWrite(sol_geri, LOW);
    digitalWrite(sag_ileri, HIGH); digitalWrite(sag_geri, LOW);
  }
  else if (komut == 2) { // GERİ
    digitalWrite(sol_ileri, LOW); digitalWrite(sol_geri, HIGH);
    digitalWrite(sag_ileri, LOW); digitalWrite(sag_geri, HIGH);
  }
  else if (komut == 3) { // SOL (Tank Dönüşü)
    digitalWrite(sol_ileri, LOW); digitalWrite(sol_geri, HIGH);
    digitalWrite(sag_ileri, HIGH); digitalWrite(sag_geri, LOW);
  }
  else if (komut == 4) { // SAĞ (Tank Dönüşü)
    digitalWrite(sol_ileri, HIGH); digitalWrite(sol_geri, LOW);
    digitalWrite(sag_ileri, LOW); digitalWrite(sag_geri, HIGH);
  }
  else { // DUR
    digitalWrite(sol_ileri, LOW); digitalWrite(sol_geri, LOW);
    digitalWrite(sag_ileri, LOW); digitalWrite(sag_geri, LOW);
  }
}

void loop() {
  // 1. Telsizden Veri Var mı?
  if (radio.available()) {
    radio.read(&gelenVeri, sizeof(gelenVeri));
    sonSinyalZamani = millis(); // Saati sıfırla
    
    // Anında uygula
    hareket(gelenVeri);
    
    // Debug (İsteğe bağlı, performansı etkilemez)
    // Serial.println(gelenVeri);
  }
  else {
    // 2. GÜVENLİK FRENİ
    // Eğer 150 milisaniye boyunca yeni emir gelmezse arabayı durdur.
    // Bu sayede eldiven kapanırsa araba sonsuza kadar gitmez.
    if (millis() - sonSinyalZamani > 150) {
      hareket(0); // Dur
    }
  }
}