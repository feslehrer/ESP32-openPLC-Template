#include <cstdint>
#include <cstring>

#ifdef ARDUINO
#include <Arduino.h>
// Arduino declares several of its helpers as function-like preprocessor macros,
// and every one of them collides with a standard-library name that the headers
// below pull in: `min` / `max` with the `std::min` / `std::max` templates and
// `numeric_limits<T>::min()` / `max()`, and `abs` with `std::abs` — which is
// worse, because `<chrono>` (reached transitively on the mbed cores) calls
// `abs` with a template argument list the one-parameter macro cannot swallow:
// `macro "abs" passed 2 arguments, but takes just 1`, and the standard header
// then fails to parse.
//
// The AVR targets never showed it, because their libstdc++ is the vendored
// freestanding port and never reaches `<chrono>`. An mbed board does.
//
// `constrain` and `sq` are left alone: neither shadows a standard name, so
// user code keeps them.
#undef min
#undef max
#undef abs
// `round` is the same story: Energia defines it as a macro and `<chrono>`
// declares `chrono::round<ToDur>()`, so the macro swallows the call and the
// header fails to parse. Undef'd here because the include that trips over it is
// in this preamble. `std::round` / `::round` from <cmath> remain available.
#undef round
// `MIN` and `MAX` in capitals, which the IEC standard library declares as
// variadic templates (`MAX(a, b, c)`) while several cores define them as
// two-argument macros — ST's USB device library does, and it sits on the include
// path every STM32, mbed and rp2040 recipe builds. The macro wins, and
// `iec_std_lib.hpp` fails with "macro MAX passed 3 arguments, but takes just 2"
// before any user code is read. Same reasoning as the lowercase pair above.
#undef MIN
#undef MAX
// `CONCAT` for the same reason, one core further along: Zephyr's `sys/util.h`
// defines it as a variadic token-pasting macro, and the IEC standard library
// declares it as a function template. The macro swallows the declaration and
// `iec_string.hpp` fails on a token-pasting error it cannot reach past, before any
// user code is read. Any IEC standard name a core happens to use as a macro
// lands here; these are the ones met so far.
#undef CONCAT
// Energia numbers the GPIO ports as the macros `PA` through `PT`, each two
// letters a PLC program is likely to want. `PT` is the preset-time input of
// every IEC standard timer, so a project holding a TON, TOF or TP expanded the
// struct strucpp emits into `IEC_TIME 18;`. The whole family goes, since a port
// letter is not how anything addresses a pin from a C block.
#undef PA
#undef PB
#undef PC
#undef PD
#undef PE
#undef PF
#undef PG
#undef PH
#undef PJ
#undef PK
#undef PL
#undef PM
#undef PN
#undef PP
#undef PQ
#undef PR
#undef PS
#undef PT
#endif

// The C block interface — the `<POU>_VARS` struct for every C++ POU in this
// project, and the `extern "C"` declarations of the setup/loop entry points
// defined below.  Declared once, in c_blocks.h, and included rather than
// restated here: the POU glue strucpp emits includes the same header, so the
// two sides cannot describe the struct differently.
//
// It transitively carries `generated.hpp` — the strucpp runtime wrappers plus
// this project's own structures, enumerations and function block classes — so a
// C block can name every type the Variables Table can declare.
//
// arduino-cli compiles this TU with the sketch, in the core's own standard —
// which is also what lets the block resolve its Arduino `#include`s. The
// strucpp runtime is C++14, so no separate pass is needed to reach it.
#include "c_blocks.h"

/*********************/
/*  IEC Types defs   */
/*********************/
//
// File-scope raw typedefs for the user's LOCAL variables inside
// setup() / loop() (e.g. `IEC_INT my_temp = 0;`).  The auto-
// generated POU struct fields use the fully-qualified
// `strucpp::IEC_*` wrappers and are unaffected by these aliases.
// STRING / WSTRING are NOT defined as raw POD structs here — local
// STRING variables should use `strucpp::IEC_STRING` directly
// (`IECStringVar<254>`), matching the wrapper used on pin fields.

typedef uint8_t  IEC_BOOL;

typedef int8_t    IEC_SINT;
typedef int16_t   IEC_INT;
typedef int32_t   IEC_DINT;
typedef int64_t   IEC_LINT;

typedef uint8_t    IEC_USINT;
typedef uint16_t   IEC_UINT;
typedef uint32_t   IEC_UDINT;
typedef uint64_t   IEC_ULINT;

typedef uint8_t    IEC_BYTE;
typedef uint16_t   IEC_WORD;
typedef uint32_t   IEC_DWORD;
typedef uint64_t   IEC_LWORD;

typedef float    IEC_REAL;
typedef double   IEC_LREAL;

// ESP32_DAC — Variables Table names bound to the interface struct
#define IN1 (*(vars->IN1))
#define IN2 (*(vars->IN2))

