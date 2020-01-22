#define FEATURE_DSOUND

#if defined(WIN32_LEAN_AND_MEAN)
#include <mmreg.h> // dsound requires some stuff from there
#endif

#include <dsound.h>

struct Sound_Buffer {
	Sound_Data sound;
	s32      bytes_per_sample;
	r32      buffer_length_in_seconds;
	DWORD      running_sample_byte;
	LPDIRECTSOUNDBUFFER secondary_buffer;
};

static Sound_Buffer sound_buffer;

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create_type);

void setup_sound_buffer(HWND window) {
	HMODULE library = LoadLibrary("dsound.dll");
	CUSTOM_ASSERT(library, "Can't load \"dsound.dll\"");
	
	auto DirectSoundCreate = (direct_sound_create_type *)(
		GetProcAddress(library, "DirectSoundCreate")
	);
	CUSTOM_ASSERT(library, "Can't load DirectSoundCreate");
	
	// initialize direct sound
	LPDIRECTSOUND direct_sound;
	HRESULT result_DirectSoundCreate = DirectSoundCreate(0, &direct_sound, 0);
	CUSTOM_ASSERT(result_DirectSoundCreate == DS_OK, "Can't create direct sound");
	
	HRESULT result_SetCooperativeLevel = direct_sound->SetCooperativeLevel(window, DSSCL_PRIORITY);
	CUSTOM_ASSERT(result_SetCooperativeLevel == DS_OK, "Can't set cooperative level");
	
	// primary buffer
	DSBUFFERDESC primary_buffer_description = {};
	primary_buffer_description.dwSize = sizeof(primary_buffer_description);
	primary_buffer_description.dwFlags = DSBCAPS_PRIMARYBUFFER;
	
	LPDIRECTSOUNDBUFFER primary_buffer;
	HRESULT result_primary_CreateSoundBuffer = direct_sound->CreateSoundBuffer(&primary_buffer_description, &primary_buffer, 0);
	CUSTOM_ASSERT(result_primary_CreateSoundBuffer == DS_OK, "Can't create primary sound buffer");
	
	// primary buffer: wave format
	WAVEFORMATEX wave_format = {};
	wave_format.wFormatTag = WAVE_FORMAT_PCM;
	wave_format.nChannels = (WORD)sound_buffer.sound.channels;
	wave_format.wBitsPerSample = (WORD)(sound_buffer.bytes_per_sample * 8);
	wave_format.nSamplesPerSec = sound_buffer.sound.samples_per_second;
	wave_format.nBlockAlign = (WORD)(sound_buffer.sound.channels * sound_buffer.bytes_per_sample);
	wave_format.nAvgBytesPerSec = wave_format.nBlockAlign * sound_buffer.sound.samples_per_second;
	wave_format.cbSize = 0;
	
	HRESULT result_SetFormat = primary_buffer->SetFormat(&wave_format);
	CUSTOM_ASSERT(result_SetFormat == DS_OK, "Can't set primary buffer wave format");
	
	// secondary buffer
	DSBUFFERDESC secondary_buffer_description = {};
	secondary_buffer_description.dwSize = sizeof(secondary_buffer_description);
	secondary_buffer_description.dwFlags = 0;
	secondary_buffer_description.dwBufferBytes = (DWORD)(wave_format.nAvgBytesPerSec * sound_buffer.buffer_length_in_seconds);
	secondary_buffer_description.lpwfxFormat = &wave_format;

	HRESULT result_secondary_CreateSoundBuffer = direct_sound->CreateSoundBuffer(&secondary_buffer_description, &sound_buffer.secondary_buffer, 0);
	CUSTOM_ASSERT(result_secondary_CreateSoundBuffer == DS_OK, "Can't create secondary sound buffer");
	
	// allocate memory
	sound_buffer.sound.size = secondary_buffer_description.dwBufferBytes;
	sound_buffer.sound.data = (s16 *)allocate_memory(secondary_buffer_description.dwBufferBytes);
	CUSTOM_ASSERT(sound_buffer.sound.data, "Can't allocate sound buffer memory");
}

