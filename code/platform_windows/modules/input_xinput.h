#define FEATURE_XINPUT

#include <xinput.h>

#define XINPUT_GET_CAPABILITIES(ROUTINE_NAME) DWORD WINAPI ROUTINE_NAME(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES * pCapabilities)
#define XINPUT_GET_STATE(ROUTINE_NAME) DWORD WINAPI ROUTINE_NAME(DWORD dwUserIndex, XINPUT_STATE * pState)
#define XINPUT_SET_STATE(ROUTINE_NAME) DWORD WINAPI ROUTINE_NAME(DWORD dwUserIndex, XINPUT_VIBRATION * pVibration)

typedef XINPUT_GET_CAPABILITIES(XInputGetCapabilities_func);
typedef XINPUT_GET_STATE(XInputGetState_func);
typedef XINPUT_SET_STATE(XInputSetState_func);

XINPUT_GET_CAPABILITIES(XInputGetCapabilities_stub) { return ERROR_DEVICE_NOT_CONNECTED; }
XINPUT_GET_STATE(XInputGetState_stub) { return ERROR_DEVICE_NOT_CONNECTED; }
XINPUT_SET_STATE(XInputSetState_stub) { return ERROR_DEVICE_NOT_CONNECTED; }

static XInputGetCapabilities_func * DLL_XInputGetCapabilities;
static XInputGetState_func * DLL_XInputGetState;
static XInputSetState_func * DLL_XInputSetState;

#undef XINPUT_GET_CAPABILITIES
#undef XINPUT_GET_STATE
#undef XINPUT_SET_STATE

#define LOAD_PROCEDURE(ROUTINE_NAME)\
DLL_##ROUTINE_NAME = (ROUTINE_NAME##_func *)GetProcAddress(library, #ROUTINE_NAME);

#define VERIFY_PROCEDURE(ROUTINE_NAME)\
if (!DLL_##ROUTINE_NAME) { DLL_##ROUTINE_NAME = ROUTINE_NAME##_stub; }

void load_xinput()
{
	HMODULE library = 0;
	cstring libraries[] = {"xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll", "xinput1_3.dll", "xinput1_2.dll", "xinput1_1.dll", NULL};
	for (s32 i = 0; libraries[i]; ++i) {
		library = LoadLibrary(libraries[i]);
		if (library) { break; }
	}

	if (library) {
		LOAD_PROCEDURE(XInputGetCapabilities)
		LOAD_PROCEDURE(XInputGetState)
		LOAD_PROCEDURE(XInputSetState)
	}
	VERIFY_PROCEDURE(XInputGetCapabilities)
	VERIFY_PROCEDURE(XInputGetState)
	VERIFY_PROCEDURE(XInputSetState)
	
	CUSTOM_MESSAGE("Initialized xinput");
}

#undef LOAD_PROCEDURE
#undef VERIFY_PROCEDURE

void process_xinput_example() {
	for (DWORD controller_index = 0; controller_index < XUSER_MAX_COUNT; ++controller_index) {
		XINPUT_STATE controller_state;
		bool is_connected = (DLL_XInputGetState(controller_index, &controller_state) == ERROR_SUCCESS);
		if (!is_connected) { continue; }
		
		XINPUT_GAMEPAD gamepad = controller_state.Gamepad;
		
		bool dpad_up        = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		bool dpad_down      = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
		bool dpad_left      = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		bool dpad_right     = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
		bool start          = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_START);
		bool back           = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_BACK);
		bool left_thumb     = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		bool right_thumb    = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
		bool left_shoulder  = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		bool right_shoulder = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		bool button_1       = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_A);
		bool button_2       = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_B);
		bool button_3       = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_X);
		bool button_4       = BITS_ARE_SET(gamepad.wButtons, XINPUT_GAMEPAD_Y);
		
		// XINPUT_GAMEPAD_TRIGGER_THRESHOLD
		r32 trigger0 = uint8_to_fraction(gamepad.bLeftTrigger);
		r32 trigger1 = uint8_to_fraction(gamepad.bRightTrigger);
		
		// XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		// XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
		r32 thumb0x = int16_to_fraction(gamepad.sThumbLX);
		r32 thumb0y = int16_to_fraction(gamepad.sThumbLY);
		r32 thumb1x = int16_to_fraction(gamepad.sThumbRX);
		r32 thumb1y = int16_to_fraction(gamepad.sThumbRY);
	}
}
