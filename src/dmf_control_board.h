/*
Copyright 2011 Ryan Fobel

This file is part of dmf_control_board.

dmf_control_board is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

dmf_control_board is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with dmf_control_board.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DMF_CONTROL_BOARD_H_
#define _DMF_CONTROL_BOARD_H_

#include <stdint.h>
#ifndef AVR
  #include <iostream>
  #include <fstream>
  #include <string>
  #include <vector>
  #include <stdexcept>
#else
  #include "Arduino.h"
#endif
#include "RemoteObject.h"


class DmfControlBoard : public RemoteObject {
public:
  static const uint32_t BAUD_RATE = 115200;
  static const uint8_t SINE = 0;
  static const uint8_t SQUARE = 1;

  /**\brief Address of config settings in persistent storage _(i.e., EEPROM)_.
   */
  static const uint16_t PERSISTENT_CONFIG_SETTINGS = 100;

  struct version_t {
    uint16_t major;
    uint16_t minor;
    uint16_t micro;
  };

#ifdef AVR
  struct watchdog_t {
    /* # `watchdog_t` #
     *
     * This structure is used to maintain the state of a watchdog timer, which
     * can be used for any purpose.  For now, the watchdog timer is used to
     * maintain the state of the ATX power supply control signal.
     *
     * If the watchdog timer is enabled, _i.e., `enabled=true`, when
     * the timer period finishes:
     *
     *  - If the `state` is not `true`, the power-supply must be
     *   turned off.
     *  - The `state` must be set to `false`.
     *
     * When the watchdog timer is enabled, it is the responsibility of the
     * client to reset the watchdog-state before each timer period ends to
     * prevent the power supply from being turned off. */
    bool enabled;
    bool state;

    watchdog_t() : enabled(true), state(false) {}
  };

  struct config_settings_t {
    /**\brief This is the software version that the persistent configuration
     * data was written with.*/
    version_t version;

    #if ___HARDWARE_MAJOR_VERSION___ == 1 && ___HARDWARE_MINOR_VERSION___ < 3
      /**\brief This byte sets the value of the analog reference (between 0 and
      ~5V).*/
      uint8_t aref;
    #endif

    /**\brief i2c address of first high-voltage switching board.*/
    uint8_t switching_board_i2c_address;

    #if ___HARDWARE_MAJOR_VERSION___ == 1
      /**\brief This byte sets the maximum output voltage for the waveform
      generator. It should be trimmed so that the output waveform is 4Vp-p when
      POT_WAVEOUT_GAIN_2 is set to 255.*/
      uint8_t waveout_gain_1;

      /**\brief This byte sets the value of the virtual ground reference (between
      0 and 5V).*/
      uint8_t vgnd;

      /**\brief Series resistor values for channel 0.*/
      float A0_series_resistance[2];
      /**\brief Series capacitance values for channel 0.*/
      float A0_series_capacitance[2];
      /**\brief Series resistor values for channel 1.*/
      float A1_series_resistance[4];
      /**\brief Series capacitance values for channel 1.*/
      float A1_series_capacitance[4];
    #else
      /**\brief i2c address of first signal generator board.*/
      uint8_t signal_generator_board_i2c_address;
      /**\brief Series resistor values for channel 0.*/
      float A0_series_resistance[3];
      /**\brief Series capacitance values for channel 0.*/
      float A0_series_capacitance[3];
      /**\brief Series resistor values for channel 1.*/
      float A1_series_resistance[5];
      /**\brief Series capacitance values for channel 1.*/
      float A1_series_capacitance[5];
    #endif

    /**\brief Amplifier gain (if >0). If <=0, the gain is automatically
    adjusted based on the measured voltage from the amplifier.*/
    float amplifier_gain;

    /**\brief voltage tolerance for amplifier gain adjustment.*/
    float voltage_tolerance;
  };
