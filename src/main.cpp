#include <fs.h>
#include <Arduino.h>
#include <hash.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiUdp.h>
#include <ArtnetnodeWifi.h>
#include <NeoPixelBus.h>

// Global universe buffer
struct {
  uint16_t universe;
  uint16_t length;
  uint8_t sequence;
  uint8_t *data;
} global;



#include "WebSocket.h"
#include "send_break.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     0 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool display_enabled = true;

// Artnet settings
WiFiUDP UdpSend;
ArtnetnodeWifi artnet;
unsigned long packetCounter = 0, frameCounter = 0;
float fps = 0;


//WS281X Pixels
#define NUM_LEDS 170  //max 1 universe
#define DATA_PIN RX
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUM_LEDS, DATA_PIN);

const int relayPin = D6;
const int button = D5;

int relayState = HIGH; 
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket previewSocket("/ws");
AsyncWiFiManager wifiManager(&server, &dnsServer);
char deviceName[17];
char longDeviceName[63];
int set_universe = 1;
int startingChannel = 0;
bool dmxEnabled = true;
bool shouldSaveConfig = false;

// keep track of the timing of the function calls
long tic_loop = 0, tic_fps = 0, tic_packet = 0, tic_ws = 0;

#define MIN(x,y) (x<y ? x : y)

void initTest();
void relayOneOn();
void relayOneOff();
void setupWebServer();
void deviceReset();
void configModeCallback(AsyncWiFiManager *);
void saveConfigCallback();
void LoadSettings();
void SaveSettings();
void SetupOTA();
void onDmxPacket(uint16_t, uint16_t, uint8_t, uint8_t *);

void setup() {
  //configure the relay
  pinMode(relayPin, OUTPUT);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); // turn on LED

  //configure the serial port (monitor)
  Serial.begin(115200);
  Serial.println("Setup started");

  global.universe = 0;
  global.sequence = 0;
  global.length = 512;
  global.data = (uint8_t *)malloc(512);
  for (int i = 0; i < 512; i++) global.data[i] = 0;

  LoadSettings();

  if (dmxEnabled) {
    //configure serial for DMX
    Serial1.begin(250000, SERIAL_8N2);
  } else {
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH); // turn off LED
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    display_enabled = false;
  }

  if (display_enabled) {
    display.display();
  }

  //Start led strip
  strip.Begin();
  initTest();

  //Setup WiFiManager and get us connected to wifi
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  // id/name, placeholder/prompt, default, length
  AsyncWiFiManagerParameter custom_devicename("DeviceName", "Device Name", deviceName, 17);
  wifiManager.addParameter(&custom_devicename);
  wifiManager.setTimeout(180);
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    deviceReset();
  }
  Serial.println("Wifi Connection Successful");
  if (display_enabled) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setTextWrap(true);
    display.print("SSID: ");
    display.println(WiFi.SSID());
    display.print("IP: "); 
    display.println(WiFi.localIP());
    display.println("Relay status: Off");
    display.display();
  }

  Serial.println("Save config");

  //save settings if we need to
  if (shouldSaveConfig)
  {
    strcpy(deviceName, custom_devicename.getValue());
    SaveSettings();
  }

  //web server reqeusts
  setupWebServer();

  //start the web server
  server.begin();
  SetupOTA();

  Serial.println("Web server started");

  //Start receivng artnet packets
  // max. 17 characters
  artnet.setShortName(deviceName);
  // max. 63 characters
  artnet.setLongName(longDeviceName);
  // set a starting universe if you wish, defaults to 0
  //artnetnode.setStartingUniverse(4);
  artnet.setNumPorts(1);
  artnet.enableDMXOutput(0);
  artnet.begin();
  artnet.setArtDmxCallback(onDmxPacket);

  Serial.println("Artnet started");

  // initialize all timers
  tic_loop   = millis();
  tic_packet = millis();
  tic_fps    = millis();
  tic_ws    = millis();

  pinMode(button, INPUT);

  Serial.println("Setup finished");
}

