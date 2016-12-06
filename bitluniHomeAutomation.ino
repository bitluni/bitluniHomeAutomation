/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing http://youtube.com/bitlunislab
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Adafruit_NeoPixel.h>

#include "PinStates.h"
#include "LedStates.h"
#include "Fader.h"
#include "RainbowFunction.h"
#include "SimpleRGBFunction.h"
#include "WaveFunction.h"
#include "RF.h"

const char* ssid = "...";
const char* password = "...";

ESP8266WebServer server(80);

const int LED_PIN = D4;
const int LED_COUNT = 300;

const int RF_OSC = 200;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

LedStates currentLedStates(strip);
LedStates targetLedStates(strip);
Fader<LedStates> ledFader(currentLedStates, targetLedStates);
PinStates currentPinStates;
PinStates targetPinStates;
Fader<PinStates> pinFader(currentPinStates, targetPinStates);

void handleRoot() {
  String message = "<html><head></head><body style='font-family: sans-serif; font-size: 12px'>Following functions are available:<br><br>";
  message += "<a href='/rainbow?fade=3000'>/rainbow</a> a rainbow animation on LEDs<br>";
  message += "<a href='/wave?r=255&g=32&b=10&fade=5000'>/wave</a> a slow wave animation on LED on base color specified by arguments: r=<0..255> g=<0..255> b=<0..255><br>";
  message += "<a href='/setleds?r=32&g=64&b=32&fade=1000'>/setleds</a> sets LEDs to the color from arguments: r=<0..255> g=<0..255> b=<0..255><br>";
  message += "<a href='/ledsoff?fade=500'>/ledsoff</a> turns off LEDs<br>";
  message += "<a href='/setpins?D1=128&D2=256&D3=512'>/setpins</a> sets to any of the in arguments specified pins (D0..D8) to their PWM values (0..1023). To use them digital: 0=off, 1023=on<br>";
  message += "<a href='/togglepins'>/togglepins</a> inverts all pin values form pins used before.<br>";
  message += "<a href='/rf?D=6&t=200&id=28013&channel=0&on=1'>/rf</a> sends a rf code from arguments: D=<0..8> t=<0..1000> id=<0..1048575> channel=<0..2> on=<0..1>. Dx is the pin, t is the optional signal clock(default is 200, works for me)<br><br>";
  message += "All functions except togglepins and rf support the argument 'fade' which specifies the milliseconds it takes to fade to the new specified state. ...nice blending ;-)<br>";
  message += "<br>Syntax is as follows: http://&ltip>/&ltcommand>?&ltargument1>=&ltvalue1>&&ltargument2>=&ltvalue2>&...<br>";
  message += "You can click on each link to see an example.<br><br>";
  message += "have fun -<a href='http://youtube.com/bitlunislab'>bitluni</a></body></html>";
  server.send(200, "text/html", message);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

int getArgValue(String name)
{
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

bool checkFadeAndSetLedFunction(LedFunction *f)
{
  int fade = getArgValue("fade");
  if(fade > -1)
  {
    targetLedStates.setFunction(f);
    ledFader.start(fade);
  }
  else
    currentLedStates.setFunction(f);  
}

void handleRf()
{
	const int pinNumbers[] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};
	int pin = getArgValue("D");
	int t = getArgValue("t");
	if(t == -1) t = RF_OSC;
	int id = getArgValue("id");
	int ch = getArgValue("channel");
	int on = getArgValue("on");
	String out = "rf D";
	out += pin;
	out += " ";
	out += t;
	out += " ";
	out += id;
	out += " ";
	out += ch;
	out += " ";
	out += on;
	pinMode(pinNumbers[pin], OUTPUT);
	for(int i = 0; i < 5; i++)
		rfWriteCode(pinNumbers[pin], t, id, (1 << (ch + 1)) | (on > 0? 1: 0));
	server.send(200, "text/plain", out);	
}

void setup(void){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //find it as http://lights.local
  /*if (MDNS.begin("lights")) 
  {
    Serial.println("MDNS responder started");
  }*/
  
  server.on("/", handleRoot);

  server.on("/rainbow", [](){
    server.send(200, "text/plain", "rainbow");
    checkFadeAndSetLedFunction(new RainbowFunction());
  });

  server.on("/wave", [](){
    server.send(200, "text/plain", "wave");
    WaveFunction *f = new WaveFunction();
    f->init(server);
    checkFadeAndSetLedFunction(f);
  });

  server.on("/setleds", [](){
    server.send(200, "text/plain", "setleds");
    SimpleRGBFunction *f = new SimpleRGBFunction();
    f->init(server);
    checkFadeAndSetLedFunction(f);
  });

  server.on("/ledsoff", [](){
    server.send(200, "text/plain", "ledsoff");
    checkFadeAndSetLedFunction(new SimpleRGBFunction());
  });

  server.on("/togglepins", [](){
    server.send(200, "text/plain", "togglepins");
    currentPinStates.toggle();
    currentPinStates.commit();
  });
  
  server.on("/setpins", [](){
    server.send(200, "text/plain", "setpins");
    int fade = getArgValue("fade");
    if(fade > -1)
    {
      targetPinStates.loadValues(server);
      pinFader.start(fade);
    }
    else
    {
      currentPinStates.loadValues(server);
      currentPinStates.commit();
    }
  });

  server.on("/pinsoff", [](){
    server.send(200, "text/plain", "pinsoff");
    currentPinStates.setAllTo(0);
    currentPinStates.commit();
  });

  server.on("/rf", handleRf);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void)
{
  server.handleClient();
  //MDNS.update();
  currentLedStates.render();
  if(ledFader.active)
    targetLedStates.render();
  if(!ledFader.fade())
    currentLedStates.commit();
  pinFader.fade();
}
