struct Memory_Chunk {
	u8 * data;
	size_t  capacity, length;
};

inline void reset_memory_chunk(Memory_Chunk * memory) {
	memory->length = 0;
	// memset(memory->data, 0, memory->capacity);
	// memset(memory->data, 0, memory->length);
}

inline u8 * allocate_data(Memory_Chunk * memory, size_t size_in_bytes) {
	u8 * result = 0;
	if (memory->length + size_in_bytes <= memory->capacity) {
		result = memory->data + memory->length;
		memory->length += size_in_bytes;
	}
	else {
		CUSTOM_ASSERT(false, "Can't allocate a memory chunk");
	}
	return result;
}

inline void deallocate_from_top(Memory_Chunk * memory, size_t size_in_bytes) {
	if (memory->length >= size_in_bytes) {
		memory->length -= size_in_bytes;
	}
	else {
		CUSTOM_ASSERT(false, "Can't deallocate a memory chunk");
	}
}

#define ALLOCATE_STRUCT(MEMORY, TYPE)       (TYPE *)allocate_data(MEMORY, sizeof(TYPE))
#define ALLOCATE_ARRAY(MEMORY, TYPE, COUNT) (TYPE *)allocate_data(MEMORY, COUNT * sizeof(TYPE))