void loop() {
  ArduinoOTA.handle();
  artnet.read();

  // read the state of the switch into a local variable:
  int reading = digitalRead(button);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        Serial.println("Button pressed");
        relayState = !relayState;
        if (relayState) {
          relayOneOn();
        } else {
          relayOneOff();
        }
      }
    }
  }

  lastButtonState = reading;
  //Handle WebSocket
  if ((millis() - tic_ws) > 250) { //4Hz
    tic_ws = millis();
    previewSocket.cleanupClients();
    //previewSocket.binaryAll((uint8_t*)board.getLeds(), (size_t)BOARD_DATA_SIZE);
  }

  //Show FPS
  if ((millis() - tic_fps) > 1000 && (packetCounter > 1 || fps > 0)) { //1Hz
    fps = (100000 * packetCounter / (millis() - tic_fps)) / 100;
    tic_fps = millis();
    packetCounter = 0;
    Serial.print("FPS = ");
    Serial.println(fps);
    
    if (display_enabled) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("FPS: ");
      display.println(fps);
      display.display();
    }
  }

  //Send data to DMX / Show on LEDs
  if ((millis() - tic_loop) > 25) { //40Hz
    tic_loop = millis();

    strip.Show();
    if (dmxEnabled) {
      sendBreak();
      Serial1.write(0); // Start-Byte
      // send out the value of the selected channels (up to 512)
      for (int i = 0; i < global.length; i++) {
        Serial1.write(global.data[i]);
      }
    }
  }
}

RgbColor WheelColor(uint16_t brightness, uint16_t wheelValue) {
       // divide the wheelValue by 360.0f to get a value between 0.0 and 1.0 needed for HslColor
       return HslColor(wheelValue / 360.0f, 1.0f, brightness / 510.0f); // this will autoconvert back to RgbColor
   }

void SetupOTA()
{
  ArduinoOTA.onStart([]() {
    // String type;
    // if (ArduinoOTA.getCommand() == U_FLASH)
    //   type = "sketch";
    // else // U_SPIFFS
    //   type = "filesystem";
    //
    // // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    // Serial.println("Start updating " + type);
    Serial.println("Starting OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void initTest()
{
  int change = 3;
  int speed = 8;
  int val = 0;
  for (int step = 0; step < 55; step++) {
    for(int led = 0; led < 170; led++) {
      if (speed * step * change - led * change > 255) {
          val = (510 - (speed * step * change - led * change));
      } else {
          val = speed * step * change - led * change;
      }
    
      if (val < 0) val = 0;
      RgbColor newColor = WheelColor(val, led);
      strip.SetPixelColor(led, newColor);
    }
    strip.Show();
  }
}

void setupWebServer()
{
  //WebSocekt to controll output
  previewSocket.onEvent(onWsEvent);
  server.addHandler(&previewSocket);

  // Server with different default file
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  //Used for relay control
  server.on("/relayon", HTTP_GET, [](AsyncWebServerRequest *request){
    relayOneOn();
    request->send(200, "text/plain", "relay on");
  });
  server.on("/relayoff", HTTP_GET, [](AsyncWebServerRequest *request){
    relayOneOff();
    request->send(200, "text/plain", "relay off");
  });

  //used to force a reset of the device
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "resetting device");
    deviceReset();
  });

  server.on("/resetwifi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "resetting wifi settings ...");
    wifiManager.resetSettings();
    deviceReset();
  });

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request){
    StaticJsonDocument<200> doc;
    doc["heap"] = String(ESP.getFreeHeap());
    doc["device_name"] = String(deviceName);
    doc["long_device_name"] = String(longDeviceName);
    doc["universe"] = set_universe;
    doc["starting_channel"] = startingChannel;
    doc["dmx_enabled"] = dmxEnabled;
    String output = "";
    serializeJson(doc, output);
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", output); //Sends 404 File Not Found
    response->addHeader("Access-Control-Allow-Origin","*");
    request->send(response);
  });

  //handles the setting update
  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request){

    if (request->hasParam("deviceName") && request->hasParam("longDeviceName") && request->hasParam("universe") && request->hasParam("startingChannel") && request->hasParam("dmxEnabled"))
    {
      strcpy(deviceName, request->getParam("deviceName")->value().c_str());
      strcpy(longDeviceName, request->getParam("longDeviceName")->value().c_str());
      set_universe = request->getParam("universe")->value().toInt();
      startingChannel = request->getParam("startingChannel")->value().toInt();
      if (request->getParam("dmxEnabled")->value() == "true") {
        dmxEnabled = true;
      } else {
        dmxEnabled = false;
      }
      SaveSettings();
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "ok"); //Sends 404 File Not Found
      response->addHeader("Access-Control-Allow-Origin","*");
      request->send(response);
      return;
    }

    //failed request
    request->send(500,"text/plain","Error saving changes.");
  });
}

