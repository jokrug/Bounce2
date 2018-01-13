/*
  The MIT License (MIT)

  Copyright (c) 2013 thomasfredericks

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
  Main code by Thomas O Fredericks (tof@t-o-f.info)
  Previous contributions by Eric Lowry, Jim Schimpf and Tom Harkaway
  BounceAnalog added by Joachim Krueger (https://github.com/jokrug/Bounce2)
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef Bounce2_h
#define Bounce2_h

// Uncomment the following line for "LOCK-OUT" debounce method
//#define BOUNCE_LOCK_OUT

// Uncomment the following line for "BOUNCE_WITH_PROMPT_DETECTION" debounce method
//#define BOUNCE_WITH_PROMPT_DETECTION

#include <inttypes.h>


class Bounce
{
 public:
    // Create an instance of the bounce library
    Bounce();
    // Attach to a pin (and also sets initial state)
    void attach(int pin);

    // Attach to a pin (and also sets initial state) and sets pin to mode (INPUT/INPUT_PULLUP/OUTPUT)
    void attach(int pin, int mode);

    // Sets the debounce interval
    void interval(uint16_t interval_millis);

    // Updates the pin
    // Returns true if the state changed
    // Returns false if the state did not change
    bool update();

    // Returns the updated pin state
    bool read() const;

    // Returns the falling pin state
    bool fell() const;

    // Returns the rising pin state
    bool rose() const;

    // Returns true for one call, if the button was pressed.
    // The button has to be released and pressed again for the next true.
    bool pressed();

    
    Bounce(uint8_t pin, unsigned long interval_millis ) : Bounce() {
        attach(pin);
        interval(interval_millis);
    }

protected:
    inline virtual int bounceRead() {return digitalRead(mPin);}
    uint8_t mPin;  

private:
    inline void setBounceFlag(const uint8_t flag)    {mState |= flag;}
    inline void unsetBounceFlag(const uint8_t flag)  {mState &= ~flag;}
    inline void toggleBounceFlag(const uint8_t flag) {mState ^= flag;}
    inline bool getBounceFlag(const uint8_t flag) const {return((mState & flag) != 0);}

    unsigned long mPreviousMillis;
    uint16_t mIntervalMillis;
    uint8_t  mState;
};


/* BounceAnalog adds the possibility to use one analog pin for many buttons.
 * Example for Arduino Nano in examples/bouce_alalog.ino
 *                  ____     ____     ____        |
 * +5V ------------|____|---|____|---|____|--     | Arduino
 *               |        |        |         |    |
 *         ____  /        /        /         /    |
 * GND ---|____|-|--------|--------|---------|----| A3
 *                                                |
 * All resistors 220 ohm.                         |     

 * You MUST use attachAnalog() with the BouceAnalog! 
 */ 
class BounceAnalog : public Bounce
{
public:
    BounceAnalog();
     
    // The button will be considered as "pressed", when the analog value
    // is between (analogLevel-analogTolerance) and (analogLevel+analogTolerance).
    // analogLevel and analogTolerance are in digits. For Arduino it should be between 0 and 1024.
    void attachAnalog(int pin, int analogLevel, int analogTolerance);

    // Returns the last analog value
    inline int getValue() const {return mAnalogValue;}

protected:
    virtual int bounceRead();

private:
    int mAnalogLowerLevel;
    int mAnalogUpperLevel;
    int mAnalogValue;
};

#endif
