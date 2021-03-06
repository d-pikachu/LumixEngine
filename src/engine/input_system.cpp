#include "engine/input_system.h"
#include "engine/controller_device.h"
#include "engine/delegate.h"
#include "engine/delegate_list.h"
#include "engine/engine.h"
#include "engine/lua_wrapper.h"
#include "engine/profiler.h"
#include "engine/vec.h"
#include <SDL.h>


namespace Lumix
{


struct MouseDevice : InputSystem::Device
{
	void update(float dt) override {}
	const char* getName() const override { return "mouse"; }
};



struct KeyboardDevice : InputSystem::Device
{
	void update(float dt) override {}
	const char* getName() const override { return "keyboard"; }
};


struct InputSystemImpl LUMIX_FINAL : public InputSystem
{
	explicit InputSystemImpl(Engine& engine)
		: m_engine(engine)
		, m_allocator(engine.getAllocator())
		, m_events(m_allocator)
		, m_is_enabled(false)
		, m_devices(m_allocator)
		, m_to_remove(m_allocator)
	{
		m_mouse_device = LUMIX_NEW(m_allocator, MouseDevice);
		m_mouse_device->type = Device::MOUSE;
		m_keyboard_device = LUMIX_NEW(m_allocator, KeyboardDevice);
		m_keyboard_device->type = Device::KEYBOARD;
		m_devices.push(m_keyboard_device);
		m_devices.push(m_mouse_device);
		ControllerDevice::init(*this);
		registerLuaAPI();
	}


	~InputSystemImpl()
	{
		ControllerDevice::shutdown();
		for (Device* device : m_devices)
		{
			LUMIX_DELETE(m_allocator, device);
		}
	}


	IAllocator& getAllocator() override { return m_allocator; }
	void enable(bool enabled) override { m_is_enabled = enabled; }
	
	
	void addDevice(Device* device) override
	{
		m_devices.push(device);
		Event event;
		event.type = Event::DEVICE_ADDED;
		event.device = device;
		injectEvent(event);
	}


	void removeDevice(Device* device) override 
	{ 
		ASSERT(device != m_keyboard_device);
		ASSERT(device != m_mouse_device);
		m_to_remove.push(device);

		Event event;
		event.type = Event::DEVICE_REMOVED;
		event.device = device;
		injectEvent(event);
	}


	void update(float dt) override
	{
		PROFILE_FUNCTION();

		for (Device* device : m_to_remove)
		{
			m_devices.eraseItem(device);
			LUMIX_DELETE(m_allocator, device);
		}

		m_events.clear();

		for (Device* device : m_devices) device->update(dt);
		ControllerDevice::frame(dt);
	}

	
	void injectEvent(const Event& event) override
	{
		m_events.push(event);
	}


	int getEventsCount() const override { return m_events.size(); }
	const Event* getEvents() const override { return m_events.empty() ? nullptr : &m_events[0]; }

	Vec2 getCursorPosition() const override { return m_cursor_pos; }
	void setCursorPosition(const Vec2& pos) override { m_cursor_pos = pos; }


	int getDevicesCount() const override { return m_devices.size(); }
	Device* getDevice(int index) override { return m_devices[index]; }
	Device* getMouseDevice() override { return m_mouse_device; }
	Device* getKeyboardDevice() override { return m_keyboard_device; }
	void registerLuaAPI();


