
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

const TProgmemRGBPalette16 Black_p FL_PROGMEM =
{  0x000000, 0x000000, 0x000000, 0x000000, 
   0x000000, 0x000000, 0x000000, 0x000000, 
   0x000000, 0x000000, 0x000000, 0x000000, 
   0x000000, 0x000000, 0x000000, 0x000000,  };


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


// A warm gold/amber palette evoking candlelight and golden ornaments
#define WG_Amber  0xFF6600
#define WG_Gold   0xFFAA00
#define WG_Cream  0xFFE080
#define WG_Warm   0xFF8800
const TProgmemRGBPalette16 WarmGold_p FL_PROGMEM =
{  WG_Amber, WG_Gold,  WG_Amber, WG_Gold,
   WG_Gold,  WG_Warm,  WG_Gold,  WG_Amber,
   WG_Cream, WG_Gold,  WG_Warm,  WG_Gold,
   WG_Amber, WG_Cream, WG_Gold,  WG_Warm };

// A Hanukkah palette: deep royal blue with silver-white accents
#define Han_Blue   0x003090
#define Han_Royal  0x1040C0
#define Han_Silver 0xC0C8D0
const TProgmemRGBPalette16 Hanukkah_p FL_PROGMEM =
{  Han_Blue,   Han_Blue,   Han_Blue,   Han_Blue,
   Han_Blue,   Han_Royal,  Han_Blue,   Han_Blue,
   Han_Blue,   Han_Blue,   Han_Royal,  Han_Blue,
   Han_Silver, Han_Blue,   Han_Blue,   Han_Silver };

// A Nordic winter palette: deep teal, aurora purple, and cold ice white
#define Nord_Teal   0x003060
#define Nord_Deep   0x0A0A40
#define Nord_Purple 0x401060
#define Nord_Ice    0x80C0E0
const TProgmemRGBPalette16 NordicWinter_p FL_PROGMEM =
{  Nord_Teal,   Nord_Deep,   Nord_Teal,   Nord_Purple,
   Nord_Teal,   Nord_Teal,   Nord_Purple, Nord_Teal,
   Nord_Deep,   Nord_Teal,   Nord_Teal,   Nord_Purple,
   Nord_Ice,    Nord_Teal,   Nord_Deep,   Nord_Teal };

// A halloween themed palette
#define Orange1 0x902C02
#define Orange2 0x902C05
#define Purple  0x301934
const TProgmemRGBPalette16 Halloween_p FL_PROGMEM =
{
   Orange1, Orange2, Orange1, Orange2, 
   Orange1, Orange2, Orange1, Orange2, 
   Orange1, Orange2, Orange1, Orange2, 
   Purple, Orange1, Orange2, Purple
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