#endif

  // TODO:
  //  Eventually, all of these variables should defined only on the arduino.
  //  The PC can interogate device using CMD_GET_NUMBER_OF_ADC_CHANNELS
  static const uint8_t NUMBER_OF_ADC_CHANNELS = 2;

  // Accessors and mutators
  static const uint8_t CMD_GET_NUMBER_OF_CHANNELS =         0xA0;
  static const uint8_t CMD_GET_STATE_OF_ALL_CHANNELS =      0xA1;
  static const uint8_t CMD_SET_STATE_OF_ALL_CHANNELS =      0xA2;
  static const uint8_t CMD_GET_STATE_OF_CHANNEL =           0xA3;
  static const uint8_t CMD_SET_STATE_OF_CHANNEL =           0xA4;
  static const uint8_t CMD_GET_WAVEFORM =                   0xA5;
  static const uint8_t CMD_SET_WAVEFORM =                   0xA6;
  static const uint8_t CMD_GET_WAVEFORM_VOLTAGE =           0xA7;
  static const uint8_t CMD_SET_WAVEFORM_VOLTAGE =           0xA8;
  static const uint8_t CMD_GET_WAVEFORM_FREQUENCY =         0xA9;
  static const uint8_t CMD_SET_WAVEFORM_FREQUENCY =         0xAA;
  static const uint8_t CMD_GET_SAMPLING_RATE =              0xAB;
  static const uint8_t CMD_SET_SAMPLING_RATE =              0xAC;
  static const uint8_t CMD_GET_SERIES_RESISTOR_INDEX =      0xAD;
  static const uint8_t CMD_SET_SERIES_RESISTOR_INDEX =      0xAE;
  static const uint8_t CMD_GET_SERIES_RESISTANCE =          0xAF;
  static const uint8_t CMD_SET_SERIES_RESISTANCE =          0xB0;
  static const uint8_t CMD_GET_SERIES_CAPACITANCE =         0xB1;
  static const uint8_t CMD_SET_SERIES_CAPACITANCE =         0xB2;
  static const uint8_t CMD_GET_AMPLIFIER_GAIN =             0xB3;
  static const uint8_t CMD_SET_AMPLIFIER_GAIN =             0xB4;
  static const uint8_t CMD_GET_AUTO_ADJUST_AMPLIFIER_GAIN = 0xB5;
  static const uint8_t CMD_SET_AUTO_ADJUST_AMPLIFIER_GAIN = 0xB6;
  static const uint8_t CMD_GET_POWER_SUPPLY_PIN =           0xB7;
  static const uint8_t CMD_GET_WATCHDOG_STATE =             0xB8;
  static const uint8_t CMD_SET_WATCHDOG_STATE =             0xB9;
  static const uint8_t CMD_GET_WATCHDOG_ENABLED =           0xBA;
  static const uint8_t CMD_SET_WATCHDOG_ENABLED =           0xBB;

  // Other commands
  static const uint8_t CMD_SYSTEM_RESET =                   0xF1; //TODO
  static const uint8_t CMD_DEBUG_MESSAGE =                  0xF2; //TODO
  static const uint8_t CMD_DEBUG_ON =                       0xF3; //TODO
  static const uint8_t CMD_MEASURE_IMPEDANCE =              0xF4;
  static const uint8_t CMD_RESET_CONFIG_TO_DEFAULTS =       0xF5;

  //////////////////////////////////////////////////////////////////////////////
  //
  // Return codes:
  //
  // Return codes are uint8_t.  Some generic return codes are defined by the
  // RemoteObject class.  You may also create custom return codes as long as
  // they are > 0x10.
  //
  //////////////////////////////////////////////////////////////////////////////

  DmfControlBoard();
  ~DmfControlBoard();