	Engine& m_engine;
	IAllocator& m_allocator;
	MouseDevice* m_mouse_device;
	KeyboardDevice* m_keyboard_device;
	Array<Event> m_events;
	bool m_is_enabled;
	Vec2 m_cursor_pos;
	Array<Device*> m_devices;
	Array<Device*> m_to_remove;
};


void InputSystemImpl::registerLuaAPI()
{
	lua_State* L = m_engine.getState();

	#define REGISTER_SCANCODE(SCANCODE) \
		LuaWrapper::createSystemVariable(L, "Engine", "INPUT_SCANCODE_" #SCANCODE, (int)SDL_SCANCODE_##SCANCODE);

		REGISTER_SCANCODE(A);
		REGISTER_SCANCODE(B);
		REGISTER_SCANCODE(C);
		REGISTER_SCANCODE(D);
		REGISTER_SCANCODE(E);
		REGISTER_SCANCODE(F);
		REGISTER_SCANCODE(G);
		REGISTER_SCANCODE(H);
		REGISTER_SCANCODE(I);
		REGISTER_SCANCODE(J);
		REGISTER_SCANCODE(K);
		REGISTER_SCANCODE(L);
		REGISTER_SCANCODE(M);
		REGISTER_SCANCODE(N);
		REGISTER_SCANCODE(O);
		REGISTER_SCANCODE(P);
		REGISTER_SCANCODE(Q);
		REGISTER_SCANCODE(R);
		REGISTER_SCANCODE(S);
		REGISTER_SCANCODE(T);
		REGISTER_SCANCODE(U);
		REGISTER_SCANCODE(V);
		REGISTER_SCANCODE(W);
		REGISTER_SCANCODE(X);
		REGISTER_SCANCODE(Y);
		REGISTER_SCANCODE(Z);
		REGISTER_SCANCODE(1);
		REGISTER_SCANCODE(2);
		REGISTER_SCANCODE(3);
		REGISTER_SCANCODE(4);
		REGISTER_SCANCODE(5);
		REGISTER_SCANCODE(6);
		REGISTER_SCANCODE(7);
		REGISTER_SCANCODE(8);
		REGISTER_SCANCODE(9);
		REGISTER_SCANCODE(0);
		REGISTER_SCANCODE(RETURN);
		REGISTER_SCANCODE(ESCAPE);
		REGISTER_SCANCODE(BACKSPACE);
		REGISTER_SCANCODE(TAB);
		REGISTER_SCANCODE(SPACE);
		REGISTER_SCANCODE(MINUS);
		REGISTER_SCANCODE(EQUALS);
		REGISTER_SCANCODE(LEFTBRACKET);
		REGISTER_SCANCODE(RIGHTBRACKET);
		REGISTER_SCANCODE(BACKSLASH);
		REGISTER_SCANCODE(NONUSHASH);
		REGISTER_SCANCODE(SEMICOLON);
		REGISTER_SCANCODE(APOSTROPHE);
		REGISTER_SCANCODE(GRAVE);
		REGISTER_SCANCODE(COMMA);
		REGISTER_SCANCODE(PERIOD);
		REGISTER_SCANCODE(SLASH);
		REGISTER_SCANCODE(CAPSLOCK);
		REGISTER_SCANCODE(F1);
		REGISTER_SCANCODE(F2);
		REGISTER_SCANCODE(F3);
		REGISTER_SCANCODE(F4);
		REGISTER_SCANCODE(F5);
		REGISTER_SCANCODE(F6);
		REGISTER_SCANCODE(F7);
		REGISTER_SCANCODE(F8);
		REGISTER_SCANCODE(F9);
		REGISTER_SCANCODE(F10);
		REGISTER_SCANCODE(F11);
		REGISTER_SCANCODE(F12);
		REGISTER_SCANCODE(PRINTSCREEN);
		REGISTER_SCANCODE(SCROLLLOCK);
		REGISTER_SCANCODE(PAUSE);
		REGISTER_SCANCODE(INSERT);
		REGISTER_SCANCODE(HOME);
		REGISTER_SCANCODE(PAGEUP);
		REGISTER_SCANCODE(DELETE);
		REGISTER_SCANCODE(END);
		REGISTER_SCANCODE(PAGEDOWN);
		REGISTER_SCANCODE(RIGHT);
		REGISTER_SCANCODE(LEFT);
		REGISTER_SCANCODE(DOWN);
		REGISTER_SCANCODE(UP);
		REGISTER_SCANCODE(NUMLOCKCLEAR);
		REGISTER_SCANCODE(KP_DIVIDE);
		REGISTER_SCANCODE(KP_MULTIPLY);
		REGISTER_SCANCODE(KP_MINUS);
		REGISTER_SCANCODE(KP_PLUS);
		REGISTER_SCANCODE(KP_ENTER);
		REGISTER_SCANCODE(KP_1);
		REGISTER_SCANCODE(KP_2);
		REGISTER_SCANCODE(KP_3);
		REGISTER_SCANCODE(KP_4);
		REGISTER_SCANCODE(KP_5);
		REGISTER_SCANCODE(KP_6);
		REGISTER_SCANCODE(KP_7);
		REGISTER_SCANCODE(KP_8);
		REGISTER_SCANCODE(KP_9);
		REGISTER_SCANCODE(KP_0);
		REGISTER_SCANCODE(KP_PERIOD);
		REGISTER_SCANCODE(NONUSBACKSLASH);
		REGISTER_SCANCODE(APPLICATION);
		REGISTER_SCANCODE(POWER);
		REGISTER_SCANCODE(KP_EQUALS);
		REGISTER_SCANCODE(F13);
		REGISTER_SCANCODE(F14);
		REGISTER_SCANCODE(F15);
		REGISTER_SCANCODE(F16);
		REGISTER_SCANCODE(F17);
		REGISTER_SCANCODE(F18);
		REGISTER_SCANCODE(F19);
		REGISTER_SCANCODE(F20);
		REGISTER_SCANCODE(F21);
		REGISTER_SCANCODE(F22);
		REGISTER_SCANCODE(F23);
		REGISTER_SCANCODE(F24);
		REGISTER_SCANCODE(EXECUTE);
		REGISTER_SCANCODE(HELP);
		REGISTER_SCANCODE(MENU);
		REGISTER_SCANCODE(SELECT);
		REGISTER_SCANCODE(STOP);
		REGISTER_SCANCODE(AGAIN);
		REGISTER_SCANCODE(UNDO);
		REGISTER_SCANCODE(CUT);
		REGISTER_SCANCODE(COPY);
		REGISTER_SCANCODE(PASTE);
		REGISTER_SCANCODE(FIND);
		REGISTER_SCANCODE(MUTE);
		REGISTER_SCANCODE(VOLUMEUP);
		REGISTER_SCANCODE(VOLUMEDOWN);
		REGISTER_SCANCODE(KP_COMMA);
		REGISTER_SCANCODE(KP_EQUALSAS400);
		REGISTER_SCANCODE(INTERNATIONAL1);
		REGISTER_SCANCODE(INTERNATIONAL2);
		REGISTER_SCANCODE(INTERNATIONAL3);
		REGISTER_SCANCODE(INTERNATIONAL4);
		REGISTER_SCANCODE(INTERNATIONAL5);
		REGISTER_SCANCODE(INTERNATIONAL6);
		REGISTER_SCANCODE(INTERNATIONAL7);
		REGISTER_SCANCODE(INTERNATIONAL8);
		REGISTER_SCANCODE(INTERNATIONAL9);
		REGISTER_SCANCODE(LANG1);
		REGISTER_SCANCODE(LANG2);
		REGISTER_SCANCODE(LANG3);
		REGISTER_SCANCODE(LANG4);
		REGISTER_SCANCODE(LANG5);
		REGISTER_SCANCODE(LANG6);
		REGISTER_SCANCODE(LANG7);
		REGISTER_SCANCODE(LANG8);
		REGISTER_SCANCODE(LANG9);
		REGISTER_SCANCODE(ALTERASE);
		REGISTER_SCANCODE(SYSREQ);
		REGISTER_SCANCODE(CANCEL);
		REGISTER_SCANCODE(CLEAR);
		REGISTER_SCANCODE(PRIOR);
		REGISTER_SCANCODE(RETURN2);
		REGISTER_SCANCODE(SEPARATOR);
		REGISTER_SCANCODE(OUT);
		REGISTER_SCANCODE(OPER);
		REGISTER_SCANCODE(CLEARAGAIN);
		REGISTER_SCANCODE(CRSEL);
		REGISTER_SCANCODE(EXSEL);
		REGISTER_SCANCODE(KP_00);
		REGISTER_SCANCODE(KP_000);
		REGISTER_SCANCODE(THOUSANDSSEPARATOR);
		REGISTER_SCANCODE(DECIMALSEPARATOR);
		REGISTER_SCANCODE(CURRENCYUNIT);
		REGISTER_SCANCODE(CURRENCYSUBUNIT);
		REGISTER_SCANCODE(KP_LEFTPAREN);
		REGISTER_SCANCODE(KP_RIGHTPAREN);
		REGISTER_SCANCODE(KP_LEFTBRACE);
		REGISTER_SCANCODE(KP_RIGHTBRACE);
		REGISTER_SCANCODE(KP_TAB);
		REGISTER_SCANCODE(KP_BACKSPACE);
		REGISTER_SCANCODE(KP_A);
		REGISTER_SCANCODE(KP_B);
		REGISTER_SCANCODE(KP_C);
		REGISTER_SCANCODE(KP_D);
		REGISTER_SCANCODE(KP_E);
		REGISTER_SCANCODE(KP_F);
		REGISTER_SCANCODE(KP_XOR);
		REGISTER_SCANCODE(KP_POWER);
		REGISTER_SCANCODE(KP_PERCENT);
		REGISTER_SCANCODE(KP_LESS);
		REGISTER_SCANCODE(KP_GREATER);
		REGISTER_SCANCODE(KP_AMPERSAND);
		REGISTER_SCANCODE(KP_DBLAMPERSAND);
		REGISTER_SCANCODE(KP_VERTICALBAR);
		REGISTER_SCANCODE(KP_DBLVERTICALBAR);
		REGISTER_SCANCODE(KP_COLON);
		REGISTER_SCANCODE(KP_HASH);
		REGISTER_SCANCODE(KP_SPACE);
		REGISTER_SCANCODE(KP_AT);
		REGISTER_SCANCODE(KP_EXCLAM);
		REGISTER_SCANCODE(KP_MEMSTORE);
		REGISTER_SCANCODE(KP_MEMRECALL);
		REGISTER_SCANCODE(KP_MEMCLEAR);
		REGISTER_SCANCODE(KP_MEMADD);
		REGISTER_SCANCODE(KP_MEMSUBTRACT);
		REGISTER_SCANCODE(KP_MEMMULTIPLY);
		REGISTER_SCANCODE(KP_MEMDIVIDE);
		REGISTER_SCANCODE(KP_PLUSMINUS);
		REGISTER_SCANCODE(KP_CLEAR);
		REGISTER_SCANCODE(KP_CLEARENTRY);
		REGISTER_SCANCODE(KP_BINARY);
		REGISTER_SCANCODE(KP_OCTAL);
		REGISTER_SCANCODE(KP_DECIMAL);
		REGISTER_SCANCODE(KP_HEXADECIMAL);
		REGISTER_SCANCODE(LCTRL);
		REGISTER_SCANCODE(LSHIFT);
		REGISTER_SCANCODE(LALT);
		REGISTER_SCANCODE(LGUI);
		REGISTER_SCANCODE(RCTRL);
		REGISTER_SCANCODE(RSHIFT);
		REGISTER_SCANCODE(RALT);
		REGISTER_SCANCODE(RGUI);
		REGISTER_SCANCODE(MODE);
		REGISTER_SCANCODE(AUDIONEXT);
		REGISTER_SCANCODE(AUDIOPREV);
		REGISTER_SCANCODE(AUDIOSTOP);
		REGISTER_SCANCODE(AUDIOPLAY);
		REGISTER_SCANCODE(AUDIOMUTE);
		REGISTER_SCANCODE(MEDIASELECT);
		REGISTER_SCANCODE(WWW);
		REGISTER_SCANCODE(MAIL);
		REGISTER_SCANCODE(CALCULATOR);
		REGISTER_SCANCODE(COMPUTER);
		REGISTER_SCANCODE(AC_SEARCH);
		REGISTER_SCANCODE(AC_HOME);
		REGISTER_SCANCODE(AC_BACK);
		REGISTER_SCANCODE(AC_FORWARD);
		REGISTER_SCANCODE(AC_STOP);
		REGISTER_SCANCODE(AC_REFRESH);
		REGISTER_SCANCODE(AC_BOOKMARKS);
		REGISTER_SCANCODE(BRIGHTNESSDOWN);
		REGISTER_SCANCODE(BRIGHTNESSUP);
		REGISTER_SCANCODE(DISPLAYSWITCH);
		REGISTER_SCANCODE(KBDILLUMTOGGLE);
		REGISTER_SCANCODE(KBDILLUMDOWN);
		REGISTER_SCANCODE(KBDILLUMUP);
		REGISTER_SCANCODE(EJECT);
		REGISTER_SCANCODE(SLEEP);
		REGISTER_SCANCODE(APP1);
		REGISTER_SCANCODE(APP2);
	
	#undef REGISTER_SCANCODE


	#define REGISTER_KEYCODE(KEYCODE) \
		LuaWrapper::createSystemVariable(L, "Engine", "INPUT_KEYCODE_" #KEYCODE, (int)SDLK_##KEYCODE);

		REGISTER_KEYCODE(RETURN);
		REGISTER_KEYCODE(ESCAPE);
		REGISTER_KEYCODE(BACKSPACE);
		REGISTER_KEYCODE(TAB);
		REGISTER_KEYCODE(SPACE);
		REGISTER_KEYCODE(EXCLAIM);
		REGISTER_KEYCODE(QUOTEDBL);
		REGISTER_KEYCODE(HASH);
		REGISTER_KEYCODE(PERCENT);
		REGISTER_KEYCODE(DOLLAR);
		REGISTER_KEYCODE(AMPERSAND);
		REGISTER_KEYCODE(QUOTE);
		REGISTER_KEYCODE(LEFTPAREN);
		REGISTER_KEYCODE(RIGHTPAREN);
		REGISTER_KEYCODE(ASTERISK);
		REGISTER_KEYCODE(PLUS);
		REGISTER_KEYCODE(COMMA);
		REGISTER_KEYCODE(MINUS);
		REGISTER_KEYCODE(PERIOD);
		REGISTER_KEYCODE(SLASH);
		REGISTER_KEYCODE(0);
		REGISTER_KEYCODE(1);
		REGISTER_KEYCODE(2);
		REGISTER_KEYCODE(3);
		REGISTER_KEYCODE(4);
		REGISTER_KEYCODE(5);
		REGISTER_KEYCODE(6);
		REGISTER_KEYCODE(7);
		REGISTER_KEYCODE(8);
		REGISTER_KEYCODE(9);
		REGISTER_KEYCODE(COLON);
		REGISTER_KEYCODE(SEMICOLON);
		REGISTER_KEYCODE(LESS);
		REGISTER_KEYCODE(EQUALS);
		REGISTER_KEYCODE(GREATER);
		REGISTER_KEYCODE(QUESTION);
		REGISTER_KEYCODE(AT);
		REGISTER_KEYCODE(LEFTBRACKET);
		REGISTER_KEYCODE(BACKSLASH);
		REGISTER_KEYCODE(RIGHTBRACKET);
		REGISTER_KEYCODE(CARET);
		REGISTER_KEYCODE(UNDERSCORE);
		REGISTER_KEYCODE(BACKQUOTE);
		REGISTER_KEYCODE(a);
		REGISTER_KEYCODE(b);
		REGISTER_KEYCODE(c);
		REGISTER_KEYCODE(d);
		REGISTER_KEYCODE(e);
		REGISTER_KEYCODE(f);
		REGISTER_KEYCODE(g);
		REGISTER_KEYCODE(h);
		REGISTER_KEYCODE(i);
		REGISTER_KEYCODE(j);
		REGISTER_KEYCODE(k);
		REGISTER_KEYCODE(l);
		REGISTER_KEYCODE(m);
		REGISTER_KEYCODE(n);
		REGISTER_KEYCODE(o);
		REGISTER_KEYCODE(p);
		REGISTER_KEYCODE(q);
		REGISTER_KEYCODE(r);
		REGISTER_KEYCODE(s);
		REGISTER_KEYCODE(t);
		REGISTER_KEYCODE(u);
		REGISTER_KEYCODE(v);
		REGISTER_KEYCODE(w);
		REGISTER_KEYCODE(x);
		REGISTER_KEYCODE(y);
		REGISTER_KEYCODE(z);
		REGISTER_KEYCODE(CAPSLOCK);
		REGISTER_KEYCODE(F1);
		REGISTER_KEYCODE(F2);
		REGISTER_KEYCODE(F3);
		REGISTER_KEYCODE(F4);
		REGISTER_KEYCODE(F5);
		REGISTER_KEYCODE(F6);
		REGISTER_KEYCODE(F7);
		REGISTER_KEYCODE(F8);
		REGISTER_KEYCODE(F9);
		REGISTER_KEYCODE(F10);
		REGISTER_KEYCODE(F11);
		REGISTER_KEYCODE(F12);
		REGISTER_KEYCODE(PRINTSCREEN);
		REGISTER_KEYCODE(SCROLLLOCK);
		REGISTER_KEYCODE(PAUSE);
		REGISTER_KEYCODE(INSERT);
		REGISTER_KEYCODE(HOME);
		REGISTER_KEYCODE(PAGEUP);
		REGISTER_KEYCODE(DELETE);
		REGISTER_KEYCODE(END);
		REGISTER_KEYCODE(PAGEDOWN);
		REGISTER_KEYCODE(RIGHT);
		REGISTER_KEYCODE(LEFT);
		REGISTER_KEYCODE(DOWN);
		REGISTER_KEYCODE(UP);
		REGISTER_KEYCODE(NUMLOCKCLEAR);
		REGISTER_KEYCODE(KP_DIVIDE);
		REGISTER_KEYCODE(KP_MULTIPLY);
		REGISTER_KEYCODE(KP_MINUS);
		REGISTER_KEYCODE(KP_PLUS);
		REGISTER_KEYCODE(KP_ENTER);
		REGISTER_KEYCODE(KP_1);
		REGISTER_KEYCODE(KP_2);
		REGISTER_KEYCODE(KP_3);
		REGISTER_KEYCODE(KP_4);
		REGISTER_KEYCODE(KP_5);
		REGISTER_KEYCODE(KP_6);
		REGISTER_KEYCODE(KP_7);
		REGISTER_KEYCODE(KP_8);
		REGISTER_KEYCODE(KP_9);
		REGISTER_KEYCODE(KP_0);
		REGISTER_KEYCODE(KP_PERIOD);
		REGISTER_KEYCODE(APPLICATION);
		REGISTER_KEYCODE(POWER);
		REGISTER_KEYCODE(KP_EQUALS);
		REGISTER_KEYCODE(F13);
		REGISTER_KEYCODE(F14);
		REGISTER_KEYCODE(F15);
		REGISTER_KEYCODE(F16);
		REGISTER_KEYCODE(F17);
		REGISTER_KEYCODE(F18);
		REGISTER_KEYCODE(F19);
		REGISTER_KEYCODE(F20);
		REGISTER_KEYCODE(F21);
		REGISTER_KEYCODE(F22);
		REGISTER_KEYCODE(F23);
		REGISTER_KEYCODE(F24);
		REGISTER_KEYCODE(EXECUTE);
		REGISTER_KEYCODE(HELP);
		REGISTER_KEYCODE(MENU);
		REGISTER_KEYCODE(SELECT);
		REGISTER_KEYCODE(STOP);
		REGISTER_KEYCODE(AGAIN);
		REGISTER_KEYCODE(UNDO);
		REGISTER_KEYCODE(CUT);
		REGISTER_KEYCODE(COPY);
		REGISTER_KEYCODE(PASTE);
		REGISTER_KEYCODE(FIND);
		REGISTER_KEYCODE(MUTE);
		REGISTER_KEYCODE(VOLUMEUP);
		REGISTER_KEYCODE(VOLUMEDOWN);
		REGISTER_KEYCODE(KP_COMMA);
		REGISTER_KEYCODE(KP_EQUALSAS400);
		REGISTER_KEYCODE(ALTERASE);
		REGISTER_KEYCODE(SYSREQ);
		REGISTER_KEYCODE(CANCEL);
		REGISTER_KEYCODE(CLEAR);
		REGISTER_KEYCODE(PRIOR);
		REGISTER_KEYCODE(RETURN2);
		REGISTER_KEYCODE(SEPARATOR);
		REGISTER_KEYCODE(OUT);
		REGISTER_KEYCODE(OPER);
		REGISTER_KEYCODE(CLEARAGAIN);
		REGISTER_KEYCODE(CRSEL);
		REGISTER_KEYCODE(EXSEL);
		REGISTER_KEYCODE(KP_00);
		REGISTER_KEYCODE(KP_000);
		REGISTER_KEYCODE(THOUSANDSSEPARATOR);
		REGISTER_KEYCODE(DECIMALSEPARATOR);
		REGISTER_KEYCODE(CURRENCYUNIT);
		REGISTER_KEYCODE(CURRENCYSUBUNIT);
		REGISTER_KEYCODE(KP_LEFTPAREN);
		REGISTER_KEYCODE(KP_RIGHTPAREN);
		REGISTER_KEYCODE(KP_LEFTBRACE);
		REGISTER_KEYCODE(KP_RIGHTBRACE);
		REGISTER_KEYCODE(KP_TAB);
		REGISTER_KEYCODE(KP_BACKSPACE);
		REGISTER_KEYCODE(KP_A);
		REGISTER_KEYCODE(KP_B);
		REGISTER_KEYCODE(KP_C);
		REGISTER_KEYCODE(KP_D);
		REGISTER_KEYCODE(KP_E);
		REGISTER_KEYCODE(KP_F);
		REGISTER_KEYCODE(KP_XOR);
		REGISTER_KEYCODE(KP_POWER);
		REGISTER_KEYCODE(KP_PERCENT);
		REGISTER_KEYCODE(KP_LESS);
		REGISTER_KEYCODE(KP_GREATER);
		REGISTER_KEYCODE(KP_AMPERSAND);
		REGISTER_KEYCODE(KP_DBLAMPERSAND);
		REGISTER_KEYCODE(KP_VERTICALBAR);
		REGISTER_KEYCODE(KP_DBLVERTICALBAR);
		REGISTER_KEYCODE(KP_COLON);
		REGISTER_KEYCODE(KP_HASH);
		REGISTER_KEYCODE(KP_SPACE);
		REGISTER_KEYCODE(KP_AT);
		REGISTER_KEYCODE(KP_EXCLAM);
		REGISTER_KEYCODE(KP_MEMSTORE);
		REGISTER_KEYCODE(KP_MEMRECALL);
		REGISTER_KEYCODE(KP_MEMCLEAR);
		REGISTER_KEYCODE(KP_MEMADD);
		REGISTER_KEYCODE(KP_MEMSUBTRACT);
		REGISTER_KEYCODE(KP_MEMMULTIPLY);
		REGISTER_KEYCODE(KP_MEMDIVIDE);
		REGISTER_KEYCODE(KP_PLUSMINUS);
		REGISTER_KEYCODE(KP_CLEAR);
		REGISTER_KEYCODE(KP_CLEARENTRY);
		REGISTER_KEYCODE(KP_BINARY);
		REGISTER_KEYCODE(KP_OCTAL);
		REGISTER_KEYCODE(KP_DECIMAL);
		REGISTER_KEYCODE(KP_HEXADECIMAL);
		REGISTER_KEYCODE(LCTRL);
		REGISTER_KEYCODE(LSHIFT);
		REGISTER_KEYCODE(LALT);
		REGISTER_KEYCODE(LGUI);
		REGISTER_KEYCODE(RCTRL);
		REGISTER_KEYCODE(RSHIFT);
		REGISTER_KEYCODE(RALT);
		REGISTER_KEYCODE(RGUI);
		REGISTER_KEYCODE(MODE);
		REGISTER_KEYCODE(AUDIONEXT);
		REGISTER_KEYCODE(AUDIOPREV);
		REGISTER_KEYCODE(AUDIOSTOP);
		REGISTER_KEYCODE(AUDIOPLAY);
		REGISTER_KEYCODE(AUDIOMUTE);
		REGISTER_KEYCODE(MEDIASELECT);
		REGISTER_KEYCODE(WWW);
		REGISTER_KEYCODE(MAIL);
		REGISTER_KEYCODE(CALCULATOR);
		REGISTER_KEYCODE(COMPUTER);
		REGISTER_KEYCODE(AC_SEARCH);
		REGISTER_KEYCODE(AC_HOME);
		REGISTER_KEYCODE(AC_BACK);
		REGISTER_KEYCODE(AC_FORWARD);
		REGISTER_KEYCODE(AC_STOP);
		REGISTER_KEYCODE(AC_REFRESH);
		REGISTER_KEYCODE(AC_BOOKMARKS);
		REGISTER_KEYCODE(BRIGHTNESSDOWN);
		REGISTER_KEYCODE(BRIGHTNESSUP);
		REGISTER_KEYCODE(DISPLAYSWITCH);
		REGISTER_KEYCODE(KBDILLUMTOGGLE);
		REGISTER_KEYCODE(KBDILLUMDOWN);
		REGISTER_KEYCODE(KBDILLUMUP);
		REGISTER_KEYCODE(EJECT);
		REGISTER_KEYCODE(SLEEP);

	#undef REGISTER_KEYCODE
}


InputSystem* InputSystem::create(Engine& engine)
{
	return LUMIX_NEW(engine.getAllocator(), InputSystemImpl)(engine);
}


void InputSystem::destroy(InputSystem& system)
{
	auto* impl = static_cast<InputSystemImpl*>(&system);
	LUMIX_DELETE(impl->m_allocator, impl);
}


} // namespace Lumix
