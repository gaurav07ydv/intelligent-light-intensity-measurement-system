#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <DHT.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= Sensors =================
BH1750 bh1750;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

#define DHTPIN   4
#define DHTTYPE  DHT22
DHT dht(DHTPIN, DHTTYPE);

// ================= Pins =================
#define LDR_PIN  34
#define LED_PIN  25

// ================= LED Control =================
bool ledState = false;

// Limit = 100 lux with hysteresis
const float LED_ON_THRESHOLD  = 100.0;
const float LED_OFF_THRESHOLD = 110.0;

// ================= Smoothing =================
const int AVG_SAMPLES = 5;
float luxBuffer[AVG_SAMPLES] = {0};
int luxIndex = 0;
bool luxFilled = false;

// ================= Timing =================
unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 300;

// ======================================================
// DEPLOYMENT MODEL
// calLux = (-0.012158 * ldr) + (3.363431 * temp) + (1.506510 * bh1750) - 105.443679
// ======================================================
float getCalibratedLux(int ldrRaw, float tempC, float bhLux) {
  float calLux = (-0.012158 * ldrRaw) +
                 (3.363431 * tempC) +
                 (1.506510 * bhLux) -
                 105.443679;

  if (calLux < 0) calLux = 0;
  return calLux;
}

// ======================================================
// Moving average smoothing
// ======================================================
float getSmoothedLux(float newLux) {
  luxBuffer[luxIndex] = newLux;
  luxIndex++;

  if (luxIndex >= AVG_SAMPLES) {
    luxIndex = 0;
    luxFilled = true;
  }

  int count = luxFilled ? AVG_SAMPLES : luxIndex;
  if (count <= 0) return newLux;

  float sum = 0;
  for (int i = 0; i < count; i++) {
    sum += luxBuffer[i];
  }

  return sum / count;
}

// ======================================================
// LED control logic
// - If calLux <= 0 -> LED OFF
// - Turn ON below 100
// - Turn OFF above 110
// - Keep previous state in between
// ======================================================
void updateLED(float calLux) {
  if (calLux <= 0 || isnan(calLux)) {
    ledState = false;
  }
  else if (!ledState && calLux < LED_ON_THRESHOLD) {
    ledState = true;
  }
  else if (ledState && calLux > LED_OFF_THRESHOLD) {
    ledState = false;
  }

  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

// ======================================================
// TSL setup
// ======================================================
void configureTSL() {
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin();
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  if (!bh1750.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 not found");
  }

  if (!tsl.begin()) {
    Serial.println("TSL2561 not found");
  } else {
    configureTSL();
  }

  delay(1000);
}

void loop() {
  // ================= Read sensors =================
  int ldrRaw = analogRead(LDR_PIN);
  float tempC = dht.readTemperature();
  float s2Lux = bh1750.readLightLevel();

  sensors_event_t event;
  tsl.getEvent(&event);
  float s1Lux = event.light;

  // ================= Clean invalid values =================
  if (isnan(tempC)) tempC = 0;
  if (isnan(s2Lux) || s2Lux < 0) s2Lux = 0;
  if (isnan(s1Lux) || s1Lux < 0) s1Lux = 0;

  // ================= Deployment model =================
  float calLux = getCalibratedLux(ldrRaw, tempC, s2Lux);

  // ================= Smooth lux =================
  float smoothCalLux = getSmoothedLux(calLux);

  // ================= LED update =================
  updateLED(smoothCalLux);

  // ================= Serial monitor =================
  Serial.print("LDR: "); Serial.print(ldrRaw);
  Serial.print(" | TEMP: "); Serial.print(tempC, 1);
  Serial.print(" | S1: "); Serial.print(s1Lux, 1);
  Serial.print(" | S2: "); Serial.print(s2Lux, 1);
  Serial.print(" | CAL_LUX: "); Serial.print(smoothCalLux, 1);
  Serial.print(" | LIMIT: 100");
  Serial.print(" | LED_STATUS: "); Serial.println(ledState ? "ON" : "OFF");

  // ================= OLED =================
  if (millis() - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = millis();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("LDR:");
    display.print(ldrRaw);

    display.setCursor(0, 10);
    display.print("S1:");
    display.print(s1Lux, 0);
    display.print(" S2:");
    display.print(s2Lux, 0);

    display.setCursor(0, 20);
    display.print("TEMP:");
    display.print(tempC, 1);
    display.print("C");

    display.setCursor(0, 30);
    display.print("CAL_LUX:");
    display.print(smoothCalLux, 1);

    display.setCursor(0, 40);
    display.print("LIMIT:100");

    display.setCursor(0, 50);
    display.print("LED:");
    display.print(ledState ? "ON" : "OFF");

    display.display();
  }

  delay(150);
}
