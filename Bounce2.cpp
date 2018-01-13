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
    : mPreviousMillis(0)
    , mIntervalMillis(10)
    , mState(0)
    , mPin(0)
{}

void Bounce::attach(int pin) {
    mPin = pin;
    mState = 0;

    if ( bounceRead() ) {
        setBounceFlag(DEBOUNCED_STATE | UNSTABLE_STATE);
    }
#ifdef BOUNCE_LOCK_OUT
    mPreviousMillis = 0;
#else
    mPreviousMillis = millis();
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

void Bounce::interval(uint16_t interval_millis)
{
    mIntervalMillis = interval_millis;
}

bool Bounce::update()
{
  unsetBounceFlag(CHANGED_STATE);

#ifdef BOUNCE_LOCK_OUT
    // Ignore everything if we are locked out
    if ( (millis() - mPreviousMillis) >= mIntervalMillis ) {
        bool currentState =  bounceRead();
        if ( currentState != getBounceFlag(DEBOUNCED_STATE)) {
            mPreviousMillis = millis();
            toggleBounceFlag(DEBOUNCED_STATE);
            setBounceFlag(CHANGED_STATE | PRESSED_STATE);
            return true;
        }
    }

#elif defined BOUNCE_WITH_PROMPT_DETECTION
    // Read the state of the switch port into a temporary variable.
    bool currentState = bounceRead();
    if ( (currentState != getBounceFlag(DEBOUNCED_STATE)) &&
      // We have seen a change from the current button state.
         ( (millis() - mPreviousMillis) >= mIntervalMillis) ) {
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
        toggleBounceFlag(UNSTABLEulf vechta_STATE);
        mPreviousMillis = millis();
    }

#else
    // Read the state of the switch in a temporary variable.
    bool currentState = bounceRead();

    // If the reading is different from last reading, reset the debounce counter
    if ( currentState != getBounceFlag(UNSTABLE_STATE) ) {
        toggleBounceFlag(UNSTABLE_STATE);
        mPreviousMillis = millis();
    } else
    {
        if ( ( millis() - mPreviousMillis >= mIntervalMillis ) &&
          // We have passed the threshold time, so the input is now stable
          // If it is different from last state, set the CHANGED_STATE flag
          ( currentState != getBounceFlag(DEBOUNCED_STATE) ) ) {
              toggleBounceFlag(DEBOUNCED_STATE);
              setBounceFlag(CHANGED_STATE | PRESSED_STATE);
              mPreviousMillis = millis();
              return true;
        }
    }
#endif

    return false;
}

bool Bounce::read() const
{
    return getBounceFlag(DEBOUNCED_STATE);
}

bool Bounce::rose() const
{
    return getBounceFlag(DEBOUNCED_STATE) && getBounceFlag(CHANGED_STATE);
}

bool Bounce::fell() const
{
    return !getBounceFlag(DEBOUNCED_STATE) && getBounceFlag(CHANGED_STATE);
}

bool Bounce::pressed()
{
    bool ret = getBounceFlag(DEBOUNCED_STATE) && getBounceFlag(PRESSED_STATE);
    unsetBounceFlag(PRESSED_STATE);
    return ret;
}


//---------------------------------------------------------------------
BounceAnalog::BounceAnalog() : Bounce()
    , mAnalogLowerLevel(-1)
    , mAnalogUpperLevel(-1)
    , mAnalogValue(0)
{
}

void BounceAnalog::attachAnalog(int pin, int analogLevel, int analogTolerance) {
    if( analogLevel != -1)
    {
      mAnalogLowerLevel = analogLevel-analogTolerance;
      mAnalogUpperLevel = analogLevel+analogTolerance;
    }
    this->attach(pin);
}

int BounceAnalog::bounceRead()
{
  mAnalogValue = analogRead(mPin);
  return((int) (mAnalogValue > mAnalogLowerLevel) && (mAnalogValue < mAnalogUpperLevel) );
}
