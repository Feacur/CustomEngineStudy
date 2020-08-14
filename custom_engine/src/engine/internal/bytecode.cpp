#include "custom_pch.h"

#include "engine/impl/bytecode.h"

namespace custom {

Bytecode::Bytecode(void)
	: buffer()
	, offset(0)
{ }

void Bytecode::reset(void) {
	buffer.count = 0;
	offset = 0;
}

}