void clear_sound_buffer() {
	// lock buffer's regions
	VOID *region1, *region2;
	DWORD region1_size, region2_size;
	HRESULT result_Lock = sound_buffer.secondary_buffer->Lock(
		0, 0,
		&region1, &region1_size,
		&region2, &region2_size,
		DSBLOCK_ENTIREBUFFER
	);
	CUSTOM_ASSERT(result_Lock == DS_OK, "Can't lock sound buffer");

	// fill buffer's regions
	memset(region1, 0, region1_size);
	memset(region2, 0, region2_size);

	// unlock buffer's regions
	HRESULT result_Unlock = sound_buffer.secondary_buffer->Unlock(
		region1, region1_size,
		region2, region2_size
	);
	CUSTOM_ASSERT(result_Unlock == DS_OK, "Can't unlock sound buffer");
}

void reinit_sound_buffer(HWND window) {
	if (sound_buffer.sound.data) {
		return;
	}

	sound_buffer.sound.channels = 2;
	sound_buffer.bytes_per_sample = sizeof(s16);
	sound_buffer.sound.samples_per_second = 48000;
	sound_buffer.buffer_length_in_seconds = 1;
	
	setup_sound_buffer(window);
	clear_sound_buffer();

	// reset play cursor
	HRESULT result_SetCurrentPosition = sound_buffer.secondary_buffer->SetCurrentPosition(0);
	CUSTOM_ASSERT(result_SetCurrentPosition == DS_OK, "Can't set current position in the sound buffer");

	// reset running sample
	DWORD play_cursor;
	DWORD write_cursor;
	HRESULT result_GetCurrentPosition = sound_buffer.secondary_buffer->GetCurrentPosition(&play_cursor, &write_cursor);
	CUSTOM_ASSERT(result_GetCurrentPosition == DS_OK, "Can't get current position in the sound buffer");

	sound_buffer.running_sample_byte = write_cursor;

	CUSTOM_MESSAGE("Initialized direct sound");
}

void play_sound_buffer() {
	DWORD status;
	HRESULT result_GetStatus = sound_buffer.secondary_buffer->GetStatus(&status);
	CUSTOM_ASSERT(result_GetStatus == DS_OK, "Can't get status of direct sound buffer");
	
	bool isPlaying = BITS_ARE_SET(status, DSBSTATUS_PLAYING);

	if (!isPlaying) {
		HRESULT result_Play = sound_buffer.secondary_buffer->Play(0, 0, DSBPLAY_LOOPING);
		CUSTOM_ASSERT(result_Play == DS_OK, "Can't play direct sound buffer");
		
		CUSTOM_MESSAGE("Playing direct sound buffer");
	}
	// else if (isPlaying) {
	// 	HRESULT result_Play = sound_buffer.secondary_buffer->Stop();
	// 	CUSTOM_ASSERT(result_Play == DS_OK, "Can't stop direct sound buffer");
	// }
}

void fill_sound_buffer(DWORD bytes_to_write) {
	s32 bytes_for_samples = sound_buffer.sound.channels * sound_buffer.bytes_per_sample;
	s32 bytes_for_second  = bytes_for_samples * sound_buffer.sound.samples_per_second;
	s32 bytes_for_buffer  = (s32)(bytes_for_second * sound_buffer.buffer_length_in_seconds);

	// lock buffer's regions
	VOID *region1, *region2;
	DWORD region1_size, region2_size;
	HRESULT result_Lock = sound_buffer.secondary_buffer->Lock(
		sound_buffer.running_sample_byte, bytes_to_write,
		&region1, &region1_size,
		&region2, &region2_size,
		0 // DSBLOCK_FROMWRITECURSOR
	);
	CUSTOM_ASSERT(result_Lock == DS_OK, "Can't lock sound buffer");

	// fill buffer's regions
	s8 *region1_in = (s8 *)sound_buffer.sound.data;
	s8 *region2_in = region1_in + region1_size;
	
	memcpy(region1, region1_in, region1_size);		
	memcpy(region2, region2_in, region2_size);
	
	// advance running sample
	sound_buffer.running_sample_byte = (
		sound_buffer.running_sample_byte + region1_size + region2_size
	) % bytes_for_buffer;
	
	// unlock buffer's regions
	HRESULT result_Unlock = sound_buffer.secondary_buffer->Unlock(
		region1, region1_size,
		region2, region2_size
	);
	CUSTOM_ASSERT(result_Unlock == DS_OK, "Can't unlock sound buffer");
}
