/********************************************************************
	created:	2011/10/14
	filename: 	GWInputDeviceEnum.h
	author:		Mwolf
	
	purpose:	Common definitions for input device
*********************************************************************/
#ifndef GWInputDeviceEnum_h__
#define GWInputDeviceEnum_h__

// Note: Key codes come from here:
//   http://www.webonweboff.com/tips/js/event_key_codes.aspx
// TODO: Fulfill this enum

enum GWKeyCode
{
	GW_KEY_UNDEFINED		= 0,

	GW_KEY_BACKSPACE		= 8,
	GW_KEY_TAB				= 9,

	GW_KEY_ENTER			= 13,

	GW_KEY_LSHIFT			= 16,
	GW_KEY_LCTRL			= 17,
	GW_KEY_LALT				= 18,
	GW_KEY_PAUSE			= 19,
	GW_KEY_CAPSLOCK			= 20,

	GW_KEY_ESC				= 27,

	GW_KEY_SPACE			= 32,

	GW_KEY_PAGEUP			= 33,
	GW_KEY_PAGEDOWN			= 34,
	GW_KEY_END				= 35,
	GW_KEY_HOME				= 36,

	GW_KEY_LEFT				= 37,
	GW_KEY_UP				= 38,
	GW_KEY_RIGHT			= 39,
	GW_KEY_DOWN				= 40,

	GW_KEY_INSERT			= 45,
	GW_KEY_DELETE			= 46,

	GW_KEY_0				= 48,
	GW_KEY_1				= 49,
	GW_KEY_2				= 50,
	GW_KEY_3				= 51,
	GW_KEY_4				= 52,
	GW_KEY_5				= 53,
	GW_KEY_6				= 54,
	GW_KEY_7				= 55,
	GW_KEY_8				= 56,
	GW_KEY_9				= 57,

	GW_KEY_A				= 65,
	GW_KEY_B				= 66,
	GW_KEY_C				= 67,
	GW_KEY_D				= 68,
	GW_KEY_E				= 69,
	GW_KEY_F				= 70,
	GW_KEY_G				= 71,
	GW_KEY_H				= 72,
	GW_KEY_I				= 73,
	GW_KEY_J				= 74,
	GW_KEY_K				= 75,
	GW_KEY_L				= 76,
	GW_KEY_M				= 77,
	GW_KEY_N				= 78,
	GW_KEY_O				= 79,
	GW_KEY_P				= 80,
	GW_KEY_Q				= 81,
	GW_KEY_R				= 82,
	GW_KEY_S				= 83,
	GW_KEY_T				= 84,
	GW_KEY_U				= 85,
	GW_KEY_V				= 86,
	GW_KEY_W				= 87,
	GW_KEY_X				= 88,
	GW_KEY_Y				= 89,
	GW_KEY_Z				= 90,

	GW_KEY_NUMPAD0			= 96,
	GW_KEY_NUMPAD1			= 97,
	GW_KEY_NUMPAD2			= 98,
	GW_KEY_NUMPAD3			= 99,
	GW_KEY_NUMPAD4			= 100,
	GW_KEY_NUMPAD5			= 101,
	GW_KEY_NUMPAD6			= 102,
	GW_KEY_NUMPAD7			= 103,
	GW_KEY_NUMPAD8			= 104,
	GW_KEY_NUMPAD9			= 105,
	GW_KEY_NUMPAD_STAR		= 106,
	GW_KEY_NUMPAD_PLUS		= 107,

	GW_KEY_NUMPAD_MINUS		= 109,
	GW_KEY_NUMPAD_DECIMAL	= 110,
	GW_KEY_NUMPAD_SLASH		= 111,

	GW_KEY_F1				= 112,
	GW_KEY_F2				= 113,
	GW_KEY_F3				= 114,
	GW_KEY_F4				= 115,
	GW_KEY_F5				= 116,
	GW_KEY_F6				= 117,
	GW_KEY_F7				= 118,
	GW_KEY_F8				= 119,
	GW_KEY_F9				= 120,
	GW_KEY_F10				= 121,
	GW_KEY_F11				= 122,
	GW_KEY_F12				= 123,

	GW_KEY_NUMLOCK			= 144,
	GW_KEY_SCROLL			= 145,
};

enum GWMouseButton
{
	MBTN_LEFT,
	MBTN_RIGHT,
	MBTN_MIDDLE,

	MBTN_COUNT,
};

#if defined GW_XINPUT
enum GWXInputControllerButton
{
	GW_XBTN_X,
	GW_XBTN_Y,
	GW_XBTN_A,
	GW_XBTN_B,

	GW_XBTN_DPAD_UP,
	GW_XBTN_DPAD_DOWN,
	GW_XBTN_DPAD_LEFT,
	GW_XBTN_DPAD_RIGHT,

	GW_XBTN_LB,
	GW_XBTN_RB,

	GW_XBTN_LTHUMB,
	GW_XBTN_RTHUMB,

	GW_XBTN_START,
	GW_XBTN_BACK,

	GW_XBTN_COUNT,
};

enum GWXInputAxis
{
	GW_XAXIS_LX,
	GW_XAXIS_LY,
	GW_XAXIS_RX,
	GW_XAXIS_RY,

	GW_XAXIS_LT,
	GW_XAXIS_RT,

	GW_XAXIS_COUNT,
};

#endif	// #if defined GW_XINPUT

#if defined GW_PLATFORM_PSP
enum GWPSPButton
{
	GW_PSPBTN_SQUARE,
	GW_PSPBTN_TRIANGLE,
	GW_PSPBTN_CIRCLE,
	GW_PSPBTN_CROSS,

	GW_PSPBTN_UP,
	GW_PSPBTN_DOWN,
	GW_PSPBTN_LEFT,
	GW_PSPBTN_RIGHT,

	GW_PSPBTN_L,
	GW_PSPBTN_R,

	GW_PSPBTN_START,
	GW_PSPBTN_SELECT,

	GW_PSPBTN_COUNT,
};

enum GWPSPAxis
{
	GW_PSPAXIS_LX,
	GW_PSPAXIS_LY,

	GW_PSPAXIS_COUNT,
};

#endif	// #if defined GW_PLATFORM_PSP

// State for key, mouse button and controller button
typedef enum GWButtonState
{
	GW_KEY_STATE_INVALID = -1,

	GW_KEY_STATE_UP = 0,
	GW_KEY_STATE_DOWN,
	GW_KEY_STATE_ON_PRESSED,
	GW_KEY_STATE_ON_RELEASED,
} GW_BUTTON_STATE;

typedef enum GWInputDeviceType
{
	GW_INPUT_DEVICE_KEYBOARD,
	GW_INPUT_DEVICE_MOUSE,
	GW_INPUT_DEVICE_CONTROLLER0,
	GW_INPUT_DEVICE_CONTROLLER1,

#if defined GW_PLATFORM_PSP
	GW_INPUT_DEVICE_PSP					= GW_INPUT_DEVICE_CONTROLLER0,
#endif	// #if defined GW_PLATFORM_PSP

#if defined GW_XINPUT
	GW_INPUT_DEVICE_XINPUT_CONTROLLER0	= GW_INPUT_DEVICE_CONTROLLER0,
#endif	// #if defined GW_XINPUT

} GW_INPUT_DEVICE;

#endif // GWInputDeviceEnum_h__
