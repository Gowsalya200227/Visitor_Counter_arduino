#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <BlynkTimer.h>

char auth[] ="L6ekDBxLUeMQ1wheW4GWSnoznyuI6Um-"
char ssid[] ="V2027 2"
char pass[] =""

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define IR_SENSOR_ENTRY 5
#define IR_SENSOR_EXIT 4
#define LED_PIN 2  // LED connected to D2

int visitorCount = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;
bool entryFirst = false;
bool exitFirst = false;
bool entryConfirmed = false;
bool exitConfirmed = false;

void setup() {
    pinMode(IR_SENSOR_ENTRY, INPUT);
    pinMode(IR_SENSOR_EXIT, INPUT);
    pinMode(LED_PIN, OUTPUT); // LED pin set as output

    Serial.begin(115200);
    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println("Visitors: 0");
    display.display();
}

void loop() {
    int entryState = digitalRead(IR_SENSOR_ENTRY);
    int exitState = digitalRead(IR_SENSOR_EXIT);
    unsigned long currentTime = millis();
    
    if (entryState == LOW && !entryFirst && (currentTime - lastDebounceTime > debounceDelay)) {
        entryFirst = true;
        lastDebounceTime = currentTime;
    }
    if (exitState == LOW && entryFirst && !entryConfirmed && (currentTime - lastDebounceTime > debounceDelay)) {
        visitorCount++;
        Serial.print("Visitor Entered. Count: ");
        Serial.println(visitorCount);
        updateDisplay();
        entryConfirmed = true;
        entryFirst = false;
        lastDebounceTime = currentTime;
    }
    if (entryState == HIGH && exitState == HIGH) {
        entryConfirmed = false;
    }
    
    if (exitState == LOW && !exitFirst && (currentTime - lastDebounceTime > debounceDelay)) {
        exitFirst = true;
        lastDebounceTime = currentTime;
    }
    if (entryState == LOW && exitFirst && !exitConfirmed && (currentTime - lastDebounceTime > debounceDelay)) {
        visitorCount = max(0, visitorCount - 1);
        Serial.print("Visitor Exited. Count: ");
        Serial.println(visitorCount);
        updateDisplay();
        exitConfirmed = true;
        exitFirst = false;
        lastDebounceTime = currentTime;
    }
    if (exitState == HIGH && entryState == HIGH) {
        entryFirst = false;
        exitFirst = false;
        entryConfirmed = false;
        exitConfirmed = false;
    }

    delay(50);
}

void updateDisplay() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("Visitors: ");
    display.println(visitorCount);
    display.display();

    // LED control
    if (visitorCount >= 1) {
        digitalWrite(LED_PIN, HIGH);  // Turn LED ON
    } else {
        digitalWrite(LED_PIN, LOW);   // Turn LED OFF
    }
}
