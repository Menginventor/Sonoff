#include <AntoIO.h>
#define SONOFF_LED_PIN 13 // Low = On
#define SONOFF_RELAY_PIN 12 // High = On
#define SONOFF_SWITCH_PIN 0 // Low = Press


const char *ssid = "true_home2G_392";
const char *pass = "60003162";
const char *user = "Neonomia";
const char *token = "gl2E8qa1iXb073xUPM1wzHBaoI1IY1mLQXk9iSGG";
const char *thing = "LightControl";

AntoIO anto(user, token, thing);
int relay_state = 0;
unsigned long timer = millis();
bool sys_is_con = true;
void setup() {
  // put your setup code here, to run once:
  pinMode(SONOFF_LED_PIN, OUTPUT);
  pinMode(SONOFF_RELAY_PIN, OUTPUT);
  turn_off();
  Serial.begin(9600);
  Serial.println("init...");
  anto_init();
  Serial.println("Sys start");

}
void anto_init() {
  anto.begin(ssid, pass, messageReceived);
  anto.sub("LightSwitch1");
}
void loop() {
  anto.loop();
  switch_read();
  //
  if (millis() - timer >= 1000) {
    while ((millis() - timer >= 1000))timer += 1000;
    status_report();

  }


}
void status_report() {
  bool wifi_con = WiFi.isConnected();
  bool anto_con = anto.isConnected();
  Serial.print(millis());
  Serial.print(" : wifi.isConnected = ");
  Serial.print(wifi_con);
  Serial.print(" :anto.isConnected = ");
  Serial.println(anto_con);
  if (!wifi_con &&sys_is_con ) {
    Serial.println("Connection lost");
    sys_is_con = false;
    return;


  }

  if (wifi_con  &&!sys_is_con ) {
    Serial.println("restarting...");

    ESP.restart();
    return;


  }


}
void switch_read() {
  static int switch_state = 1;
  int switch_read = digitalRead(SONOFF_SWITCH_PIN);
  if (switch_state != switch_read) {

    switch_state = switch_read;
    if (switch_state == 1) {
      Serial.println("SW Release");
      relay_state = !relay_state;
      if(relay_state == 1)turn_on();
      else turn_off();
      anto.pub("LightSwitch1", relay_state);
    }

  }
}

void turn_on() {

  digitalWrite(SONOFF_LED_PIN, LOW);
  digitalWrite(SONOFF_RELAY_PIN, HIGH);
  relay_state = 1;
  Serial.println("Turning On");
}
void turn_off() {

  digitalWrite(SONOFF_LED_PIN, HIGH);
  digitalWrite(SONOFF_RELAY_PIN, LOW);
  relay_state = 0;
  Serial.println("Turning Off");

}

void messageReceived(String thing, String channel, String payload) {
  Serial.println("Rx msg");
  if (thing == "LightControl" && channel == "LightSwitch1") {
    if (payload == "1")turn_on();
    else turn_off();
  }

}

