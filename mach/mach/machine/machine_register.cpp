#include "machine_register.h"

std::size_t mach::machine::register_object::get_bytes_remaining() const{
	return get_size();
}

mach::machine::byte_integer_register::byte_integer_register(word_integer_register &parent)
	: parent_(parent){}

mach::machine::register_object::byte *mach::machine::byte_integer_register::get_data() const{
	return parent_.get_data();
}

std::size_t mach::machine::byte_integer_register::read(byte *buffer, std::size_t size) const{
	return parent_.read(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

std::size_t mach::machine::byte_integer_register::write(const byte *buffer, std::size_t size){
	return parent_.write(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

mach::machine::word_integer_register::word_integer_register(dword_integer_register &parent)
	: parent_(parent){
	child_.reset(new byte_integer_register(*this));
}

mach::machine::register_object::byte *mach::machine::word_integer_register::get_data() const{
	return parent_.get_data();
}

std::size_t mach::machine::word_integer_register::read(byte *buffer, std::size_t size) const{
	return parent_.read(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

std::size_t mach::machine::word_integer_register::write(const byte *buffer, std::size_t size){
	return parent_.write(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

mach::machine::generic_register<unsigned __int8, unsigned __int64> *mach::machine::word_integer_register::get_child() const{
	return child_.get();
}

mach::machine::dword_integer_register::dword_integer_register(qword_integer_register &parent)
	: parent_(parent){
	child_.reset(new word_integer_register(*this));
}

mach::machine::register_object::byte *mach::machine::dword_integer_register::get_data() const{
	return parent_.get_data();
}

std::size_t mach::machine::dword_integer_register::read(byte *buffer, std::size_t size) const{
	return parent_.read(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

std::size_t mach::machine::dword_integer_register::write(const byte *buffer, std::size_t size){
	return parent_.write(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

mach::machine::generic_register<unsigned __int16, unsigned __int64> *mach::machine::dword_integer_register::get_child() const{
	return child_.get();
}

mach::machine::qword_integer_register::qword_integer_register(){
	child_.reset(new dword_integer_register(*this));
}

mach::machine::register_object::byte *mach::machine::qword_integer_register::get_data() const{
	return const_cast<byte *>(reinterpret_cast<const byte *>(&buffer_));
}

std::size_t mach::machine::qword_integer_register::read(byte *buffer, std::size_t size) const{
	if (sizeof(m_buffer_type) < size)//Restrict size
		size = sizeof(m_buffer_type);

	memcpy(buffer, &buffer_, size);
	return size;
}

std::size_t mach::machine::qword_integer_register::write(const byte *buffer, std::size_t size){
	if (sizeof(m_buffer_type) < size)//Restrict size
		size = sizeof(m_buffer_type);

	memcpy(&buffer_, buffer, size);
	return size;
}

mach::machine::generic_register<unsigned __int32, unsigned __int64> *mach::machine::qword_integer_register::get_child() const{
	return child_.get();
}

mach::machine::dword_float_register::dword_float_register(qword_float_register &parent)
	: parent_(parent){}

mach::machine::register_object::byte *mach::machine::dword_float_register::get_data() const{
	return parent_.get_data();
}

std::size_t mach::machine::dword_float_register::read(byte *buffer, std::size_t size) const{
	return parent_.read(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

std::size_t mach::machine::dword_float_register::write(const byte *buffer, std::size_t size){
	return parent_.write(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
}

mach::machine::qword_float_register::qword_float_register(){
	child_.reset(new dword_float_register(*this));
}

mach::machine::register_object::byte *mach::machine::qword_float_register::get_data() const{
	return const_cast<byte *>(reinterpret_cast<const byte *>(&buffer_));
}

std::size_t mach::machine::qword_float_register::read(byte *buffer, std::size_t size) const{
	if (sizeof(m_buffer_type) < size)//Restrict size
		size = sizeof(m_buffer_type);

	memcpy(buffer, &buffer_, size);
	return size;
}

std::size_t mach::machine::qword_float_register::write(const byte *buffer, std::size_t size){
	if (sizeof(m_buffer_type) < size)//Restrict size
		size = sizeof(m_buffer_type);

	memcpy(&buffer_, buffer, size);
	return size;
}

mach::machine::generic_register<float, long double> *mach::machine::qword_float_register::get_child() const{
	return child_.get();
}
