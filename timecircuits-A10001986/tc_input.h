/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2022 Thomas Winischhofer (A10001986)
 * https://github.com/realA10001986/Time-Circuits-Display-A10001986
 *
 * Keypad_I2C handling
 *
 * This is a customized fork and unification of the Keypad and the
 * Keypad_I2C libraries
 * Authors of original Keypad library: Mark Stanley, Alexander Brevig
 * Authors of original Keypad_I2C library: G. D. (Joe) Young, ptw
 * -------------------------------------------------------------------
 * License of original libraries and this version:
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version
 * 2.1 of the License or later versions.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <https://www.gnu.org/licenses/>
 */

#ifndef _TCINPUT_H
#define _TCINPUT_H

#include <Wire.h>

#define PCF8574 1 // PCF8574 I/O expander device is 1 byte wide
#define PCF8575 2 // PCF8575 I/O expander device is 2 bytes wide

#define KI2C_LISTMAX 1    // Max number of keys recognized simultaneously
#define KI2C_MAPSIZE 10   // Number of rows (x 16 columns)

typedef enum {
    IDLE,
    PRESSED,
    HOLD,
    RELEASED
} KeyState;

const char NO_KEY = '\0';

/*
 * TCKey class
 */

class TCKey {

    public:

        TCKey();

        char      kchar;
        int       kcode;
        KeyState  kstate;
        bool      stateChanged;
        unsigned long holdTimer;
};

/*
 * Keypad_i2c class
 */

#define makeKeymap(x) ((char*)x)

class Keypad_I2C {

    public:

        Keypad_I2C(char *userKeymap, const uint8_t *row, const uint8_t *col, 
                   uint8_t numRows, uint8_t numCols,
                   int address, int width = 1, TwoWire *awire = &Wire);

        void begin();

        // Setter for custom delay function
        void setCustomDelayFunc(void (*myDelay)(unsigned int));

        void setScanInterval(unsigned int interval);
        void setHoldTime(unsigned int holdTime);

        void addEventListener(void (*listener)(char, KeyState));

        bool scanKeypad();

    private:

        void scanKeys();
        bool updateList();
        void nextKeyState(uint8_t n, bool kstate);
        int  findInList(int keyCode);
        void transitionTo(uint8_t n, KeyState nextState);

        void (*_keypadEventListener)(char, KeyState);

        void pin_write(uint8_t pinNum, bool level);
        void port_write(uint16_t i2cportval);
        uint16_t readPinState();

        unsigned int  _scanInterval;
        unsigned int  _holdTime;
        const uint8_t *_rowPins;
        const uint8_t *_columnPins;
        uint8_t       _rows;
        uint8_t       _columns;
        char          *_keymap;
        int           _i2caddr;
        int           _i2cwidth;

        unsigned long _startTime = 0;        
        uint16_t      _rowMask;

        uint16_t      _pinState;

        uint16_t      _bitMap[KI2C_MAPSIZE];
        TCKey         _key[KI2C_LISTMAX];

        TwoWire       *_wire;

        // Ptr to custom delay function
        void (*_customDelayFunc)(unsigned int) = NULL;
};

/*
 * TCButton class
 */

enum stateMachine_t : int {
    OCS_INIT = 0,
    OCS_DOWN = 1,
    OCS_UP = 2,
    OCS_COUNT = 3,
    OCS_PRESS = 6,
    OCS_PRESSEND = 7,
    UNKNOWN = 99
};

class TCButton {
  
    public:
        TCButton(const int pin, const boolean activeLow = true, const bool pullupActive = true);
      
        void setDebounceTicks(const int ticks);
        void setClickTicks(const int ticks);
        void setPressTicks(const int ticks);
      
        void attachClick(void (*newFunction)(void));
        void attachLongPressStart(void (*newFunction)(void));
        void attachLongPressStop(void (*newFunction)(void));

        void tick(void);

    private:

        void tick(bool level);
        void reset(void);

        int _pin;
        
        unsigned int _debounceTicks = 50;
        unsigned int _clickTicks = 400;
        unsigned int _pressTicks = 800;
      
        int _buttonPressed;

        void (*_clickFunc)(void) = NULL;
        void (*_longPressStartFunc)(void) = NULL;
        void (*_longPressStopFunc)(void) = NULL;
        
        void _newState(stateMachine_t nextState);
      
        stateMachine_t _state     = OCS_INIT;
        stateMachine_t _lastState = OCS_INIT;
      
        unsigned long _startTime;
        int _nClicks;
        int _maxClicks = 1;
};

#endif