// In our case, the PC is the only one sending commands
#ifndef AVR
    std::string command_label(uint8_t command) const {
        if (command == CMD_GET_NUMBER_OF_CHANNELS) {
            return std::string("CMD_GET_NUMBER_OF_CHANNELS");
        } else if (command == CMD_GET_STATE_OF_ALL_CHANNELS) {
            return std::string("CMD_GET_STATE_OF_ALL_CHANNELS");
        } else if (command == CMD_SET_STATE_OF_ALL_CHANNELS) {
            return std::string("CMD_SET_STATE_OF_ALL_CHANNELS");
        } else if (command == CMD_GET_STATE_OF_CHANNEL) {
            return std::string("CMD_GET_STATE_OF_CHANNEL");
        } else if (command == CMD_SET_STATE_OF_CHANNEL) {
            return std::string("CMD_SET_STATE_OF_CHANNEL");
        } else if (command == CMD_GET_WAVEFORM) {
            return std::string("CMD_GET_WAVEFORM");
        } else if (command == CMD_SET_WAVEFORM) {
            return std::string("CMD_SET_WAVEFORM");
        } else if (command == CMD_GET_WAVEFORM_VOLTAGE) {
            return std::string("CMD_GET_WAVEFORM_VOLTAGE");
        } else if (command == CMD_SET_WAVEFORM_VOLTAGE) {
            return std::string("CMD_SET_WAVEFORM_VOLTAGE");
        } else if (command == CMD_GET_WAVEFORM_FREQUENCY) {
            return std::string("CMD_GET_WAVEFORM_FREQUENCY");
        } else if (command == CMD_SET_WAVEFORM_FREQUENCY) {
            return std::string("CMD_SET_WAVEFORM_FREQUENCY");
        } else if (command == CMD_GET_SAMPLING_RATE) {
            return std::string("CMD_GET_SAMPLING_RATE");
        } else if (command == CMD_SET_SAMPLING_RATE) {
            return std::string("CMD_SET_SAMPLING_RATE");
        } else if (command == CMD_GET_SERIES_RESISTOR_INDEX) {
            return std::string("CMD_GET_SERIES_RESISTOR_INDEX");
        } else if (command == CMD_SET_SERIES_RESISTOR_INDEX) {
            return std::string("CMD_SET_SERIES_RESISTOR_INDEX");
        } else if (command == CMD_GET_SERIES_RESISTANCE) {
            return std::string("CMD_GET_SERIES_RESISTANCE");
        } else if (command == CMD_SET_SERIES_RESISTANCE) {
            return std::string("CMD_SET_SERIES_RESISTANCE");
        } else if (command == CMD_GET_SERIES_CAPACITANCE) {
            return std::string("CMD_GET_SERIES_CAPACITANCE");
        } else if (command == CMD_SET_SERIES_CAPACITANCE) {
            return std::string("CMD_SET_SERIES_CAPACITANCE");
        } else if (command == CMD_GET_AMPLIFIER_GAIN) {
            return std::string("CMD_GET_AMPLIFIER_GAIN");
        } else if (command == CMD_SET_AMPLIFIER_GAIN) {
            return std::string("CMD_SET_AMPLIFIER_GAIN");
        } else if (command == CMD_GET_AUTO_ADJUST_AMPLIFIER_GAIN) {
            return std::string("CMD_GET_AUTO_ADJUST_AMPLIFIER_GAIN");
        } else if (command == CMD_SET_AUTO_ADJUST_AMPLIFIER_GAIN) {
            return std::string("CMD_SET_AUTO_ADJUST_AMPLIFIER_GAIN");
        } else if (command == CMD_GET_POWER_SUPPLY_PIN) {
            return std::string("CMD_GET_POWER_SUPPLY_PIN");
        } else if (command == CMD_GET_WATCHDOG_STATE) {
            return std::string("CMD_GET_WATCHDOG_STATE");
        } else if (command == CMD_SET_WATCHDOG_STATE) {
            return std::string("CMD_SET_WATCHDOG_STATE");
        } else if (command == CMD_GET_WATCHDOG_ENABLED) {
            return std::string("CMD_GET_WATCHDOG_ENABLED");
        } else if (command == CMD_SET_WATCHDOG_ENABLED) {
            return std::string("CMD_SET_WATCHDOG_ENABLED");
        } else {
            throw std::runtime_error("Invalid command.");
        }
    }
  uint16_t number_of_channels();
  std::vector<uint8_t> state_of_all_channels();
  uint8_t state_of_channel(const uint16_t channel);
  float sampling_rate();
  uint8_t series_resistor_index(const uint8_t channel);
  float series_resistance(const uint8_t channel);
  float series_capacitance(const uint8_t channel);
  std::string waveform();
  float waveform_frequency();
  float waveform_voltage();
  float amplifier_gain();
  bool auto_adjust_amplifier_gain();
  uint8_t power_supply_pin();
  bool watchdog_state();
  bool watchdog_enabled();

    template <typename Output>
    Output send_read_command(uint8_t command, const char* function_name) {
        LogSeparator();
        LogMessage("send command", function_name);
        if (SendCommand(command) == RETURN_OK) {
            LogMessage(command_label(command).c_str(), function_name);
            if (payload_length() == sizeof(Output)) {
                Output value = Read<Output>();
                LogMessage(boost::str(boost::format(command_label(command) +
                                                    "=" +
                                                    type_format<Output>()) %
                                      value).c_str(), function_name);
                return value;
            } else {
                LogMessage((command_label(command) +
                            ", Bad packet size").c_str(), function_name);
                throw std::runtime_error("Bad packet size.");
            }
        }
        throw std::runtime_error("Error processing command.");
    }

    template <typename T>
    uint8_t send_set_command(uint8_t command, const char* function_name,
                             T value) {
        LogSeparator();
        LogMessage("send command", function_name);
        Serialize(&value, sizeof(value));
        if (SendCommand(command) == RETURN_OK) {
            LogMessage(command_label(command).c_str(), function_name);
            LogMessage("  --> set successfully", function_name);
        }
        return return_code();
    }

  // Remote mutators (return code is from reply packet)
  uint8_t set_state_of_channel(const uint16_t channel, const uint8_t state);
  uint8_t set_state_of_all_channels(const std::vector<uint8_t> state);
  uint8_t set_waveform_voltage(const float v_rms);
  uint8_t set_waveform_frequency(const float freq_hz);
  uint8_t set_waveform(bool waveform);
  uint8_t set_sampling_rate(const uint8_t sampling_rate);
  uint8_t set_series_resistor_index(const uint8_t channel,
                                    const uint8_t index);
  uint8_t set_series_resistance(const uint8_t channel,
                                float resistance);
  uint8_t set_series_capacitance(const uint8_t channel,
                                 float capacitance);
  uint8_t set_amplifier_gain(float gain);
  uint8_t set_auto_adjust_amplifier_gain(bool on);
  uint8_t set_watchdog_state(bool state);
  uint8_t set_watchdog_enabled(bool state);

  // other functions
  void MeasureImpedanceNonBlocking(
                          uint16_t sampling_time_ms,
                          uint16_t n_samples,
                          uint16_t delay_between_samples_ms,
                          const std::vector<uint8_t> state);
  std::vector<float> GetImpedanceData();
  std::vector<float> MeasureImpedance(
                          uint16_t sampling_time_ms,
                          uint16_t n_samples,
                          uint16_t delay_between_samples_ms,
                          const std::vector<uint8_t> state);
  uint8_t ResetConfigToDefaults();
  std::string host_name() { return NAME_; }
  std::string host_manufacturer() { return MANUFACTURER_; }
  std::string host_software_version() { return SOFTWARE_VERSION_; }
  std::string host_url() { return URL_; }
