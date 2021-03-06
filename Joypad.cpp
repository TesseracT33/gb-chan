#include "Joypad.h"

using namespace Util;

void Joypad::Initialize()
{
	P1 = bus->ReadIOPointer(Bus::Addr::P1);
}


void Joypad::Reset()
{
	buttons_currently_held = { 0, 0, 0, 0, 0, 0, 0, 0 };
	OpenGameControllers();
}


void Joypad::OpenGameControllers()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_IsGameController(i))
		{
			controller = SDL_GameControllerOpen(i);
			if (controller != nullptr)
				return;
		}
	}
}


void Joypad::PollInput()
{
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			MatchInputToBindings(event.key.keysym.sym, InputEvent::PRESS, InputMethod::KEYBOARD);
			break;

		case SDL_KEYUP:
			MatchInputToBindings(event.key.keysym.sym, InputEvent::RELEASE, InputMethod::KEYBOARD);
			break;

		case SDL_CONTROLLERBUTTONDOWN:
			MatchInputToBindings(event.cbutton.button, InputEvent::PRESS, InputMethod::JOYPAD);
			break;

		case SDL_CONTROLLERBUTTONUP:
			MatchInputToBindings(event.cbutton.button, InputEvent::RELEASE, InputMethod::JOYPAD);
			break;

		case SDL_CONTROLLERDEVICEADDED:
			OpenGameControllers();
			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			OpenGameControllers();
			break;

		default:
			break;
		}
	}
}


void Joypad::UpdateBinding(Button button, SDL_GameControllerButton bind)
{
	switch (button)
	{
	case Button::A     : new_JoypadBindings.A      = bind; break;
	case Button::B     : new_JoypadBindings.B      = bind; break;
	case Button::SELECT: new_JoypadBindings.SELECT = bind; break;
	case Button::START : new_JoypadBindings.START  = bind; break;
	case Button::RIGHT : new_JoypadBindings.RIGHT  = bind; break;
	case Button::LEFT  : new_JoypadBindings.LEFT   = bind; break;
	case Button::UP    : new_JoypadBindings.UP     = bind; break;
	case Button::DOWN  : new_JoypadBindings.DOWN   = bind; break;
	}
}


void Joypad::UpdateBinding(Button button, SDL_Keycode key)
{
	switch (button)
	{
	case Button::A     : new_keyboard_bindings.A      = key; break;
	case Button::B     : new_keyboard_bindings.B      = key; break;
	case Button::SELECT: new_keyboard_bindings.SELECT = key; break;
	case Button::START : new_keyboard_bindings.START  = key; break;
	case Button::RIGHT : new_keyboard_bindings.RIGHT  = key; break;
	case Button::LEFT  : new_keyboard_bindings.LEFT   = key; break;
	case Button::UP    : new_keyboard_bindings.UP     = key; break;
	case Button::DOWN  : new_keyboard_bindings.DOWN   = key; break;
	}
}


void Joypad::SaveBindings()
{
	joypad_bindings = new_JoypadBindings;
	keyboard_bindings = new_keyboard_bindings;
}


void Joypad::RevertBindingChanges()
{
	new_JoypadBindings = joypad_bindings;
	new_keyboard_bindings = keyboard_bindings;
}


void Joypad::ResetBindings(InputMethod input_method)
{
	if (input_method == InputMethod::JOYPAD)
		new_JoypadBindings = default_joypad_bindings;
	else
		new_keyboard_bindings = default_keyboard_bindings;
}


const char* Joypad::GetCurrentBindingString(Button button, InputMethod input_method)
{
	if (input_method == InputMethod::JOYPAD)
	{
		switch (button)
		{
		case Button::A:      return SDL_GameControllerGetStringForButton(new_JoypadBindings.A);
		case Button::B:      return SDL_GameControllerGetStringForButton(new_JoypadBindings.B);
		case Button::SELECT: return SDL_GameControllerGetStringForButton(new_JoypadBindings.SELECT);
		case Button::START:  return SDL_GameControllerGetStringForButton(new_JoypadBindings.START);
		case Button::RIGHT:  return SDL_GameControllerGetStringForButton(new_JoypadBindings.RIGHT);
		case Button::LEFT:   return SDL_GameControllerGetStringForButton(new_JoypadBindings.LEFT);
		case Button::UP:     return SDL_GameControllerGetStringForButton(new_JoypadBindings.UP);
		case Button::DOWN:   return SDL_GameControllerGetStringForButton(new_JoypadBindings.DOWN);
		}
	}
	else
	{
		switch (button)
		{
		case Button::A:      return SDL_GetKeyName(new_keyboard_bindings.A);
		case Button::B:      return SDL_GetKeyName(new_keyboard_bindings.B);
		case Button::SELECT: return SDL_GetKeyName(new_keyboard_bindings.SELECT);
		case Button::START:  return SDL_GetKeyName(new_keyboard_bindings.START);
		case Button::RIGHT:  return SDL_GetKeyName(new_keyboard_bindings.RIGHT);
		case Button::LEFT:   return SDL_GetKeyName(new_keyboard_bindings.LEFT);
		case Button::UP:     return SDL_GetKeyName(new_keyboard_bindings.UP);
		case Button::DOWN:   return SDL_GetKeyName(new_keyboard_bindings.DOWN);
		}
	}
	return nullptr;
}


