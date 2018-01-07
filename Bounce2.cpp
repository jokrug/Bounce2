// Please read Bounce2.h for information about the liscence and authors

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Bounce2.h"

static const uint8_t DEBOUNCED_STATE = 0b00000001;
static const uint8_t UNSTABLE_STATE  = 0b00000010;
static const uint8_t CHANGED_STATE   = 0b00000100;
static const uint8_t PRESSED_STATE   = 0b00001000;

Bounce::Bounce()
    : previous_millis(0)
    , interval_millis(10)
    , state(0)
    , pin(0)
    , analogLowerLevel(-1)
    , analogUpperLevel(-1)
    , readFunction(&Bounce::digitalBounceRead)
{}

void Bounce::attach(int pin) {
    this->pin = pin;
    state = 0;

    if ((this->*readFunction)(pin)) {
        setBounceFlag(DEBOUNCED_STATE | UNSTABLE_STATE);
    }
#ifdef BOUNCE_LOCK_OUT
    previous_millis = 0;
#else
    previous_millis = millis();
#endif
}

void Bounce::attach(int pin, int mode){
#if defined(ARDUINO_STM_NUCLEO_F103RB) || defined(ARDUINO_GENERIC_STM32F103C)
    pinMode(pin, (WiringPinMode)mode);
#else
    pinMode(pin, mode);
#endif
  this->attach(pin);
}

void Bounce::attachAnalog(int pin, int analogLevel, int analogTolerance) {
    if( analogLevel != -1)
    {
      this->analogLowerLevel = analogLevel-analogTolerance;
      this->analogUpperLevel = analogLevel+analogTolerance;
      readFunction = &Bounce::analogBounceRead;
    }
    this->attach(pin);
}

void Bounce::interval(uint16_t interval_millis)
{
    this->interval_millis = interval_millis;
}

bool Bounce::update()
{
  unsetBounceFlag(CHANGED_STATE);

#ifdef BOUNCE_LOCK_OUT
    // Ignore everything if we are locked out
    if ( (millis() - previous_millis) >= interval_millis ) {
        bool currentState = (this->*readFunction)(pin);
        if ( currentState != getBounceFlag(DEBOUNCED_STATE)) {
            previous_millis = millis();
            toggleBounceFlag(DEBOUNCED_STATE);
            setBounceFlag(CHANGED_STATE | PRESSED_STATE);
            return true;
        }
    }

#elif defined BOUNCE_WITH_PROMPT_DETECTION
    // Read the state of the switch port into a temporary variable.
    bool currentState = (this->*readFunction)(pin);
    if ( (currentState != getBounceFlag(DEBOUNCED_STATE)) &&
      // We have seen a change from the current button state.
         ( (millis() - previous_millis) >= interval_millis) ) {
        // We have passed the time threshold, so a new change of state is allowed.
        // set the CHANGED_STATE flag and the new DEBOUNCED_STATE.
        // This will be prompt as long as there has been greater than interval_misllis ms since last change of input.
        // Otherwise debounced state will not change again until bouncing is stable for the timeout period.
        toggleBounceFlag(DEBOUNCED_STATE);
        setBounceFlag(CHANGED_STATE | PRESSED_STATE);
        return true;
    }

    // If the readState is different from previous readState, reset the debounce timer - as input is still unstable
    // and we want to prevent new button state changes until the previous one has remained stable for the timeout.
    if ( currentState != getBounceFlag(UNSTABLE_STATE) ) {
	// Update Unstable Bit to macth readState
        toggleBounceFlag(UNSTABLE_STATE);
        previous_millis = millis();
    }

#else
    // Read the state of the switch in a temporary variable.
    bool currentState = (this->*readFunction)(pin);

    // If the reading is different from last reading, reset the debounce counter
    if ( currentState != getBounceFlag(UNSTABLE_STATE) ) {
        toggleBounceFlag(UNSTABLE_STATE);
        previous_millis = millis();
    } else
    {
        if ( ( millis() - previous_millis >= interval_millis ) &&
          // We have passed the threshold time, so the input is now stable
          // If it is different from last state, set the CHANGED_STATE flag
          ( currentState != getBounceFlag(DEBOUNCED_STATE) ) ) {
              toggleBounceFlag(DEBOUNCED_STATE);
              setBounceFlag(CHANGED_STATE | PRESSED_STATE);
              previous_millis = millis();
              return true;
        }
    }
#endif

    return false;
}

bool Bounce::read()
{
    return getBounceFlag(DEBOUNCED_STATE);
}

bool Bounce::rose()
{
    return getBounceFlag(DEBOUNCED_STATE) && getBounceFlag(CHANGED_STATE);
}

bool Bounce::fell()
{
    return !getBounceFlag(DEBOUNCED_STATE) && getBounceFlag(CHANGED_STATE);;
}

bool Bounce::pressed()
{
    bool ret = getBounceFlag(DEBOUNCED_STATE) && getBounceFlag(PRESSED_STATE);
    unsetBounceFlag(PRESSED_STATE);
    return ret;
}

int Bounce::analogBounceRead(uint8_t pin)
{
  int value = analogRead(pin);
  return((int) (value > analogLowerLevel) && (value < analogUpperLevel) );
}
