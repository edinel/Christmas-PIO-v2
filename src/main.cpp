
#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <stdio.h>
#include <time.h>
#include "arduino-secrets.h"
#include <TemplatePrinter.h>
#include "string_constants.h"

#define DATA_PIN_1    27
#define DATA_PIN_2    33
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS_1    400
#define NUM_LEDS_2    200
#define BRIGHTNESS          255  
#define FRAMES_PER_SECOND  120
#define NUM_FUNCTIONS 4
#define hostname "BackYard-Xmas-Arduino"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

CRGBArray<NUM_LEDS_1> leds_1;
CRGBArray<NUM_LEDS_2> leds_2;
bool debug = true;

// List of patterns to cycle through.  Each is defined as a separate function below.
//typedef void (*SimplePatternList[])();
// SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon };
String gButtonClicked = "off";
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 111; // rotating "base color" used by many of the patterns
int gLoopCounter = 0;

// Set web server port number to 80
WiFiServer gWiFiServer(80);

// Variable to store the HTTP request
String header;



// Overall twinkle speed.
// 0 (VERY slow) to 8 (VERY fast).  
// 4, 5, and 6 are recommended, default is 4.
#define TWINKLE_SPEED 4

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once).  
// Default is 5.
#define TWINKLE_DENSITY 5

// How often to change color palettes.
#define SECONDS_PER_PALETTE  30
// Also: toward the bottom of the file is an array 
// called "ActivePaletteList" which controls which color
// palettes are used; you can add or remove color palettes
// from there freely.

// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
CRGB gBackgroundColor = CRGB::Black; 
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries 
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will 
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 1


CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;



// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;



void Connect_to_Wifi() {
  int status = WL_IDLE_STATUS;
  Serial.print("Attempting to connect to WiFi, ");
  Serial.print("SSID ");
  Serial.println(ssid);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println(WiFi.localIP());

}

void Print_Wifi_Status() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String isThisOn (String color){
  if (color == gButtonClicked){
    return "checked";
  }else{
    return "";
  }
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % NUM_FUNCTIONS;
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds_1, NUM_LEDS_1, gHue, 7);
  fill_rainbow( leds_2, NUM_LEDS_2, gHue, 7);
  Serial.println ("Rainbow");
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds_1[ random16(NUM_LEDS_1) ] += CRGB::White;
    leds_2[ random16(NUM_LEDS_2) ] += CRGB::White;
  }
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
  Serial.println ("Rainbow with Glitter");
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds_1, NUM_LEDS_1, 10);
  int pos1 = random16(NUM_LEDS_1);
  leds_1[pos1] += CHSV( gHue + random8(64), 200, 255);

  fadeToBlackBy( leds_2, NUM_LEDS_2, 10);
  int pos2 = random16(NUM_LEDS_2);
  leds_2[pos2] += CHSV( gHue + random8(64), 200, 255);
  Serial.println ("Confetti");
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds_1, NUM_LEDS_1, 20);
  int pos1 = beatsin16( 13, 0, NUM_LEDS_1-1 );
  leds_1[pos1] += CHSV( gHue, 255, 192);

  fadeToBlackBy( leds_2, NUM_LEDS_2, 20);
  int pos2 = beatsin16( 13, 0, NUM_LEDS_2-1 );
  leds_2[pos2] += CHSV( gHue, 255, 192);
  Serial.println ("sinelon");
}

void coolLikeIncandescent( CRGB& c, uint8_t phase)
// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the 
// way that incandescent bulbs fade toward 'red' as they dim.
{
  if( phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8( c.g, cooling);
  c.b = qsub8( c.b, cooling * 2);
}

uint8_t attackDecayWave8( uint8_t i)
// This function is like 'triwave8', which produces a 
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//
{
  if( i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i/2));
  }
}

