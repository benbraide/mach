#pragma once

#include <memory>

#include "../io/io_reader.h"
#include "../io/io_writer.h"

#include "machine_op_code.h"

namespace mach::machine{
	template <class buffer_type>
	struct register_size_type{
		static const op_operand_size value = op_operand_size::nil;
	};

	template <>
	struct register_size_type<unsigned __int8>{
		static const op_operand_size value = op_operand_size::byte;
	};

	template <>
	struct register_size_type<unsigned __int16>{
		static const op_operand_size value = op_operand_size::word;
	};

	template <>
	struct register_size_type<unsigned __int32>{
		static const op_operand_size value = op_operand_size::dword;
	};

	template <>
	struct register_size_type<unsigned __int64>{
		static const op_operand_size value = op_operand_size::qword;
	};

	template <>
	struct register_size_type<float>{
		static const op_operand_size value = op_operand_size::dfloat;
	};

	template <>
	struct register_size_type<long double>{
		static const op_operand_size value = op_operand_size::qfloat;
	};

	class register_object : public io::reader, public io::writer{
	public:
		using byte = unsigned __int8;

		explicit register_object(byte index)
			: index_(index){}

		register_object(const register_object &) = delete;

		register_object &operator =(const register_object &) = delete;

		virtual std::size_t get_offset() const override{
			return 0u;
		}

		virtual std::size_t get_bytes_remaining() const override{
			return get_size();
		}

		virtual std::size_t ignore(std::size_t size) const override{
			return 0u;
		}

		virtual std::size_t get_size() const = 0;

		virtual op_operand_size get_size_type() const = 0;

		virtual byte get_index() const{
			return index_;
		}

		virtual byte *get_data() const = 0;

		template <typename target_type>
		target_type read_scalar() const{
			auto buffer = target_type();
			return ((read(reinterpret_cast<byte *>(&buffer), sizeof(target_type)) == sizeof(target_type)) ? buffer : target_type());
		}

		template <typename target_type>
		std::size_t read_buffer(target_type *buffer, std::size_t size) const{
			return read(reinterpret_cast<byte *>(buffer), (sizeof(target_type) * size));
		}

		virtual std::size_t advance(std::size_t size) override{
			return 0u;
		}

		virtual std::size_t set(byte data, std::size_t size) override{
			if (get_size() < size)//Restrict size
				size = get_size();

			memset(get_data(), data, size);
			return size;
		}

		template <typename target_type>
		bool write_scalar(target_type buffer){
			return (write(reinterpret_cast<const byte *>(&buffer), sizeof(target_type)) == sizeof(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(const target_type *buffer, std::size_t size){
			return write(reinterpret_cast<const byte *>(buffer), (sizeof(target_type) * size));
		}

	protected:
		byte index_;
	};

	template <class buffer_type, class qword_type>
	class generic_register : public register_object{
	public:
		using m_buffer_type = buffer_type;
		using m_qword_type = qword_type;

		explicit generic_register(byte index)
			: register_object(index){}

		virtual std::size_t get_size() const override{
			return sizeof(m_buffer_type);
		}

		virtual op_operand_size get_size_type() const override{
			return register_size_type<m_buffer_type>::template value;
		}

		virtual m_qword_type read_qword() const{
			return static_cast<m_qword_type>(read_scalar<m_buffer_type>());
		}

		virtual void write_qword(m_qword_type buffer){
			write_scalar(static_cast<m_buffer_type>(buffer));
		}

		template <typename target_type>
		target_type read_converted_scalar() const{
			return static_cast<target_type>(read_qword());
		}

		template <typename target_type>
		void write_converted_scalar(target_type buffer){
			write_qword(static_cast<m_qword_type>(buffer));
		}
	};

	template <class buffer_type, class qword_type>
	class child_register : public generic_register<buffer_type, qword_type>{
	public:
		using m_buffer_type = buffer_type;
		using m_qword_type = qword_type;

		explicit child_register(register_object &parent)
			: generic_register<buffer_type, qword_type>(parent.get_index()), parent_(parent){}

		virtual register_object::byte *get_data() const override{
			return parent_.get_data();
		}

		virtual std::size_t read(register_object::byte *buffer, std::size_t size) const override{
			return parent_.read(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
		}

		virtual std::size_t write(const register_object::byte *buffer, std::size_t size) override{
			return parent_.write(buffer, ((sizeof(m_buffer_type) < size) ? sizeof(m_buffer_type) : size));
		}

	protected:
		register_object &parent_;
	};

	using byte_integer_register = child_register<unsigned __int8, unsigned __int64>;
	using word_integer_register = child_register<unsigned __int16, unsigned __int64>;
	using dword_integer_register = child_register<unsigned __int32, unsigned __int64>;
	using dword_float_register = child_register<float, long double>;

	template <class buffer_type, class qword_type>
	class qword_register : public generic_register<buffer_type, qword_type>{
	public:
		using m_buffer_type = buffer_type;
		using m_qword_type = qword_type;

		explicit qword_register(register_object::byte index)
			: generic_register<buffer_type, qword_type>(index){}

		virtual register_object::byte *get_data() const override{
			return const_cast<register_object::byte *>(reinterpret_cast<const register_object::byte *>(&buffer_));
		}

		virtual std::size_t read(register_object::byte *buffer, std::size_t size) const override{
			if (sizeof(m_buffer_type) < size)//Restrict size
				size = sizeof(m_buffer_type);

			memcpy(buffer, &buffer_, size);
			return size;
		}

		virtual std::size_t write(const register_object::byte *buffer, std::size_t size) override{
			if (sizeof(m_buffer_type) < size)//Restrict size
				size = sizeof(m_buffer_type);

			memcpy(&buffer_, buffer, size);
			return size;
		}

	protected:
		m_buffer_type buffer_ = 0u;
	};

	using qword_integer_register = qword_register<unsigned __int64, unsigned __int64>;
	using qword_float_register = child_register<long double, long double>;
}