/* ================================================================
 *  C/C++ FUNCTION BLOCK
 *
 *  ---------------------------------------------------------------
 *  - This function block runs **in sync** with the PLC runtime.
 *  - The `setup()` function is called once when the block initializes.
 *  - The `loop()` function is called at every PLC scan cycle.
 *  - Block input and output variables declared in the variable table
 *    can be accessed directly by name in this C/C++ code.
 *
 *  This block executes as part of the main PLC process and follows
 *  the configured scan time in the Resources. Use it for real-time
 *  control logic, fast I/O operations, or any C-based algorithms.
 * ================================================================ */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DAC1 25
#define DAC2 26

void esp32_dac_setup(ESP32_DAC_VARS *vars)
{
  #ifdef ARDUINO
    pinMode(DAC1, ANALOG);
    pinMode(DAC2, ANALOG);

  #endif
}

void esp32_dac_loop(ESP32_DAC_VARS *vars)
{
  #ifdef ARDUINO
    // IN1 / IN2 sind UINT (0–65535), dacWrite erwartet 0–255
    // Skalierung: (wert * 255) / 65535 → vereinfacht: wert / 257
    uint8_t dac1_val = (uint8_t)(IN1);
    uint8_t dac2_val = (uint8_t)(IN2);

    dacWrite(DAC1, dac1_val);
    dacWrite(DAC2, dac2_val);
  #endif
}
#undef IN1
#undef IN2

// ESP32_DISPLAY — Variables Table names bound to the interface struct
#define TextZeile1 (*(vars->TEXTZEILE1))
#define TextZeile2 (*(vars->TEXTZEILE2))

/* ================================================================
 *  C/C++ FUNCTION BLOCK
 *
 *  ---------------------------------------------------------------
 *  - This function block runs **in sync** with the PLC runtime.
 *  - The `setup()` function is called once when the block initializes.
 *  - The `loop()` function is called at every PLC scan cycle.
 *  - Block input and output variables declared in the variable table
 *    can be accessed directly by name in this C/C++ code.
 *
 *  This block executes as part of the main PLC process and follows
 *  the configured scan time in the Resources. Use it for real-time
 *  control logic, fast I/O operations, or any C-based algorithms.
 * ================================================================ */

#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <Wire.h>               // Wire Bibliothek für I2C-Display
#include <LiquidCrystal_I2C.h>

// I2C-Display: adresse 0x27, 2 Zeilen á 16 Zeichen
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Konvertiert UTF-8 Strings (OpenPLC) in den Zeichensatz des Standard-Textdisplays
String convertToLCDCharset(const String& input) 
{
    String output = "";
    output.reserve(input.length()); // Speicher vorab reservieren

    for (size_t i = 0; i < input.length(); i++) 
    {
        unsigned char c = input[i];

        // UTF-8 Sonderzeichen beginnen meist mit dem Startbyte 0xC3 oder 0xC2
        if (c == 0xC3 && i + 1 < input.length()) {
            unsigned char next = input[i + 1];
            i++; // Das Folgebyte direkt überspringen, da wir es jetzt verarbeiten

            switch (next) {
                case 0x9F: output += (char)0xE2; break; // ß
                case 0xA4:
                case 0x84: output += (char)0xE1; break; // ä
                case 0xB6: 
                case 0x96: output += (char)0xEF; break; // ö
                case 0xBC: 
                case 0x9C: output += (char)0xF5; break; // ü
                
                default: 
                    // Falls ein anderes 0xC3-Zeichen kommt, unverändert lassen
                    output += (char)c; 
                    output += (char)next; 
                    break;
            }
        } 
        else if (c == 0xC2 && i + 1 < input.length()) {
            unsigned char next = input[i + 1];
            if (next == 0xB0) { // UTF-8 für '°'
                output += (char)0xDF; // HD44780 spezifisches '°'
                i++;
            } else {
                output += (char)c;
            }
        }
        else {
            // Standard-ASCII-Zeichen einfach eins zu eins übernehmen
            output += (char)c;
        }
    }
    return output;
}




void esp32_display_setup(ESP32_DISPLAY_VARS *vars)
{
  Wire.begin(21,22);
  lcd.init();
  lcd.clear();
}

void esp32_display_loop(ESP32_DISPLAY_VARS *vars) 
{
  lcd.setCursor(0,0);
  String displayReadyString1 = convertToLCDCharset(TextZeile1.c_str());
  lcd.print(displayReadyString1);

  lcd.setCursor(0,1);
  String displayReadyString2 = convertToLCDCharset(TextZeile2.c_str());
  lcd.print(displayReadyString2.c_str());
}
#undef TextZeile1
#undef TextZeile2

// ESP32_LM75 — Variables Table names bound to the interface struct
#define fTemperature (*(vars->FTEMPERATURE))
#define sTemperature (*(vars->STEMPERATURE))