#else
  void begin();

  // local accessors
  const char* protocol_name() { return PROTOCOL_NAME_; }
  const char* protocol_version() { return PROTOCOL_VERSION_; }
  const char* name() { return NAME_; } //device name
  const char* manufacturer() { return MANUFACTURER_; }
  const char* software_version() { return SOFTWARE_VERSION_; }
  const char* hardware_version();
  const char* url() { return URL_; }
  virtual void persistent_write(uint16_t address, uint8_t value);
  bool watchdog_enabled() { return watchdog_.enabled; }
  void watchdog_enabled(bool state) { watchdog_.enabled = state; }
  bool watchdog_state() { return watchdog_.state; }
  void watchdog_state(bool state) { watchdog_.state = state; }

  template <typename Timer, typename Function>
  void watchdog_reset(Timer &timer, Function &f) {
    timer.detachInterrupt();
    watchdog_state(true);
    timer.restart();
    timer.attachInterrupt(f);
  }

  /* Callback function when watchdog timer period is finished. */
  void watchdog_timeout() {
    /*
     * If the watchdog timer is enabled, _i.e., `enabled=true`, when
     * the timer period finishes:
     *
     *  - If the `state` is not `true`, the power-supply must be
     *   turned off.
     *  - The `state` must be set to `false`.
     *
     * When the watchdog timer is enabled, it is the responsibility of the
     * client to reset the watchdog-state before each timer period ends to
     * prevent the power supply from being turned off. */
    if (watchdog_enabled()) {
        if (!watchdog_state()) {
            /* Watchdog timer has timed out! */
            digitalWrite(13, 0);
        } else {
            watchdog_state(false);
        }
    }
  }

  /* Expose to allow timer callback to check state. */
  watchdog_t watchdog_;
