#pragma once

#include <map>
#include <string>

#include "machine_register.h"

namespace mach::machine{
	class memory;

	enum class memory_error_code{
		nil,
		out_of_address_space,
		allocation_failure,
		access_protected,
		write_protected,
		external_data_extend,
		block_not_found,
	};

	class memory_block{
	public:
		using byte = unsigned __int8;
		using qword = unsigned __int64;

		memory_block(qword address, std::size_t size, unsigned int attributes = 0u);

		virtual ~memory_block();

		qword get_address() const;

		qword get_address_offset(qword address) const;

		bool address_is_inside_block(qword address) const;

		bool address_range_intersects_block(qword first, qword last) const;

		std::size_t get_size() const;

		std::size_t get_real_size() const;

		byte *get_data() const;

		unsigned int get_attributes() const;

		std::size_t read(qword offset, byte *buffer, std::size_t size) const;

		std::size_t write(qword offset, const byte *buffer, std::size_t size);

		std::size_t set(qword offset, byte value, std::size_t size);

		template <typename target_type>
		target_type read_scalar(qword offset) const{
			auto buffer = target_type();
			return ((read(offset, reinterpret_cast<byte *>(&buffer), sizeof(target_type)) == sizeof(target_type)) ? buffer : target_type());
		}

		template <typename target_type>
		std::size_t read_buffer(qword offset, target_type *buffer, std::size_t size) const{
			return read(offset, reinterpret_cast<byte *>(buffer), (sizeof(target_type) * size));
		}

		template <typename target_type>
		bool write_scalar(qword offset, target_type buffer){
			return (write(offset, reinterpret_cast<const byte *>(&buffer), sizeof(target_type)) == sizeof(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(qword offset, const target_type *buffer, std::size_t size){
			return write(offset, reinterpret_cast<const byte *>(buffer), (sizeof(target_type) * size));
		}

		static const unsigned int access_protected			= (0u << 0x0000u);
		static const unsigned int write_protected			= (1u << 0x0000u);
		static const unsigned int external_data_extend		= (1u << 0x0001u);

	protected:
		friend class memory;

		friend class full_memory_block;
		friend class partial_memory_block;

		memory_block();

		bool offset_is_inside_block_(qword offset) const;

		void set_previous_block_(memory_block *block);

		void set_next_block_(memory_block *block);

		unsigned __int64 address_ = 0u;
		unsigned int attributes_ = 0u;

		std::size_t size_ = 0u;
		std::size_t real_size_ = 0u;
		std::unique_ptr<byte[]> data_;

		memory_block *previous_block_ = nullptr;
		memory_block *next_block_ = nullptr;
	};

	class external_memory_block : public memory_block{
	public:
		external_memory_block(byte *data, qword address, std::size_t size, unsigned int attributes = 0u);

		virtual ~external_memory_block();

		byte *get_data() const;

	protected:
		byte *external_data_ = nullptr;
	};
}
