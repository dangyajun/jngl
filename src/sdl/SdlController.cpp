// Copyright 2017-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SdlController.hpp"

#include <cmath>

namespace jngl {

SdlController::SdlController(SDL_Joystick* const handle) : handle(handle) {
	switch (SDL_JoystickNumButtons(handle)) {
	case 11:
		model = Model::XBOX_WIRED;
		break;
	case 13:
		model = Model::DS4;
		break;
	case 15:
		model = Model::XBOX;
		break;
	case 16:
		model = Model::DS4;
		break;
	};
}

SdlController::~SdlController() {
	SDL_JoystickClose(handle);
}

float SdlController::stateWithoutDeadzone(controller::Button button) const {
	const bool xboxWired = model == Model::XBOX_WIRED;
	int axisIndex;
	switch (button) {
		case controller::LeftStickX: axisIndex = 0; break;
		case controller::LeftStickY: axisIndex = 1; break;
		case controller::RightStickX: axisIndex = xboxWired ? 2 : 3; break;
		case controller::RightStickY: axisIndex = xboxWired ? 3 : 4; break;
		case controller::LeftTrigger: axisIndex = xboxWired ? 5 : 2; break;
		case controller::RightTrigger: axisIndex = xboxWired ? 4 : 5; break;
		default: return down(button) ? 1 : 0;
	}
	float state = SDL_JoystickGetAxis(handle, axisIndex);
	if (state < 0) {
		state /= 32768;
	} else {
		state /= 32767;
	}
	if (button == controller::LeftStickY or button == controller::RightStickY) {
		state *= -1;
	}
	return state;
}

float SdlController::state(controller::Button button) const {
	float state = stateWithoutDeadzone(button);
	controller::Button otherAxis;
	switch(button) {
		case controller::LeftStickY:
			otherAxis = controller::LeftStickX;
			break;
		case controller::LeftStickX:
			otherAxis = controller::LeftStickY;
			break;
		case controller::RightStickY:
			otherAxis = controller::RightStickX;
			break;
		case controller::RightStickX:
			otherAxis = controller::RightStickY;
			break;
		default:
			return state; // no deadzone needed
	}
	auto otherState = stateWithoutDeadzone(otherAxis);
	if (model == Model::DS4) { // DualShock 4 returns sticks as square coordinates
		const double tmp = state * std::sqrt(1 - 0.5 * otherState * otherState);
		otherState *= std::sqrt(1 - 0.5 * state * state);
		state = tmp;
	}
	if (model != Model::XBOX_WIRED and model != Model::XBOX) {
		return state; // no deadzone needed
	}
	if (state * state + otherState * otherState < 0.1) { // inside deadzone circle?
		return 0;
	}
	return state;
}

bool SdlController::down(const controller::Button button) const {
	const bool xbox = (model == Model::XBOX || model == Model::XBOX_WIRED);
	if (xbox && (button == controller::LeftTrigger || button == controller::RightTrigger)) {
		return state(button) > 0;
	}
	int buttonIndex;
	switch (button) {
		case controller::A: buttonIndex = 0; break;
		case controller::B: buttonIndex = 1; break;
		case controller::X: buttonIndex = xbox ? 2 : 3; break;
		case controller::Y: buttonIndex = xbox ? 3 : 2; break;
		case controller::LeftButton: buttonIndex = 4; break;
		case controller::RightButton: buttonIndex = 5; break;
		case controller::LeftTrigger: buttonIndex = 6; break;
		case controller::RightTrigger: buttonIndex = 7; break;
		case controller::Start: buttonIndex = xbox ? 7 : 9; break;
		case controller::Back: buttonIndex = xbox ? 6 : 8; break;
		case controller::DpadUp: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_UP) != 0;
		case controller::DpadDown: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_DOWN) != 0;
		case controller::DpadLeft: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_LEFT) != 0;
		case controller::DpadRight: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_RIGHT) != 0;
		case controller::LeftStick: buttonIndex = xbox ? 9 : 11; break;
		case controller::RightStick: buttonIndex = xbox ? 10 : 12; break;
		default: return false;
	}
	return SDL_JoystickGetButton(handle, buttonIndex) != 0;
}

bool SdlController::is(SDL_Joystick* const handle) const {
	return this->handle == handle;
}

} // namespace jngl
