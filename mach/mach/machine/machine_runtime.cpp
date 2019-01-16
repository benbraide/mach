#include "machine_runtime.h"

mach::machine::runtime::runtime(io::reader &reader){
	init_(reader);
}

mach::machine::runtime::runtime(qword entry)
	: entry_(entry){}

mach::machine::runtime::runtime(byte *data, std::size_t size){
	init_(data, size);
}

void mach::machine::runtime::run(){
	if (stack_ != nullptr)
		return;//Already running

	memory_block *module_block = nullptr;
	if (module_size_ != 0u){//Load module
		module_block = ((reader_ == nullptr) ? memory_.allocate_block(data_, module_size_) : memory_.allocate_block(module_size_));
		if (reader_ != nullptr)//Write module to memory
			memory_.copy(*reader_, module_block->get_address(), module_block->get_size());

		entry_ += module_block->get_address();
	}

	auto stack_block = memory_.allocate_block(stack_size_);
	stack_ = std::make_shared<stack>(stack_block->get_data(), stack_block->get_real_size());

	reg_table_.get_instruction_pointer()->write_scalar(entry_);
	while (stack_ != nullptr){
		switch (memory_.read_scalar<op_code>(reg_table_.get_instruction_pointer()->read_scalar<qword>())){
		case op_code::noop://No operation
			break;
		case op_code::mov:
			byte_code::mov_instruction::execute(memory_, reg_table_, *stack_);
			break;
		default:
			throw byte_code::instruction_error_code::unrecognized_instruction;
			break;
		}
	}

	memory_.deallocate_block(stack_block->get_address());
	if (module_block != nullptr)//Unload module
		memory_.deallocate_block(module_block->get_address());
}

void mach::machine::runtime::init_(io::reader &reader){
	prepare_(reader);
	reader_ = &reader;
}

void mach::machine::runtime::init_(byte *data, std::size_t size){
	io::binary_buffer_reader reader(data, size);
	prepare_(reader);
	data_ = (data + reader.get_offset());
}

void mach::machine::runtime::prepare_(io::reader &reader){
	if ((stack_size_ = reader.read_scalar<std::size_t>()) == 0u)
		stack_size_ = (1024u * 1024u);//Use default size -- 1MB

	module_size_ = reader.read_scalar<std::size_t>();
	entry_ = reader.read_scalar<qword>();
}

mach::machine::memory mach::machine::runtime::memory_;

std::size_t mach::machine::runtime::stack_size_ = 0u;
