/**
* EasyButton.cpp
* @author Evert Arias
* @version 1.0.0
* @license MIT
*/

#include "EasyButton.h"

void EasyButton::begin() {
	pinMode(_pin, _pu_enabled ? INPUT_PULLUP : INPUT);
	_current_state = digitalRead(_pin);
	if (_invert) _current_state = !_current_state;
	_time = millis();
	_last_state = _current_state;
	_changed = false;
	_last_change = _time;
}

void EasyButton::onPressed(EasyButton::callback_t callback) {
	mPressedCallback = callback;
}

void EasyButton::onPressedFor(uint32_t duration, EasyButton::callback_t callback) {
	_held_threshold = duration;
	mPressedForCallback = callback;
}

void EasyButton::onSequence(uint8_t sequences, uint32_t duration, EasyButton::callback_t callback)
{
	_press_sequences = sequences;
	_press_sequence_duration = duration;
	mPressedSequenceCallback = callback;
}

bool EasyButton::isPressed()
{
	return _current_state;
}

bool EasyButton::isReleased()
{
	return !_current_state;
}

bool EasyButton::wasPressed()
{
	return _current_state && _changed;
}

bool EasyButton::wasReleased()
{
	return !_current_state && _changed;
}

bool EasyButton::pressedFor(uint32_t duration)
{
	return _current_state && _time - _last_change >= duration;
}

bool EasyButton::releasedFor(uint32_t duration)
{
	return !_current_state && _time - _last_change >= duration;
}

bool EasyButton::read() {

	// get current millis.
	uint32_t read_started_ms = millis();

	// read pin value.
	bool pinVal = digitalRead(_pin);

	// if invert = true, invert Button's pin value. 
	if (_invert) {
		pinVal = !pinVal;
	};

	// detect change on button's state.
	if (read_started_ms - _last_change < _db_time)
	{
		// button's state has not changed.
		_changed = false;
	}
	else
	{
		// button's state has changed.
		_last_state = _current_state;				// save last state.
		_current_state = pinVal;					// assign new state as current state from pin's value.
		_changed = (_current_state != _last_state); // report state change if current state vary from last state.
													// if state has changed since last read.
		if (_changed) {
			// save current millis as last change time.
			_last_change = read_started_ms;
		}
	}

	// call the callback functions when conditions met.
	if (!_current_state && _changed) {
		// button was released.
		if (!_was_btn_held) {
			if (_short_press_count == 0) {
				_first_press_time = read_started_ms;
			}
			// increment presses counter.
			_short_press_count++;
			// button is not being held.
			// call the callback function for a short press event if it exist.
			if (mPressedCallback) {
				mPressedCallback();
			}

			if (_short_press_count == _press_sequences && _press_sequence_duration >= (read_started_ms - _first_press_time)) {
				if (mPressedSequenceCallback) {
					mPressedSequenceCallback();
				}
				_short_press_count = 0;
				_first_press_time = 0;
			}
			// if secuence timeout, reset short presses counters.
			else if (_press_sequence_duration <= (read_started_ms - _first_press_time)) {
				_short_press_count = 0;
				_first_press_time = 0;
			}
		}
		// button was not held.
		else {
			_was_btn_held = false;
		}
		// since button released, reset mPressedForCallbackCalled value.
		_held_callback_called = false;
	}
	// button is not released.
	else if (_current_state && _time - _last_change >= _held_threshold && mPressedForCallback) {
		// button has been pressed for at least the given time 
		_was_btn_held = true;
		// reset short presses counters.
		_short_press_count = 0;
		_first_press_time = 0;
		// call the callback function for a long press event if it exist and if it has not been called yet.
		if (mPressedForCallback && !_held_callback_called) {
			_held_callback_called = true; // set as called.
			mPressedForCallback();
		}
	}

	_time = read_started_ms;

	return _current_state;
}