void relayOneOn()
{
  previewSocket.textAll("on",2);
  Serial.println("Turn Relay On");
  relayState = HIGH;
  digitalWrite(relayPin, HIGH);
}

void relayOneOff()
{
  previewSocket.textAll("off",3);
  Serial.println("Turn Relay Off");
  relayState = LOW;
  digitalWrite(relayPin, LOW);
}

void deviceReset()
{
  delay(3000);
  ESP.reset();
  delay(5000);
}

//callback printing IP and SSID
void configModeCallback(AsyncWiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());

  if (display_enabled) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("SSID: ");
    display.println(myWiFiManager->getConfigPortalSSID());
    display.print("IP: "); 
    display.println(WiFi.softAPIP());
    display.display();
  }
}

//callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

//Used to load up settings
void LoadSettings()
{
  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        DynamicJsonDocument doc(1024);
        auto error = deserializeJson(doc, configFile); 
        configFile.close();
        if (error) {
          Serial.println("failed to load json config");
        } else {
          Serial.println("\nparsed json");
          serializeJson(doc, Serial);
          if (doc.containsKey("device_name")) { strcpy(deviceName, doc["device_name"]); }
          if (doc.containsKey("long_device_name")) { strcpy(longDeviceName, doc["long_device_name"]); }
          if (doc.containsKey("universe")) { set_universe = doc["universe"]; }
          if (doc.containsKey("starting_channel")) { startingChannel = doc["starting_channel"]; }          
          if (doc.containsKey("dmx_enabled")) { dmxEnabled = doc["dmx_enabled"]; }  
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
}

//Used to save settings
void SaveSettings()
{
  Serial.println("saving config");
  DynamicJsonDocument doc(1024);
  doc["device_name"] = deviceName;
  doc["long_device_name"] = longDeviceName;
  doc["universe"] = set_universe;
  doc["starting_channel"] = startingChannel;
  doc["dmx_enabled"] = dmxEnabled;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  serializeJson(doc, Serial);
  serializeJson(doc, configFile);
  configFile.close();
  //end save
}

//this will be called for each UDP packet received
void onDmxPacket(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t * data) {
  // count FPS
  packetCounter++;

  if (universe == set_universe) {
    // read universe and put into the right part of the display buffer
    for (int i = 0; (i + startingChannel) < length / 3; i++)
    {
      int led = i;
      if (led < NUM_LEDS) strip.SetPixelColor(led, RgbColor(data[(i + startingChannel) * 3], data[(i + startingChannel) * 3 + 1], data[(i + startingChannel) * 3 + 2]));
    }
    // copy the data from the UDP packet over to the global universe buffer
    global.universe = universe;
    global.sequence = sequence;
    if (length < 512)
      global.length = length;
    for (int i = 0; i < global.length; i++)
      global.data[i] = data[i];
  }
} // onDmxpacket