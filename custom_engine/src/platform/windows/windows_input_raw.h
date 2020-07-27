#pragma once
#define CUSTOM_FEATURE_RAW_INPUT

// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
// https://docs.microsoft.com/ru-ru/windows/desktop/inputdev/about-raw-input
// https://docs.microsoft.com/ru-ru/windows/desktop/inputdev/wm-input
// https://docs.microsoft.com/ru-ru/windows/desktop/inputdev/using-raw-input
// https://docs.microsoft.com/ru-ru/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use
// http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html

// namespace custom {

enum struct Raw_Input_Device : USHORT { // usage page 0x01
	Pointer  = 0x01, // Win32  -- Exclusive
	Mouse    = 0x02, // Win32  -- Exclusive
	Joystick = 0x04, // DInput -- Shared
	Gamepad  = 0x05, // DInput -- Shared
	Keyboard = 0x06, // Win32  -- Exclusive
	Keypad   = 0x07, // Win32  -- Exclusive
	System   = 0x80, // Win32  -- Shared
};
UNDERLYING_TYPE_META(Raw_Input_Device, USHORT)
IS_ENUM_META(Raw_Input_Device)

// RIDEV_APPKEYS:      the application command keys are handled (only with keyboard's RIDEV_NOLEGACY)
// RIDEV_NOLEGACY:     prevents mouse and keyboard from generating legacy messages
// RIDEV_CAPTUREMOUSE: the mouse button click does not activate the other window

constexpr static inline RAWINPUTDEVICE raw_input_device(HWND hwnd, USHORT usage, DWORD flags) {
	RAWINPUTDEVICE device = {}; // ZeroMemory(&device, sizeof(device));
	device.usUsagePage = 0x01;
	device.usUsage     = usage;
	device.dwFlags     = flags;
	device.hwndTarget  = hwnd;
	return device;
}

static void platform_raw_input_set(HWND hwnd, DWORD flags, Input_Mode mode) {
	// https://docs.microsoft.com/ru-ru/windows/win32/api/winuser/ns-winuser-rawinputdevice
	using U = meta::underlying_type<Raw_Input_Device>::type;
	RAWINPUTDEVICE devices[] = {
		raw_input_device(hwnd, (U)Raw_Input_Device::Keyboard, flags),
		raw_input_device(hwnd, (U)Raw_Input_Device::Mouse, flags),
	};
	if (RegisterRawInputDevices(devices, C_ARRAY_LENGTH(devices), sizeof(RAWINPUTDEVICE))) {
		keyboard_mode = mode;
		mouse_mode    = mode;
	}
}

static void platform_raw_input_init(HWND hwnd) {
	// @Todo: fullscreen handling
	//        RIDEV_NOLEGACY | RIDEV_NOHOTKEYS | RIDEV_CAPTUREMOUSE
	platform_raw_input_set(hwnd, 0, Input_Mode::Raw);
}

static void platform_raw_input_shutdown(HWND hwnd) {
	platform_raw_input_set(NULL, RIDEV_REMOVE, Input_Mode::Message);
}

static void raw_input_callback(Window * window, RAWMOUSE    const & data);
static void raw_input_callback(Window * window, RAWKEYBOARD const & data);
static void raw_input_callback(Window * window, RAWHID      const & data);
static void process_message_raw(Window * window, WPARAM wParam, LPARAM lParam) {
	char buffer[sizeof(RAWINPUT)]; // = {};
	ZeroMemory(buffer, sizeof(buffer));
	UINT size = sizeof(RAWINPUT);
	GetRawInputData(
		(HRAWINPUT)lParam,
		RID_INPUT,
		buffer,
		&size,
		sizeof(RAWINPUTHEADER)
	);

	RAWINPUT * raw = (RAWINPUT *)buffer;
	switch (raw->header.dwType) {
		case RIM_TYPEMOUSE:
			raw_input_callback(window, raw->data.mouse);
			break;
		case RIM_TYPEKEYBOARD:
			raw_input_callback(window, raw->data.keyboard);
			break;
		case RIM_TYPEHID:
			raw_input_callback(window, raw->data.hid);
			break;
	}
}

// }
