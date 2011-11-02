/********************************************************************
	created:	2011/10/30
	filename: 	GWDeviceDirectInput.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "GWInputControl.h"
#include "Renderer/GWRenderWindow.h"

#include <dinput.h>

LPDIRECTINPUT8			dinput;
LPDIRECTINPUTDEVICE8	di_keyboard_device;
LPDIRECTINPUTDEVICE8	di_mouse_device;

BYTE					keystate[256];				// direct input key state table
DIMOUSESTATE			mousestate;

bool					di_key_down[0xFF];
GWButtonState			di_key_state[0xFF];		// GWKeyCode state table

bool					di_mbtn_down[MBTN_COUNT];
GWButtonState			di_mbtn_state[MBTN_COUNT];

GWKeyCode DI_GWKeyCodeTable[0xFF] = { GW_KEY_UNDEFINED };

static void SetupKeyCodeMap()
{
	// Initialize key map
	DI_GWKeyCodeTable[	DIK_ESCAPE	] = 	GW_KEY_UNDEFINED	; /*	0x01	*/	
	DI_GWKeyCodeTable[	DIK_1	] = 	GW_KEY_1	; /*	0x02	*/	
	DI_GWKeyCodeTable[	DIK_2	] = 	GW_KEY_2	; /*	0x03	*/	
	DI_GWKeyCodeTable[	DIK_3	] = 	GW_KEY_3	; /*	0x04	*/	
	DI_GWKeyCodeTable[	DIK_4	] = 	GW_KEY_4	; /*	0x05	*/	
	DI_GWKeyCodeTable[	DIK_5	] = 	GW_KEY_5	; /*	0x06	*/	
	DI_GWKeyCodeTable[	DIK_6	] = 	GW_KEY_6	; /*	0x07	*/	
	DI_GWKeyCodeTable[	DIK_7	] = 	GW_KEY_7	; /*	0x08	*/	
	DI_GWKeyCodeTable[	DIK_8	] = 	GW_KEY_8	; /*	0x09	*/	
	DI_GWKeyCodeTable[	DIK_9	] = 	GW_KEY_9	; /*	0x0A	*/	
	DI_GWKeyCodeTable[	DIK_0	] = 	GW_KEY_0	; /*	0x0B	*/	
	DI_GWKeyCodeTable[	DIK_MINUS	] = 	GW_KEY_UNDEFINED	; /*	0x0C	*/	/* - on main keyboard */
	DI_GWKeyCodeTable[	DIK_EQUALS	] = 	GW_KEY_UNDEFINED	; /*	0x0D	*/	
	DI_GWKeyCodeTable[	DIK_BACK	] = 	GW_KEY_BACKSPACE	; /*	0x0E	*/	/* backspace */
	DI_GWKeyCodeTable[	DIK_TAB	] = 	GW_KEY_TAB	; /*	0x0F	*/	
	DI_GWKeyCodeTable[	DIK_Q	] = 	GW_KEY_Q	; /*	0x10	*/	
	DI_GWKeyCodeTable[	DIK_W	] = 	GW_KEY_W	; /*	0x11	*/	
	DI_GWKeyCodeTable[	DIK_E	] = 	GW_KEY_E	; /*	0x12	*/	
	DI_GWKeyCodeTable[	DIK_R	] = 	GW_KEY_R	; /*	0x13	*/	
	DI_GWKeyCodeTable[	DIK_T	] = 	GW_KEY_T	; /*	0x14	*/	
	DI_GWKeyCodeTable[	DIK_Y	] = 	GW_KEY_Y	; /*	0x15	*/	
	DI_GWKeyCodeTable[	DIK_U	] = 	GW_KEY_U	; /*	0x16	*/	
	DI_GWKeyCodeTable[	DIK_I	] = 	GW_KEY_I	; /*	0x17	*/	
	DI_GWKeyCodeTable[	DIK_O	] = 	GW_KEY_O	; /*	0x18	*/	
	DI_GWKeyCodeTable[	DIK_P	] = 	GW_KEY_P	; /*	0x19	*/	
	DI_GWKeyCodeTable[	DIK_LBRACKET	] = 	GW_KEY_UNDEFINED	; /*	0x1A	*/	
	DI_GWKeyCodeTable[	DIK_RBRACKET	] = 	GW_KEY_UNDEFINED	; /*	0x1B	*/	
	DI_GWKeyCodeTable[	DIK_RETURN	] = 	GW_KEY_ENTER	; /*	0x1C	*/	/* Enter on main keyboard */
	DI_GWKeyCodeTable[	DIK_LCONTROL	] = 	GW_KEY_UNDEFINED	; /*	0x1D	*/	
	DI_GWKeyCodeTable[	DIK_A	] = 	GW_KEY_A	; /*	0x1E	*/	
	DI_GWKeyCodeTable[	DIK_S	] = 	GW_KEY_S	; /*	0x1F	*/	
	DI_GWKeyCodeTable[	DIK_D	] = 	GW_KEY_D	; /*	0x20	*/	
	DI_GWKeyCodeTable[	DIK_F	] = 	GW_KEY_F	; /*	0x21	*/	
	DI_GWKeyCodeTable[	DIK_G	] = 	GW_KEY_G	; /*	0x22	*/	
	DI_GWKeyCodeTable[	DIK_H	] = 	GW_KEY_H	; /*	0x23	*/	
	DI_GWKeyCodeTable[	DIK_J	] = 	GW_KEY_J	; /*	0x24	*/	
	DI_GWKeyCodeTable[	DIK_K	] = 	GW_KEY_K	; /*	0x25	*/	
	DI_GWKeyCodeTable[	DIK_L	] = 	GW_KEY_L	; /*	0x26	*/	
	DI_GWKeyCodeTable[	DIK_SEMICOLON	] = 	GW_KEY_UNDEFINED	; /*	0x27	*/	
	DI_GWKeyCodeTable[	DIK_APOSTROPHE	] = 	GW_KEY_UNDEFINED	; /*	0x28	*/	
	DI_GWKeyCodeTable[	DIK_GRAVE	] = 	GW_KEY_UNDEFINED	; /*	0x29	*/	/* accent grave */
	DI_GWKeyCodeTable[	DIK_LSHIFT	] = 	GW_KEY_UNDEFINED	; /*	0x2A	*/	
	DI_GWKeyCodeTable[	DIK_BACKSLASH	] = 	GW_KEY_UNDEFINED	; /*	0x2B	*/	
	DI_GWKeyCodeTable[	DIK_Z	] = 	GW_KEY_Z	; /*	0x2C	*/	
	DI_GWKeyCodeTable[	DIK_X	] = 	GW_KEY_X	; /*	0x2D	*/	
	DI_GWKeyCodeTable[	DIK_C	] = 	GW_KEY_C	; /*	0x2E	*/	
	DI_GWKeyCodeTable[	DIK_V	] = 	GW_KEY_V	; /*	0x2F	*/	
	DI_GWKeyCodeTable[	DIK_B	] = 	GW_KEY_B	; /*	0x30	*/	
	DI_GWKeyCodeTable[	DIK_N	] = 	GW_KEY_N	; /*	0x31	*/	
	DI_GWKeyCodeTable[	DIK_M	] = 	GW_KEY_M	; /*	0x32	*/	
	DI_GWKeyCodeTable[	DIK_COMMA	] = 	GW_KEY_UNDEFINED	; /*	0x33	*/	
	DI_GWKeyCodeTable[	DIK_PERIOD	] = 	GW_KEY_UNDEFINED	; /*	0x34	*/	/* . on main keyboard */
	DI_GWKeyCodeTable[	DIK_SLASH	] = 	GW_KEY_UNDEFINED	; /*	0x35	*/	/* / on main keyboard */
	DI_GWKeyCodeTable[	DIK_RSHIFT	] = 	GW_KEY_UNDEFINED	; /*	0x36	*/	
	DI_GWKeyCodeTable[	DIK_MULTIPLY	] = 	GW_KEY_UNDEFINED	; /*	0x37	*/	/* * on numeric keypad */
	DI_GWKeyCodeTable[	DIK_LMENU	] = 	GW_KEY_UNDEFINED	; /*	0x38	*/	/* left Alt */
	DI_GWKeyCodeTable[	DIK_SPACE	] = 	GW_KEY_SPACE	; /*	0x39	*/	
	DI_GWKeyCodeTable[	DIK_CAPITAL	] = 	GW_KEY_UNDEFINED	; /*	0x3A	*/	
	DI_GWKeyCodeTable[	DIK_F1	] = 	GW_KEY_UNDEFINED	; /*	0x3B	*/	
	DI_GWKeyCodeTable[	DIK_F2	] = 	GW_KEY_UNDEFINED	; /*	0x3C	*/	
	DI_GWKeyCodeTable[	DIK_F3	] = 	GW_KEY_UNDEFINED	; /*	0x3D	*/	
	DI_GWKeyCodeTable[	DIK_F4	] = 	GW_KEY_UNDEFINED	; /*	0x3E	*/	
	DI_GWKeyCodeTable[	DIK_F5	] = 	GW_KEY_UNDEFINED	; /*	0x3F	*/	
	DI_GWKeyCodeTable[	DIK_F6	] = 	GW_KEY_UNDEFINED	; /*	0x40	*/	
	DI_GWKeyCodeTable[	DIK_F7	] = 	GW_KEY_UNDEFINED	; /*	0x41	*/	
	DI_GWKeyCodeTable[	DIK_F8	] = 	GW_KEY_UNDEFINED	; /*	0x42	*/	
	DI_GWKeyCodeTable[	DIK_F9	] = 	GW_KEY_UNDEFINED	; /*	0x43	*/	
	DI_GWKeyCodeTable[	DIK_F10	] = 	GW_KEY_UNDEFINED	; /*	0x44	*/	
	DI_GWKeyCodeTable[	DIK_NUMLOCK	] = 	GW_KEY_UNDEFINED	; /*	0x45	*/	
	DI_GWKeyCodeTable[	DIK_SCROLL	] = 	GW_KEY_UNDEFINED	; /*	0x46	*/	/* Scroll Lock */
	DI_GWKeyCodeTable[	DIK_NUMPAD7	] = 	GW_KEY_UNDEFINED	; /*	0x47	*/	
	DI_GWKeyCodeTable[	DIK_NUMPAD8	] = 	GW_KEY_UNDEFINED	; /*	0x48	*/	
	DI_GWKeyCodeTable[	DIK_NUMPAD9	] = 	GW_KEY_UNDEFINED	; /*	0x49	*/	
	DI_GWKeyCodeTable[	DIK_SUBTRACT	] = 	GW_KEY_UNDEFINED	; /*	0x4A	*/	/* - on numeric keypad */
	DI_GWKeyCodeTable[	DIK_NUMPAD4	] = 	GW_KEY_UNDEFINED	; /*	0x4B	*/	
	DI_GWKeyCodeTable[	DIK_NUMPAD5	] = 	GW_KEY_UNDEFINED	; /*	0x4C	*/	
	DI_GWKeyCodeTable[	DIK_NUMPAD6	] = 	GW_KEY_UNDEFINED	; /*	0x4D	*/	
	DI_GWKeyCodeTable[	DIK_ADD	] = 	GW_KEY_UNDEFINED	; /*	0x4E	*/	/* + on numeric keypad */
	DI_GWKeyCodeTable[	DIK_NUMPAD1	] = 	GW_KEY_UNDEFINED	; /*	0x4F	*/	
	DI_GWKeyCodeTable[	DIK_NUMPAD2	] = 	GW_KEY_UNDEFINED	; /*	0x50	*/	
	DI_GWKeyCodeTable[	DIK_NUMPAD3	] = 	GW_KEY_UNDEFINED	; /*	0x51	*/	
	DI_GWKeyCodeTable[	DIK_NUMPAD0	] = 	GW_KEY_UNDEFINED	; /*	0x52	*/	
	DI_GWKeyCodeTable[	DIK_DECIMAL	] = 	GW_KEY_UNDEFINED	; /*	0x53	*/	/* . on numeric keypad */
	DI_GWKeyCodeTable[	DIK_OEM_102	] = 	GW_KEY_UNDEFINED	; /*	0x56	*/	/* <> or \| on RT 102-key keyboard (Non-U.S.) */
	DI_GWKeyCodeTable[	DIK_F11	] = 	GW_KEY_UNDEFINED	; /*	0x57	*/	
	DI_GWKeyCodeTable[	DIK_F12	] = 	GW_KEY_UNDEFINED	; /*	0x58	*/	
	DI_GWKeyCodeTable[	DIK_F13	] = 	GW_KEY_UNDEFINED	; /*	0x64	*/	/*                     (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_F14	] = 	GW_KEY_UNDEFINED	; /*	0x65	*/	/*                     (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_F15	] = 	GW_KEY_UNDEFINED	; /*	0x66	*/	/*                     (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_KANA	] = 	GW_KEY_UNDEFINED	; /*	0x70	*/	/* (Japanese keyboard)            */
	DI_GWKeyCodeTable[	DIK_ABNT_C1	] = 	GW_KEY_UNDEFINED	; /*	0x73	*/	/* /? on Brazilian keyboard */
	DI_GWKeyCodeTable[	DIK_CONVERT	] = 	GW_KEY_UNDEFINED	; /*	0x79	*/	/* (Japanese keyboard)            */
	DI_GWKeyCodeTable[	DIK_NOCONVERT	] = 	GW_KEY_UNDEFINED	; /*	0x7B	*/	/* (Japanese keyboard)            */
	DI_GWKeyCodeTable[	DIK_YEN	] = 	GW_KEY_UNDEFINED	; /*	0x7D	*/	/* (Japanese keyboard)            */
	DI_GWKeyCodeTable[	DIK_ABNT_C2	] = 	GW_KEY_UNDEFINED	; /*	0x7E	*/	/* Numpad . on Brazilian keyboard */
	DI_GWKeyCodeTable[	DIK_NUMPADEQUALS	] = 	GW_KEY_UNDEFINED	; /*	0x8D	*/	/* = on numeric keypad (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_PREVTRACK	] = 	GW_KEY_UNDEFINED	; /*	0x90	*/	/* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	DI_GWKeyCodeTable[	DIK_AT	] = 	GW_KEY_UNDEFINED	; /*	0x91	*/	/*                     (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_COLON	] = 	GW_KEY_UNDEFINED	; /*	0x92	*/	/*                     (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_UNDERLINE	] = 	GW_KEY_UNDEFINED	; /*	0x93	*/	/*                     (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_KANJI	] = 	GW_KEY_UNDEFINED	; /*	0x94	*/	/* (Japanese keyboard)            */
	DI_GWKeyCodeTable[	DIK_STOP	] = 	GW_KEY_UNDEFINED	; /*	0x95	*/	/*                     (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_AX	] = 	GW_KEY_UNDEFINED	; /*	0x96	*/	/*                     (Japan AX) */
	DI_GWKeyCodeTable[	DIK_UNLABELED	] = 	GW_KEY_UNDEFINED	; /*	0x97	*/	/*                        (J3100) */
	DI_GWKeyCodeTable[	DIK_NEXTTRACK	] = 	GW_KEY_UNDEFINED	; /*	0x99	*/	/* Next Track */
	DI_GWKeyCodeTable[	DIK_NUMPADENTER	] = 	GW_KEY_UNDEFINED	; /*	0x9C	*/	/* Enter on numeric keypad */
	DI_GWKeyCodeTable[	DIK_RCONTROL	] = 	GW_KEY_UNDEFINED	; /*	0x9D	*/	
	DI_GWKeyCodeTable[	DIK_MUTE	] = 	GW_KEY_UNDEFINED	; /*	0xA0	*/	/* Mute */
	DI_GWKeyCodeTable[	DIK_CALCULATOR	] = 	GW_KEY_UNDEFINED	; /*	0xA1	*/	/* Calculator */
	DI_GWKeyCodeTable[	DIK_PLAYPAUSE	] = 	GW_KEY_UNDEFINED	; /*	0xA2	*/	/* Play / Pause */
	DI_GWKeyCodeTable[	DIK_MEDIASTOP	] = 	GW_KEY_UNDEFINED	; /*	0xA4	*/	/* Media Stop */
	DI_GWKeyCodeTable[	DIK_VOLUMEDOWN	] = 	GW_KEY_UNDEFINED	; /*	0xAE	*/	/* Volume - */
	DI_GWKeyCodeTable[	DIK_VOLUMEUP	] = 	GW_KEY_UNDEFINED	; /*	0xB0	*/	/* Volume + */
	DI_GWKeyCodeTable[	DIK_WEBHOME	] = 	GW_KEY_UNDEFINED	; /*	0xB2	*/	/* Web home */
	DI_GWKeyCodeTable[	DIK_NUMPADCOMMA	] = 	GW_KEY_UNDEFINED	; /*	0xB3	*/	/* , on numeric keypad (NEC PC98) */
	DI_GWKeyCodeTable[	DIK_DIVIDE	] = 	GW_KEY_UNDEFINED	; /*	0xB5	*/	/* / on numeric keypad */
	DI_GWKeyCodeTable[	DIK_SYSRQ	] = 	GW_KEY_UNDEFINED	; /*	0xB7	*/	
	DI_GWKeyCodeTable[	DIK_RMENU	] = 	GW_KEY_UNDEFINED	; /*	0xB8	*/	/* right Alt */
	DI_GWKeyCodeTable[	DIK_PAUSE	] = 	GW_KEY_UNDEFINED	; /*	0xC5	*/	/* Pause */
	DI_GWKeyCodeTable[	DIK_HOME	] = 	GW_KEY_UNDEFINED	; /*	0xC7	*/	/* Home on arrow keypad */
	DI_GWKeyCodeTable[	DIK_UP	] = 	GW_KEY_UP	; /*	0xC8	*/	/* UpArrow on arrow keypad */
	DI_GWKeyCodeTable[	DIK_PRIOR	] = 	GW_KEY_UNDEFINED	; /*	0xC9	*/	/* PgUp on arrow keypad */
	DI_GWKeyCodeTable[	DIK_LEFT	] = 	GW_KEY_LEFT	; /*	0xCB	*/	/* LeftArrow on arrow keypad */
	DI_GWKeyCodeTable[	DIK_RIGHT	] = 	GW_KEY_RIGHT	; /*	0xCD	*/	/* RightArrow on arrow keypad */
	DI_GWKeyCodeTable[	DIK_END	] = 	GW_KEY_UNDEFINED	; /*	0xCF	*/	/* End on arrow keypad */
	DI_GWKeyCodeTable[	DIK_DOWN	] = 	GW_KEY_DOWN	; /*	0xD0	*/	/* DownArrow on arrow keypad */
	DI_GWKeyCodeTable[	DIK_NEXT	] = 	GW_KEY_UNDEFINED	; /*	0xD1	*/	/* PgDn on arrow keypad */
	DI_GWKeyCodeTable[	DIK_INSERT	] = 	GW_KEY_UNDEFINED	; /*	0xD2	*/	/* Insert on arrow keypad */
	DI_GWKeyCodeTable[	DIK_DELETE	] = 	GW_KEY_UNDEFINED	; /*	0xD3	*/	/* Delete on arrow keypad */
	DI_GWKeyCodeTable[	DIK_LWIN	] = 	GW_KEY_UNDEFINED	; /*	0xDB	*/	/* Left Windows key */
	DI_GWKeyCodeTable[	DIK_RWIN	] = 	GW_KEY_UNDEFINED	; /*	0xDC	*/	/* Right Windows key */
	DI_GWKeyCodeTable[	DIK_APPS	] = 	GW_KEY_UNDEFINED	; /*	0xDD	*/	/* AppMenu key */
	DI_GWKeyCodeTable[	DIK_POWER	] = 	GW_KEY_UNDEFINED	; /*	0xDE	*/	/* System Power */
	DI_GWKeyCodeTable[	DIK_SLEEP	] = 	GW_KEY_UNDEFINED	; /*	0xDF	*/	/* System Sleep */
	DI_GWKeyCodeTable[	DIK_WAKE	] = 	GW_KEY_UNDEFINED	; /*	0xE3	*/	/* System Wake */
	DI_GWKeyCodeTable[	DIK_WEBSEARCH	] = 	GW_KEY_UNDEFINED	; /*	0xE5	*/	/* Web Search */
	DI_GWKeyCodeTable[	DIK_WEBFAVORITES	] = 	GW_KEY_UNDEFINED	; /*	0xE6	*/	/* Web Favorites */
	DI_GWKeyCodeTable[	DIK_WEBREFRESH	] = 	GW_KEY_UNDEFINED	; /*	0xE7	*/	/* Web Refresh */
	DI_GWKeyCodeTable[	DIK_WEBSTOP	] = 	GW_KEY_UNDEFINED	; /*	0xE8	*/	/* Web Stop */
	DI_GWKeyCodeTable[	DIK_WEBFORWARD	] = 	GW_KEY_UNDEFINED	; /*	0xE9	*/	/* Web Forward */
	DI_GWKeyCodeTable[	DIK_WEBBACK	] = 	GW_KEY_UNDEFINED	; /*	0xEA	*/	/* Web Back */
	DI_GWKeyCodeTable[	DIK_MYCOMPUTER	] = 	GW_KEY_UNDEFINED	; /*	0xEB	*/	/* My Computer */
	DI_GWKeyCodeTable[	DIK_MAIL	] = 	GW_KEY_UNDEFINED	; /*	0xEC	*/	/* Mail */
	DI_GWKeyCodeTable[	DIK_MEDIASELECT	] = 	GW_KEY_UNDEFINED	; /*	0xED	*/	/* Media Select */
}

