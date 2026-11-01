#ifndef C_BLOCKS_H
#define C_BLOCKS_H

// The project's own generated declarations. Every struct field below is a
// strucpp type: an `IECVar<T>` / `IECStringVar<N>` wrapper for an elementary
// pin, and for a user-defined type the structure, enumeration or function block
// class the project declares. `generated.hpp` carries all of them, and pulls in
// the runtime headers transitively, so any translation unit that includes this
// header gets the whole surface regardless of include order.
//
// Every translation unit that includes this header is built at -std=gnu++17,
// which is what `generated.hpp` needs. On a Runtime v4 or Arduino target the
// POU glue and `c_blocks_code.cpp` are pre-compiled from
// `precompile/sources/` with the board's toolchain at that standard; on the
// baremetal/simulator path the flag is appended to the arduino-cli invocation
// so the sketch-side copy of `c_blocks_code.cpp` compiles identically. Only
// the core itself is left on whatever standard it ships with, and the core
// never includes this header.
#include "generated.hpp"

//definition of external blocks - ESP32_DAC
typedef struct {
  strucpp::IEC_USINT *IN1;
  strucpp::IEC_USINT *IN2;
} ESP32_DAC_VARS;
extern "C" void esp32_dac_setup(ESP32_DAC_VARS *vars);
extern "C" void esp32_dac_loop(ESP32_DAC_VARS *vars);

//definition of external blocks - ESP32_DISPLAY
typedef struct {
  strucpp::IEC_STRING *TEXTZEILE1;
  strucpp::IEC_STRING *TEXTZEILE2;
} ESP32_DISPLAY_VARS;
extern "C" void esp32_display_setup(ESP32_DISPLAY_VARS *vars);
extern "C" void esp32_display_loop(ESP32_DISPLAY_VARS *vars);

//definition of external blocks - ESP32_LM75
typedef struct {
  strucpp::IEC_REAL *FTEMPERATURE;
  strucpp::IEC_STRING *STEMPERATURE;
} ESP32_LM75_VARS;
extern "C" void esp32_lm75_setup(ESP32_LM75_VARS *vars);
extern "C" void esp32_lm75_loop(ESP32_LM75_VARS *vars);

//definition of external blocks - ESP32_PWM
typedef struct {
  strucpp::IEC_UINT *PINNR;
  strucpp::IEC_UINT *IDUTYCYCLE;
} ESP32_PWM_VARS;
extern "C" void esp32_pwm_setup(ESP32_PWM_VARS *vars);
extern "C" void esp32_pwm_loop(ESP32_PWM_VARS *vars);

//definition of external blocks - ESP32_TONE
typedef struct {
  strucpp::IEC_INT *IOKTAVE;
  strucpp::IEC_INT *INOTE;
  strucpp::IEC_BOOL *XNOTEON;
} ESP32_TONE_VARS;
extern "C" void esp32_tone_setup(ESP32_TONE_VARS *vars);
extern "C" void esp32_tone_loop(ESP32_TONE_VARS *vars);

#endif // C_BLOCKS_H
