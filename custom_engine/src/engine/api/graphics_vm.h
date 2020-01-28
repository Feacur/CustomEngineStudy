#pragma once

namespace custom {
struct Bytecode;
}

namespace custom {
namespace graphics {

struct VM
{
	VM();
	~VM();

	void update(Bytecode const & bc);
};

}}