CRGB computeOneTwinkle( uint32_t ms, uint8_t salt)

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as 
//  input to the brightness wave function.  
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
{
  uint16_t ticks = ms >> (8-TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8( slowcycle16);
  slowcycle16 =  (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);
  
  uint8_t bright = 0;
  if( ((slowcycle8 & 0x0E)/2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8( fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if( bright > 0) {
    c = ColorFromPalette( gCurrentPalette, hue, bright, NOBLEND);
    if( COOL_LIKE_INCANDESCENT == 1 ) {
      coolLikeIncandescent( c, fastcycle8);
    }
  } else {
    c = CRGB::Black;
  }
  return c;
}

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
{  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
{  Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Red 
};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
{  CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
{  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
{  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
{  0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red    0xB80400
#define C9_Orange 0x902C02
#define C9_Green  0x046002
#define C9_Blue   0x070758
#define C9_White  0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
{  C9_Red,    C9_Orange, C9_Red,    C9_Orange,
   C9_Orange, C9_Red,    C9_Orange, C9_Red,
   C9_Green,  C9_Green,  C9_Green,  C9_Green,
   C9_Blue,   C9_Blue,   C9_Blue,
   C9_White
};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
{
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};

#define Black_1 0x000000
const TProgmemRGBPalette16 Black_p FL_PROGMEM = {
  Black_1, Black_1, Black_1, Black_1,
  Black_1, Black_1,Black_1, Black_1,
  Black_1, Black_1,Black_1, Black_1,
  Black_1, Black_1,Black_1, Black_1,
};

// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16* ActivePaletteList[] = {
  &RetroC9_p,
  &BlueWhite_p,
  &RainbowColors_p,
  &FairyLight_p,
  &RedGreenWhite_p,
  &PartyColors_p,
  &RedWhite_p,
  &Snow_p,
  &Holly_p,
  &Ice_p  
};


// Advance to the next color palette in the list (above).
void chooseNextColorPalette( CRGBPalette16& pal)
{
  const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
  static uint8_t whichPalette = -1; 
  whichPalette = addmod8( whichPalette, 1, numberOfPalettes);

  pal = *(ActivePaletteList[whichPalette]);
}


void drawTwinkles( CRGBSet& L)
{
  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;
  
  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
  // the current palette are identical, then a deeply faded version of
  // that color is used for the background color
  CRGB bg;
  if( (AUTO_SELECT_BACKGROUND_COLOR == 1) &&
      (gCurrentPalette[0] == gCurrentPalette[1] )) {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if( bglight > 64) {
      bg.nscale8_video( 16); // very bright, so scale to 1/16th
    } else if( bglight > 16) {
      bg.nscale8_video( 64); // not that bright, so scale to 1/4th
    } else {
      bg.nscale8_video( 86); // dim, scale to 1/3rd.
    }
  } else {
    bg = gBackgroundColor; // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();
  
  for( CRGB& pixel: L) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16= PRNG16; // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle( myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if( deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color, 
      // use the new color.
      pixel = c;
    } else if( deltabright > 0 ) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend( bg, c, deltabright * 8);
    } else { 
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }
  }
}

bool RadioProcessor (Print& output, const char *param){
  if (strcmp(param, "CSS_CODE")== 0){
    output.print (CSS_CODE);
    Serial.println ("CSS");
    return true;
  }else if (strcmp (param, "BUTTONPLACEHOLDER") == 0){
    output.print ("");
    output.print ("<div class=\"radio-toolbar\">\n");
    output.print ("<input type=\"radio\" id=\"off\" name=\"selector\" value=\"off\" ");
    output.print (isThisOn("off"));
    output.print (">\n");

    output.print ("<label for=\"blue\">Off</label>\n");
    output.print ("<input type=\"radio\" id=\"blue\" name=\"selector\" value=\"blue\" ");
    output.print (isThisOn("blue"));
    output.print (">\n");
    output.print ("<label for=\"blue\">Blue</label>\n");

    output.print ("<input type=\"radio\" id=\"green\" name=\"selector\" value=\"green\" ");
    output.print (isThisOn("green"));
    output.print (">\n");
    output.print ("<label for=\"green\">Green</label>\n");

    output.print ("<input type=\"radio\" id=\"red\" name=\"selector\" value=\"red\" ");
    output.print (isThisOn("red"));
    output.print (">\n");
    output.print ("<label for=\"red\">Red</label>\n");
    output.print ("<p>");
    return true;
  }else if (strcmp (param, "BUTTON_TWO")==0 ){
    output.print("<div class='content'> \n <h3>Ripple animation on  input type radio and Checkbox</h3>");
    output.print("<div class=\"dpx\">\n");
    output.print("<div class=\'py\'>");
    output.print("<label>");
    output.print("<input type=\"radio\" class=\"option-input radio\" value=\"off\" name=\"Off\" ");
    output.print (isThisOn("off"));
    output.print("/>");
    output.print("Off\n");
    output.print ("</label>\n");
    output.print("<label>");
    output.print("<input type=\"radio\" class=\"option-input radio\" value=\"red\" name=\"Red\" ");
    output.print (isThisOn("red"));
    output.print("/>");
    output.print("Red\n");
    output.print ("</label>\n");
    output.print("<label>");
    output.print("<input type=\"radio\" class=\"option-input radio\" value=\"Green\" name=\"Green\" ");
    output.print (isThisOn("green"));
    output.print("/>");
    output.print("Green\n");
    output.print ("</label>\n");
    output.print("<label>");
    output.print("<input type=\"radio\" class=\"option-input radio\" value=\"blue\" name=\"Blue\" ");
    output.print (isThisOn("blue"));
    output.print("/>");
    output.print("Blue\n");
    output.print ("</label>\n");
    output.print ("</div>\n</div>\n</div>\n");
    return true;
  }else if (strcmp (param, "BOUNCE")==0 ){
    output.print("<h3>Bouncing Animation</h3>");
    output.print("<div class=\"container\"> ");
    output.print("<input type=\"radio\" class=\"radio\" id=\"radio-1\" name=\"selector\" value=\"off\"");
    output.print (isThisOn("off"));
    output.print("/>");
    output.print("<label for=\"radio-1\"></label>");
    output.print("<input type=\"radio\" class=\"radio\" id=\"radio-2\" name=\"selector\" value=\"red\"");
    output.print (isThisOn("red"));
    output.print("/>");
    output.print("<label for=\"radio-2\"></label>");
    output.print("<input type=\"radio\" class=\"radio\" id=\"radio-3\" name=\"selector\" value=\"blue\"");
    output.print (isThisOn("blue"));
    output.print("/>");
    output.print("<label for=\"radio-3\"></label>");
    output.print("<input type=\"radio\" class=\"radio\" id=\"radio-4\" name=\"selector\" value=\"green\"");
    output.print (isThisOn("green"));
    output.print("/>");
    output.print("<label for=\"radio-4\"></label>");
    output.print("<input type=\"radio\" class=\"radio\" id=\"radio-5\" name=\"selector\" value=\"yellow\"");
    output.print (isThisOn("yellow"));
    output.print("/>");
    output.print("<label for=\"radio-5\">yellow</label>");
    output.print("<input type=\"radio\" class=\"radio\" id=\"radio-6\" name=\"selector\" value=\"cyan\"");
    output.print (isThisOn("cyan"));
    output.print("/>");
    output.print("<label for=\"radio-6\">cyan</label>");
    output.print("<input type=\"radio\" class=\"radio\" id=\"radio-7\" name=\"selector\" value=\"white\"");
    output.print (isThisOn("white"));
    output.print("/>");
    output.print("<label for=\"radio-7\">white</label>");
    output.print ("</div>\n");
    return true;
  }else{
    return false; 
  }
}


void UpdatePattern (){
 if (gButtonClicked == "red"){
    gCurrentPalette = RetroC9_p;
  }else if (gButtonClicked == "blue"){
    gCurrentPalette = BlueWhite_p;
  }else if (gButtonClicked == "green"){
    gCurrentPalette = RedGreenWhite_p;
  }else if (gButtonClicked == "yellow"){
    gCurrentPalette = PartyColors_p;
  }else if (gButtonClicked == "cyan"){
    gCurrentPalette = Snow_p;
  }else if (gButtonClicked == "white"){
    gCurrentPalette = FairyLight_p;
  }else{ //we're in an unknown state or "off"
    gCurrentPalette = Black_p;
  }
}

void DoTwinkle (){
  drawTwinkles( leds_1);
  drawTwinkles( leds_2);
  FastLED.show();
}

void setup() {
  
  // SET UP SERIAL
  Serial.begin(115200);
    while (!Serial) {
      ;  // wait for serial port to connect. Needed for native USB port only
    }
  Serial.println ("1 setup");
  Serial.flush();


  // SET UP LEDS tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN_1,COLOR_ORDER>(leds_1, NUM_LEDS_1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_2,COLOR_ORDER>(leds_2, NUM_LEDS_2).setCorrection(TypicalLEDStrip);  
  FastLED.setBrightness(BRIGHTNESS); // set master brightness control
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  Serial.println("Before Show 1");
  leds_2[0] = CRGB::White;
  leds_1[0] = CRGB::White;
  FastLED.show();
  Serial.println ("After Show 1");
  // SET UP WIFI 
   Connect_to_Wifi();  // Like it says
  if (debug) { Print_Wifi_Status(); }
  sleep (3);
  Serial.println("Before Show 2");
  FastLED.show();
  Serial.println ("After Show 2");
  gWiFiServer.begin();
  Serial.println ("After WifiServer Start, before Show 3");
  FastLED.clear();
  FastLED.show();
  Serial.println ("After Show 3 ");

  Serial.println ("Exting setup");
  Serial.flush();
}

void loop() {
  gHue++;
  WiFiClient client = gWiFiServer.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Get info about which button was clicked and set the global variable for it.
            if (header.indexOf("GET /update?color=off") >= 0) {
              gButtonClicked = "off";
            }else if (header.indexOf("GET /update?color=red") >= 0) {
              gButtonClicked = "red";
            }else if (header.indexOf("GET /update?color=blue") >= 0) {
              gButtonClicked = "blue";
            }else if (header.indexOf("GET /update?color=green") >= 0) {
              gButtonClicked = "green";
            }else if (header.indexOf("GET /update?color=yellow") >= 0) {
              gButtonClicked = "yellow";
            }else if (header.indexOf("GET /update?color=cyan") >= 0) {
              gButtonClicked = "cyan";
            }else if (header.indexOf("GET /update?color=white") >= 0) {
              gButtonClicked = "white";
            }else{
              gButtonClicked = "off";
            }
            
            // Display the HTML web page
            TemplatePrinter printer(client, RadioProcessor);  
            printer.print (index_html);
            printer.flush();
            
/*            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            */
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

  
  UpdatePattern();
  drawTwinkles(leds_1);
  drawTwinkles(leds_2);
  FastLED.show(); 
}


















/*
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS_1; i++) { //9948
    leds_1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  for( int i = 0; i < NUM_LEDS_2; i++) { //9948
    leds_2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }

}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds_1, NUM_LEDS_1, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds_1[beatsin16( i+7, 0, NUM_LEDS_1-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }

fadeToBlackBy( leds_2, NUM_LEDS_2 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds_2[beatsin16( i+7, 0, NUM_LEDS_2-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }


}


*/


// HTML Code Ideas
/*

<a href="KFC.html" class="button">KFC</a>
 <a href="Pizza_hut.html" class="button">Pizza hut</a>
 
 
 
 a.button { 
   display: inline-block;
   padding: 5px;
   border: 1px solid #999;
   background: #aaa;
   color: #333;
   text-decoration: none;
 }


*/



