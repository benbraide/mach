#include "machine_register_table.h"

mach::machine::register_table::register_table(){
	registers_["rax"] = registers_["r0"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(0));
	registers_["eax"] = registers_["r0d"] = std::make_shared<dword_integer_register>(*registers_["r0"]);
	registers_["ax"] = registers_["r0w"] = std::make_shared<word_integer_register>(*registers_["r0"]);
	registers_["al"] = registers_["r0b"] = std::make_shared<byte_integer_register>(*registers_["r0"]);

	registers_["rbx"] = registers_["r1"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(1));
	registers_["ebx"] = registers_["r1d"] = std::make_shared<dword_integer_register>(*registers_["r1"]);
	registers_["bx"] = registers_["r1w"] = std::make_shared<word_integer_register>(*registers_["r1"]);
	registers_["bl"] = registers_["r1b"] = std::make_shared<byte_integer_register>(*registers_["r1"]);

	registers_["rcx"] = registers_["r2"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(2));
	registers_["ecx"] = registers_["r2d"] = std::make_shared<dword_integer_register>(*registers_["r2"]);
	registers_["cx"] = registers_["r2w"] = std::make_shared<word_integer_register>(*registers_["r2"]);
	registers_["cl"] = registers_["r2b"] = std::make_shared<byte_integer_register>(*registers_["r2"]);

	registers_["rdx"] = registers_["r3"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(3));
	registers_["edx"] = registers_["r3d"] = std::make_shared<dword_integer_register>(*registers_["r3"]);
	registers_["dx"] = registers_["r3w"] = std::make_shared<word_integer_register>(*registers_["r3"]);
	registers_["al"] = registers_["r3b"] = std::make_shared<byte_integer_register>(*registers_["r3"]);

	registers_["rsp"] = registers_["r4"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(4));
	registers_["esp"] = registers_["r4d"] = std::make_shared<dword_integer_register>(*registers_["r4"]);
	registers_["sp"] = registers_["r4w"] = std::make_shared<word_integer_register>(*registers_["r4"]);
	registers_["spl"] = registers_["r4b"] = std::make_shared<byte_integer_register>(*registers_["r4"]);

	registers_["rbp"] = registers_["r5"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(5));
	registers_["ebp"] = registers_["r5d"] = std::make_shared<dword_integer_register>(*registers_["r5"]);
	registers_["bp"] = registers_["r5w"] = std::make_shared<word_integer_register>(*registers_["r5"]);
	registers_["bpl"] = registers_["r5b"] = std::make_shared<byte_integer_register>(*registers_["r5"]);

	registers_["rsi"] = registers_["r6"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(6));
	registers_["esi"] = registers_["r6d"] = std::make_shared<dword_integer_register>(*registers_["r6"]);
	registers_["si"] = registers_["r6w"] = std::make_shared<word_integer_register>(*registers_["r6"]);
	registers_["sil"] = registers_["r6b"] = std::make_shared<byte_integer_register>(*registers_["r6"]);

	registers_["rdi"] = registers_["r7"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(7));
	registers_["edi"] = registers_["r7d"] = std::make_shared<dword_integer_register>(*registers_["r7"]);
	registers_["di"] = registers_["r7w"] = std::make_shared<word_integer_register>(*registers_["r7"]);
	registers_["dil"] = registers_["r7b"] = std::make_shared<byte_integer_register>(*registers_["r7"]);

	registers_["rip"] = registers_["r8"] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(8));
	registers_["eip"] = registers_["r8d"] = std::make_shared<dword_integer_register>(*registers_["r8"]);
	registers_["ip"] = registers_["r8w"] = std::make_shared<word_integer_register>(*registers_["r8"]);
	registers_["ipl"] = registers_["r8b"] = std::make_shared<byte_integer_register>(*registers_["r8"]);

	std::string key;
	for (auto index = 9; index < 16; ++index){
		key = ("r" + std::to_string(index));
		registers_[key] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(index));
		registers_[(key + "d")] = std::make_shared<dword_integer_register>(*registers_[key]);
		registers_[(key + "w")] = std::make_shared<word_integer_register>(*registers_[key]);
		registers_[(key + "b")] = std::make_shared<byte_integer_register>(*registers_[key]);
	}

	for (auto index = 0; index < 8; ++index){
		key = ("xmm" + std::to_string(index));
		registers_[key] = std::make_shared<qword_integer_register>(static_cast<register_object::byte>(index + 16));
		registers_[(key + "d")] = std::make_shared<dword_integer_register>(*registers_[key]);
	}

	cache_.base_pointer = registers_["rbp"].get();
	cache_.stack_pointer = registers_["rsp"].get();
	cache_.instruction_pointer = registers_["rip"].get();
}

mach::machine::register_object *mach::machine::register_table::find(const std::string &key) const{
	auto it = registers_.find(key);
	return ((it == registers_.end()) ? nullptr : it->second.get());
}

mach::machine::register_object *mach::machine::register_table::get_base_pointer() const{
	return cache_.base_pointer;
}

mach::machine::register_object *mach::machine::register_table::get_stack_pointer() const{
	return cache_.stack_pointer;
}

mach::machine::register_object *mach::machine::register_table::get_instruction_pointer() const{
	return cache_.instruction_pointer;
}

void mach::machine::register_table::set_flag(unsigned int flag){
	flags_ |= flag;
}

void mach::machine::register_table::clear_flag(unsigned int flag){
	flags_ &= ~flag;
}

bool mach::machine::register_table::has_flag(unsigned int flag) const{
	return ((flags_ & flag) == flag);
}
