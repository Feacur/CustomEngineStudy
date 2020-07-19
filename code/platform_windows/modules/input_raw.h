#define FEATURE_RAW_INPUT

// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
// https://docs.microsoft.com/en-us/windows/desktop/inputdev/about-raw-input
// https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-input
// https://docs.microsoft.com/en-us/windows/desktop/inputdev/using-raw-input
// https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use
// http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html

enum struct Raw_Input_Device_Usage : USHORT {
	pointer  = 0x01,
	mouse    = 0x02,
	joystick = 0x04,
	gamepad  = 0x05,
	keyboard = 0x06,
};
UNDERLYING_TYPE_META(Raw_Input_Device_Usage, USHORT)
IS_ENUM_META(Raw_Input_Device_Usage)

// RIDEV_APPKEYS:      the application command keys are handled (only with keyboard's RIDEV_NOLEGACY)
// RIDEV_NOLEGACY:     prevents mouse and keyboard from generating legacy messages
// RIDEV_CAPTUREMOUSE: the mouse button click does not activate the other window

constexpr inline RAWINPUTDEVICE raw_input_device(HWND window, USHORT usage, DWORD flags) {
	RAWINPUTDEVICE device = {};
	device.usUsagePage = 0x01;
	device.usUsage     = usage;
	device.dwFlags     = flags;
	device.hwndTarget  = window;
	return device;
}

void register_raw_input(HWND window) {
	using U = meta::underlying_type<Raw_Input_Device_Usage>::type;
	RAWINPUTDEVICE devices[] = {
		raw_input_device(window, (U)Raw_Input_Device_Usage::keyboard, 0),
		raw_input_device(window, (U)Raw_Input_Device_Usage::mouse, 0),
	};
	RegisterRawInputDevices(devices, C_ARRAY_LENGTH(devices), sizeof(RAWINPUTDEVICE));
}

void process_raw_input_callback(HWND window, RAWMOUSE    const & data);
void process_raw_input_callback(HWND window, RAWKEYBOARD const & data);
void process_raw_input_callback(HWND window, RAWHID      const & data);
void process_raw_input(HWND window, LPARAM lParam) {
	char buffer[sizeof(RAWINPUT)] = {};
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
			process_raw_input_callback(window, raw->data.mouse);
			break;
		case RIM_TYPEKEYBOARD:
			process_raw_input_callback(window, raw->data.keyboard);
			break;
		case RIM_TYPEHID:
			process_raw_input_callback(window, raw->data.hid);
			break;
	}
}
