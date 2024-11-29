#include <USBComposite.h>

// Pin tanımları (6 buton için)
const int switchPins[6] = {PA4, PA5, PA6, PA7, PB6, PB7}; // 6 buton

// Debounce ayarları
#define debounceTime 50
bool oldSwitchState[6] = {LOW};
bool switchState[6] = {LOW};
unsigned long debounce[6] = {0};

// Potansiyometre pini
const int potPin = PA2;  // STM32'de analog pin olarak PA2 kullanılıyor

// USB Serial ayarları
USBCompositeSerial serial;

// ADC ortalama için örnek sayısı
#define SAMPLE_COUNT 10

void setup() {
  // Pinleri giriş olarak ayarla
  for (int i = 0; i < 6; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
  }

  // USB Serial başlat
  serial.begin();
  USBComposite.begin();
}

void loop() {
  // Buton durumlarını kontrol et
  for (int i = 0; i < 6; i++) {
    switchState[i] = digitalRead(switchPins[i]);

    // Butona basıldıysa ve önceki durumdan farklıysa
    if (switchState[i] == LOW && oldSwitchState[i] == HIGH && (millis() - debounce[i]) > debounceTime) {
      debounce[i] = millis();

      // Seri port üzerinden buton numarasını gönder
      serial.println(i);
    }

    oldSwitchState[i] = switchState[i]; // Önceki durumu güncelle
  }

  // Potansiyometre değerini oku ve ortalama al
  long sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += analogRead(potPin);
    delay(5); // Her okuma arasında kısa bir gecikme
  }
  int potValue = sum / SAMPLE_COUNT; // Ortalama hesapla

  // Potansiyometre değerini 0-255 arası dönüştür
  int volume = map(potValue, 0, 1023, 0, 255);

  // Seri port üzerinden ses seviyesini gönder
  serial.print("Volume: ");
  serial.println(volume);

  delay(100); // 100ms gecikme, bu debounce için yeterli olacaktır
}