void GWInput_InitializeDevice( GW_RENDER_WINDOW* rw )
{
	SetupKeyCodeMap();

	memset(di_key_state, 0, sizeof(di_key_state));
	memset(di_key_down, 0, sizeof(di_key_down));
	memset(di_mbtn_down, 0, sizeof(di_mbtn_down));
	memset(di_mbtn_state, 0, sizeof(di_mbtn_state));

	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hWnd = *((HWND*)rw->params[WND_PARAM_WIN32_HANDLE]);

	// Create direct input
	DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
						(void**)&dinput, NULL );

	dinput->CreateDevice( GUID_SysKeyboard, &di_keyboard_device, NULL );
	dinput->CreateDevice( GUID_SysMouse, &di_mouse_device, NULL );

	di_keyboard_device->SetDataFormat( &c_dfDIKeyboard );
	di_keyboard_device->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );

	di_mouse_device->SetDataFormat( &c_dfDIMouse );
	di_mouse_device->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
}

void GWInput_UpdateInputState()
{
	// Handle input
	di_keyboard_device->Acquire();
	di_keyboard_device->GetDeviceState( 256, (LPVOID)keystate );

	for (int i=0; i<0xFF; i++)
	{
		bool key_down = ((keystate[i] & 0x80) != 0);
		if (di_key_down[DI_GWKeyCodeTable[i]] != key_down)
		{
			di_key_state[DI_GWKeyCodeTable[i]] = key_down ? GW_KEY_STATE_ON_PRESSED : GW_KEY_STATE_ON_RELEASED;
		}
		else
		{
			di_key_state[DI_GWKeyCodeTable[i]] = key_down ? GW_KEY_STATE_DOWN : GW_KEY_STATE_UP;
		}
		di_key_down[DI_GWKeyCodeTable[i]] = key_down;
	}

	di_mouse_device->Acquire();
	di_mouse_device->GetDeviceState( sizeof(DIMOUSESTATE), (LPVOID)&mousestate );

	for (int i=0; i<MBTN_COUNT; i++)
	{
		bool btn_down = ((mousestate.rgbButtons[i] & 0x80) != 0);
		if (di_mbtn_down[i] != btn_down)
		{
			di_mbtn_state[i] = btn_down ? GW_KEY_STATE_ON_PRESSED : GW_KEY_STATE_ON_RELEASED;
		}
		else
		{
			di_mbtn_state[i] = btn_down ? GW_KEY_STATE_DOWN : GW_KEY_STATE_UP;
		}
		di_mbtn_down[i] = btn_down;
	}
}

void GWInput_DestroyDevice()
{
	di_keyboard_device->Unacquire();
	di_mouse_device->Unacquire();
	dinput->Release();
}

GWButtonState GWInput_GetKeyState(int key)
{
	return (key >= 0 && key < 0xFF) ? di_key_state[key] : GW_KEY_STATE_INVALID;
}

GWButtonState GWInput_GetMouseBtnState(int btn)
{
	return (btn >= 0 && btn < MBTN_COUNT) ? di_mbtn_state[btn] : GW_KEY_STATE_INVALID;
}

int GWInput_GetMouseWheelValue()
{
	return mousestate.lZ;
}
