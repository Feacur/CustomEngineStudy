#pragma once
#define CUSTOM_FEATURE_RAW_INPUT

// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
// https://docs.microsoft.com/ru-ru/windows/desktop/inputdev/about-raw-input
// https://docs.microsoft.com/ru-ru/windows/desktop/inputdev/wm-input
// https://docs.microsoft.com/ru-ru/windows/desktop/inputdev/using-raw-input
// https://docs.microsoft.com/ru-ru/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use
// http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html

// namespace custom {

enum struct Raw_Input_Device_Usage : USHORT {
	Pointer  = 0x01,
	Mouse    = 0x02,
	Joystick = 0x04,
	Gamepad  = 0x05,
	Keyboard = 0x06,
};
UNDERLYING_TYPE_META(Raw_Input_Device_Usage, USHORT)
IS_ENUM_META(Raw_Input_Device_Usage)

// RIDEV_APPKEYS:      the application command keys are handled (only with keyboard's RIDEV_NOLEGACY)
// RIDEV_NOLEGACY:     prevents mouse and keyboard from generating legacy messages
// RIDEV_CAPTUREMOUSE: the mouse button click does not activate the other window

static constexpr inline RAWINPUTDEVICE raw_input_device(HWND hwnd, USHORT usage, DWORD flags) {
	RAWINPUTDEVICE device = {}; // ZeroMemory(&device, sizeof(device));
	device.usUsagePage = 0x01;
	device.usUsage     = usage;
	device.dwFlags     = flags;
	device.hwndTarget  = hwnd;
	return device;
}

static void platform_raw_input_init(HWND hwnd) {
	using U = meta::underlying_type<Raw_Input_Device_Usage>::type;
	RAWINPUTDEVICE devices[] = {
		raw_input_device(hwnd, (U)Raw_Input_Device_Usage::Keyboard, 0),
		raw_input_device(hwnd, (U)Raw_Input_Device_Usage::Mouse, 0),
	};
	RegisterRawInputDevices(devices, C_ARRAY_LENGTH(devices), sizeof(RAWINPUTDEVICE));
}

static void raw_input_callback(HWND hwnd, RAWMOUSE    const & data);
static void raw_input_callback(HWND hwnd, RAWKEYBOARD const & data);
static void raw_input_callback(HWND hwnd, RAWHID      const & data);
static void process_message_raw(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	char buffer[sizeof(RAWINPUT)]; // = {};
	ZeroMemory(buffer, sizeof(buffer));
	UINT size = sizeof(RAWINPUT);
	GetRawInputData(
		reinterpret_cast<HRAWINPUT>(lParam),
		RID_INPUT,
		buffer,
		&size,
		sizeof(RAWINPUTHEADER)
	);

	auto raw = reinterpret_cast<RAWINPUT *>(buffer);
	switch (raw->header.dwType) {
		case RIM_TYPEMOUSE:
			raw_input_callback(hwnd, raw->data.mouse);
			break;
		case RIM_TYPEKEYBOARD:
			raw_input_callback(hwnd, raw->data.keyboard);
			break;
		case RIM_TYPEHID:
			raw_input_callback(hwnd, raw->data.hid);
			break;
	}
}

// }
