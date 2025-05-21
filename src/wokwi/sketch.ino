#include <Arduino.h>
#include "DHT.h"

// Definição dos pinos
#define DHTPIN 15          
#define DHTTYPE DHT22

#define BUTTON_P 4         
#define BUTTON_K 5         
#define LDR_PIN 34         
#define RELAY_PIN 18       
#define LED_PIN 2          

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_P, INPUT_PULLUP);
  pinMode(BUTTON_K, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  dht.begin();
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Leitura dos botões (Fósforo e Potássio)
  bool fosforo = !digitalRead(BUTTON_P); // Pressionado = presente
  bool potassio = !digitalRead(BUTTON_K);

  // Leitura do LDR (simulando pH)
  int ldrValue = analogRead(LDR_PIN);
  float pH = map(ldrValue, 0, 4095, 0, 14);

  // Leitura do DHT22 (umidade do solo)
  float umidade = dht.readHumidity();

  // CONDIÇÕES SEPARADAS:
  bool falta_nutriente = (!fosforo || !potassio); // Fósforo ou potássio ausente
  bool solo_seco = (umidade < 40.0);              // Umidade < 40%
  bool ph_ruim = (pH < 5.5 || pH > 7.5);          // pH fora do ideal

  // Lógica: bomba liga se QUALQUER condição for verdadeira
  bool bombaOn = (falta_nutriente || solo_seco || ph_ruim);

  digitalWrite(RELAY_PIN, bombaOn ? HIGH : LOW);
  digitalWrite(LED_PIN, bombaOn ? HIGH : LOW);

  // Printando tudo pra monitorar
  Serial.print("Fosforo: ");
  Serial.print(fosforo ? "PRESENTE" : "AUSENTE");
  Serial.print(" | Potassio: ");
  Serial.print(potassio ? "PRESENTE" : "AUSENTE");
  Serial.print(" | pH: ");
  Serial.print(pH, 2);
  Serial.print(" | Umidade: ");
  Serial.print(umidade, 1);
  Serial.print("% | Bomba: ");
  Serial.print(bombaOn ? "LIGADA" : "DESLIGADA");

  // Motivo da bomba estar ligada
  if (bombaOn) {
    Serial.print(" | Motivo: ");
    if (falta_nutriente) Serial.print("[Nutriente ausente] ");
    if (solo_seco) Serial.print("[Solo seco] ");
    if (ph_ruim) Serial.print("[pH fora do ideal]");
  }
  Serial.println();

  delay(2000);
}