/* ================================================================
 *  C/C++ FUNCTION BLOCK
 *
 *  ---------------------------------------------------------------
 *  - This function block runs **in sync** with the PLC runtime.
 *  - The `setup()` function is called once when the block initializes.
 *  - The `loop()` function is called at every PLC scan cycle.
 *  - Block input and output variables declared in the variable table
 *    can be accessed directly by name in this C/C++ code.
 *
 *  This block executes as part of the main PLC process and follows
 *  the configured scan time in the Resources. Use it for real-time
 *  control logic, fast I/O operations, or any C-based algorithms.
 * ================================================================ */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <Wire.h>

#define SensorAdresse 0x48  // I2C-Adresse fuer LM75

float lm75_read(int adress); // Prototyp

void esp32_lm75_setup(ESP32_LM75_VARS *vars) 
{
  Wire.begin();   
}

void esp32_lm75_loop(ESP32_LM75_VARS *vars) 
{ //char buffer[16];
  //fTemperature = lm75_read(SensorAdresse);
  int16_t raw_temp = 0;

  Wire.beginTransmission(SensorAdresse);
  Wire.write(0);                            // Pointer auf Temperatur-Register setzen
  Wire.endTransmission();

  Wire.requestFrom(SensorAdresse, 2);
  if (Wire.available() >= 2)
  {
    byte msb = Wire.read();
    byte lsb = Wire.read();

    raw_temp = (msb << 8) | lsb;
    raw_temp = raw_temp >> 7; 
  }
  //return (float)raw_temp * 0.5;
  fTemperature = (float)raw_temp * 0.5;

  //snprintf(buffer, sizeof(buffer), "%.1f", fTemperature);
  String tempStr = String(fTemperature, 1); 
  sTemperature.set(tempStr.c_str());
}
#undef fTemperature
#undef sTemperature

// ESP32_PWM — Variables Table names bound to the interface struct
#define PinNr (*(vars->PINNR))
#define iDutycycle (*(vars->IDUTYCYCLE))

/* ================================================================
 *  C/C++ FUNCTION BLOCK
 *
 *  ---------------------------------------------------------------
 *  - This function block runs **in sync** with the PLC runtime.
 *  - The `setup()` function is called once when the block initializes.
 *  - The `loop()` function is called at every PLC scan cycle.
 *  - Block input and output variables declared in the variable table
 *    can be accessed directly by name in this C/C++ code.
 *
 *  This block executes as part of the main PLC process and follows
 *  the configured scan time in the Resources. Use it for real-time
 *  control logic, fast I/O operations, or any C-based algorithms.
 * ================================================================ */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


const int PWM_FREQU = 1'000;
const int PWM_RES = 12;
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RES) - 1); 

void esp32_pwm_setup(ESP32_PWM_VARS *vars)
{
  ledcAttach(PinNr,PWM_FREQU,PWM_RES);
}

void esp32_pwm_loop(ESP32_PWM_VARS *vars)
{ 
  int dimmer = map(iDutycycle,0,65535,0,MAX_DUTY_CYCLE);
  ledcWrite(PinNr, dimmer);        // (GPIO, duty_cycle)
}
#undef PinNr
#undef iDutycycle

// ESP32_TONE — Variables Table names bound to the interface struct
#define iOktave (*(vars->IOKTAVE))
#define iNote (*(vars->INOTE))
#define xNoteOn (*(vars->XNOTEON))

/* ================================================================
 *  C/C++ FUNCTION BLOCK
 *
 *  ---------------------------------------------------------------
 *  - This function block runs **in sync** with the PLC runtime.
 *  - The `setup()` function is called once when the block initializes.
 *  - The `loop()` function is called at every PLC scan cycle.
 *  - Block input and output variables declared in the variable table
 *    can be accessed directly by name in this C/C++ code.
 *
 *  This block executes as part of the main PLC process and follows
 *  the configured scan time in the Resources. Use it for real-time
 *  control logic, fast I/O operations, or any C-based algorithms.
 * ================================================================ */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>


#define SPEAKER 27

#define NOTE_C  0
#define NOTE_Cs 1
#define NOTE_D  2
#define NOTE_Eb 3
#define NOTE_E  4
#define NOTE_F  5
#define NOTE_Fs 6
#define NOTE_G  7
#define NOTE_Gs 8
#define NOTE_A  9
#define NOTE_Bb 10
#define NOTE_B  11

void esp32_tone_setup(ESP32_TONE_VARS *vars)
{
  pinMode(SPEAKER,OUTPUT);
  ledcAttach(SPEAKER,440,10);           //GPIO 27
}

void esp32_tone_loop(ESP32_TONE_VARS *vars)
{
    if (xNoteOn)
    {
        int noteValue = (int)iNote.get();
        if (noteValue<0 || noteValue>11)
            noteValue = 9;
        ledcWriteNote(SPEAKER,(note_t)noteValue,iOktave);  // (GPIO, Note, Oktave)
        
        //ledcWriteNote(SPEAKER,NOTE_A,iOktave);  // (GPIO, Note, Oktave)
    }
    else
        ledcWrite(SPEAKER,0);
}
#undef iOktave
#undef iNote
#undef xNoteOn