void Joypad::MatchInputToBindings(s32 button, InputEvent input_event, InputMethod input_method)
{
	// 'button' can either be a SDL_Keycode (typedef s32) or an u8 (from a controller button event)
	// Pressing two opposing d-pad directions simultaneously is disallowed
	if (input_method == InputMethod::JOYPAD)
	{
		if      (button == joypad_bindings.A)      buttons_currently_held.A      = input_event;
		else if (button == joypad_bindings.B)      buttons_currently_held.B      = input_event;
		else if (button == joypad_bindings.SELECT) buttons_currently_held.SELECT = input_event;
		else if (button == joypad_bindings.START)  buttons_currently_held.START  = input_event;

		else if (button == joypad_bindings.RIGHT)  buttons_currently_held.RIGHT  = input_event && !buttons_currently_held.LEFT;
		else if (button == joypad_bindings.LEFT)   buttons_currently_held.LEFT   = input_event && !buttons_currently_held.RIGHT;
		else if (button == joypad_bindings.UP)     buttons_currently_held.UP     = input_event && !buttons_currently_held.DOWN;
		else if (button == joypad_bindings.DOWN)   buttons_currently_held.DOWN   = input_event && !buttons_currently_held.UP;

		else return;
	}
	else
	{
		if      (button == keyboard_bindings.A)      buttons_currently_held.A      = input_event;
		else if (button == keyboard_bindings.B)      buttons_currently_held.B      = input_event;
		else if (button == keyboard_bindings.SELECT) buttons_currently_held.SELECT = input_event;
		else if (button == keyboard_bindings.START)  buttons_currently_held.START  = input_event;

		else if (button == keyboard_bindings.RIGHT)  buttons_currently_held.RIGHT  = input_event;
		else if (button == keyboard_bindings.LEFT)   buttons_currently_held.LEFT   = input_event;
		else if (button == keyboard_bindings.UP)     buttons_currently_held.UP     = input_event;
		else if (button == keyboard_bindings.DOWN)   buttons_currently_held.DOWN   = input_event;

		else return;
	}

	UpdateP1OutputLines();
}


// Updates bits 0-3 of P1. Called whenever a button is pressed, or when P1 bit 4 or 5 is written to
void Joypad::UpdateP1OutputLines()
{
	u8 prev_P1 = *P1;

	// an input line set to 0 only if P14/P15 is also 0, and a button is pressed
	bool P14 = CheckBit(P1, 4);
	bool P15 = CheckBit(P1, 5);
	bool P10 = !(buttons_currently_held.RIGHT && P14 == 0 || buttons_currently_held.A      && P15 == 0);
	bool P11 = !(buttons_currently_held.LEFT  && P14 == 0 || buttons_currently_held.B      && P15 == 0);
	bool P12 = !(buttons_currently_held.UP    && P14 == 0 || buttons_currently_held.SELECT && P15 == 0);
	bool P13 = !(buttons_currently_held.DOWN  && P14 == 0 || buttons_currently_held.START  && P15 == 0);
	*P1 &= 0xF0;
	*P1 |= P10 | P11 << 1 | P12 << 2 | P13 << 3;

	// request interrupt if P10, P11, P12 or P13 has gone from 1 to 0
	if (P10 == 0 && CheckBit(prev_P1, 0) == 1 ||
		P11 == 0 && CheckBit(prev_P1, 1) == 1 ||
		P12 == 0 && CheckBit(prev_P1, 2) == 1 ||
		P13 == 0 && CheckBit(prev_P1, 3) == 1)
	{
		cpu->RequestInterrupt(CPU::Interrupt::Joypad);
	}
}


void Joypad::Configure(Serialization::BaseFunctor& functor)
{
	functor.fun(&joypad_bindings, sizeof(joypad_bindings));
	functor.fun(&keyboard_bindings, sizeof(keyboard_bindings));
}


void Joypad::SetDefaultConfig()
{
	joypad_bindings = new_JoypadBindings = default_joypad_bindings;
	keyboard_bindings = new_keyboard_bindings = default_keyboard_bindings;
}