#endif

private:
  // private static members
  static const char SOFTWARE_VERSION_[];
  static const char NAME_[];
  static const char HARDWARE_VERSION_[];
  static const char MANUFACTURER_[];
  static const char URL_[];
#ifdef AVR
  static const char PROTOCOL_NAME_[];
  static const char PROTOCOL_VERSION_[];

  static const uint8_t AD5204_SLAVE_SELECT_PIN_ = 53; // digital pot

  #if ___HARDWARE_MAJOR_VERSION___ == 1 && ___HARDWARE_MINOR_VERSION___ < 3
    static const uint8_t POT_INDEX_AREF_ = 0;
  #endif

  #if ___HARDWARE_MAJOR_VERSION___ == 1
    static const uint8_t POT_INDEX_VGND_ = 1;
    static const uint8_t POT_INDEX_WAVEOUT_GAIN_1_ = 2;
    static const uint8_t POT_INDEX_WAVEOUT_GAIN_2_ = 3;
  #endif

  #if ___HARDWARE_MAJOR_VERSION___ == 1 && ___HARDWARE_MINOR_VERSION___ > 1
    static const uint8_t PWR_SUPPLY_ON_ = 8;
  #elif ___HARDWARE_MAJOR_VERSION___ == 2
    static const uint8_t PWR_SUPPLY_ON_ = 2;
  #endif

  #if ___HARDWARE_MAJOR_VERSION___ == 1
    static const uint8_t WAVEFORM_SELECT_ = 9;
    static const uint8_t A0_SERIES_RESISTOR_0_ = 13;
    static const uint8_t A1_SERIES_RESISTOR_0_ = 12;
    static const uint8_t A1_SERIES_RESISTOR_1_ = 11;
    static const uint8_t A1_SERIES_RESISTOR_2_ = 10;
  #else
    static const uint8_t A0_SERIES_RESISTOR_0_ = 8;
    static const uint8_t A0_SERIES_RESISTOR_1_ = 9;
    static const uint8_t A1_SERIES_RESISTOR_0_ = 10;
    static const uint8_t A1_SERIES_RESISTOR_1_ = 11;
    static const uint8_t A1_SERIES_RESISTOR_2_ = 12;
    static const uint8_t A1_SERIES_RESISTOR_3_ = 13;
  #endif

  static const float SAMPLING_RATES_[];

  // I2C bus
  // =======
  // A4 SDA
  // A5 SCL

  // SPI bus
  // =======
  // D10 SS
  // D11 MOSI
  // D13 SCK

  // PCA9505 (gpio) chip/register addresses
  static const uint8_t PCA9505_CONFIG_IO_REGISTER_ = 0x18;
  static const uint8_t PCA9505_OUTPUT_PORT_REGISTER_ = 0x08;

  // LTC6904 (programmable oscillator) chip address
  static const uint8_t LTC6904_ = 0x17;
#else
  static const char CSV_INDENT_[];
#endif

  // private functions
  virtual uint8_t ProcessCommand(uint8_t cmd);
#ifdef AVR
  uint8_t UpdateChannel(const uint16_t channel, const uint8_t state);
  void UpdateAllChannels();
  void SendSPI(uint8_t pin, uint8_t address, uint8_t data);
  uint8_t SetPot(uint8_t index, uint8_t value);
  uint8_t SetSeriesResistor(const uint8_t channel,
                            const uint8_t index);
  uint8_t SetAdcPrescaler(const uint8_t index);
  void LoadConfig(bool use_defaults=false);
  void SaveConfig();
  version_t ConfigVersion();
  uint8_t SetWaveformVoltage(const float output_vrms,
                             const bool wait_for_reply=true);
#endif

  //private members
#ifdef AVR
  uint16_t number_of_channels_;
  uint8_t sampling_rate_index_;
  uint8_t A0_series_resistor_index_;
  uint8_t A1_series_resistor_index_;
  uint8_t peak_;
  float waveform_voltage_;
  float waveform_frequency_;
  float amplifier_gain_;
  bool auto_adjust_amplifier_gain_;
  config_settings_t config_settings_;
#endif
};
#endif // _DMF_CONTROL_BOARD_H_

