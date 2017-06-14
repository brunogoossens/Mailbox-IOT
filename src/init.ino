#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WIFI
const char* ssid          = ""; // wifi ssid
const char* password      = ""; // wifi password

// Mailgun
String apiHost            = ""; // api.mailgun.net/v3/sandbox--XXX--.mailgun.org
String apiHostFingerprint = ""; // 1B F5 6D 84 C6 76 9B 01 68 BA 9C 64 EB D5 49 F5 F0 F0 3D 8A // sha1 fingerpirnt form host api.mailgun.net
String apiName            = ""; // api
String apiKey             = ""; // key-XX
String from               = ""; // must be url encoded like test%40gmail.com
String to                 = ""; // must be url encoded like test%40gmail.com
String html               = "The mailman dropped new mail for you."; // must be url encoded
String subject            = "You got mail!"; // must be url encoded

// Pins
const int pin = 5; // GPIO5 - D1

// States
int oldPinValue = 2;

void setup() {
  pinMode(pin, INPUT_PULLUP);
  Serial.begin(9600);
  delay(10);
  WiFi.forceSleepBegin();
  delay(1);
}

void loop() {
  delay(500);
  checkMailboxOpen();
}

void checkMailboxOpen(){
  int val = digitalRead(pin);

  if(oldPinValue != val && oldPinValue == 0){
    Serial.println("Mailbox is open");
    connectToWifi();
    sendMailRequest();
    disconnectWifi();
  }

  oldPinValue = val;
}

void connectToWifi(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.forceSleepWake();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void disconnectWifi(){
  WiFi.disconnect();
  WiFi.forceSleepBegin();
  Serial.println("WiFi disconnected");
  delay(1);
}

void sendMailRequest(){
  Serial.println("Sending mail");
  HTTPClient http;
  http.begin("https://"+apiName+":"+apiKey+"@"+apiHost+"/messages", apiHostFingerprint);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("from="+from+"&to="+to+"&subject="+subject+"&html="+html);
  http.writeToStream(&Serial);
  http.end();
  Serial.print("HTTP code:");
  Serial.println(httpCode);
  if(httpCode < 0){
    Serial.printf("[HTTPS] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
}
