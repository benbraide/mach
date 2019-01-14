#pragma once

#include <list>
#include <shared_mutex>
#include <unordered_map>

#include "../io/io_reader.h"

#include "machine_memory_block.h"

namespace mach::machine{
	class memory{
	public:
		using byte = memory_block::byte;
		using qword = memory_block::qword;

		struct size_info{
			std::size_t value;
			bool is_free;
		};

		struct range_info{
			qword start;
			qword end;
			unsigned int attributes;
		};

		memory();

		void read(qword address, byte *buffer, std::size_t size) const;

		void write(qword address, const byte *buffer, std::size_t size);

		void set(qword address, byte value, std::size_t size);

		void copy(qword source, qword destination, std::size_t size);

		void copy(const io::reader &source, qword destination, std::size_t size);

		void copy(qword source, io::writer &destination, std::size_t size);

		memory_block *allocate_block(std::size_t size, unsigned int attributes = 0u);

		memory_block *allocate_block(byte *data, std::size_t size, unsigned int attributes = 0u);

		memory_block *reallocate_block(qword address, std::size_t size);

		memory_block *reallocate_block(memory_block &block, std::size_t size);

		void deallocate_block(qword address);

		memory_block *find_block(qword address) const;

		memory_block *find_block_containing_address(qword address) const;

		void set_range_attributes(qword start, qword end, unsigned int attributes);

		unsigned int get_range_attributes(qword start, qword end) const;

		template <typename target_type>
		target_type read_scalar(qword address) const{
			auto buffer = target_type();
			read_buffer(address, &buffer, 1u);
			return buffer;
		}

		template <typename target_type>
		void read_buffer(qword address, target_type *buffer, std::size_t size) const{
			read(address, (byte *)buffer, (sizeof(target_type) * size));
		}

		template <typename target_type>
		void write_scalar(qword address, target_type buffer){
			write_buffer(address, &buffer, 1u);
		}

		template <typename target_type>
		void write_buffer(qword address, const target_type *buffer, std::size_t size){
			write(address, (byte *)buffer, (sizeof(target_type) * size));
		}

		static bool address_is_inside_range(qword address, qword start, qword end);
		
	protected:
		void read_(qword address, byte *buffer, std::size_t size) const;

		void write_(qword address, const byte *buffer, std::size_t size, bool is_array_buffer);

		void copy_(qword source, qword destination, std::size_t size);

		void copy_(const io::reader &source, qword destination, std::size_t size);

		void copy_(qword source, io::writer &destination, std::size_t size);

		memory_block *allocate_block_(byte *data, std::size_t size, unsigned int attributes);

		memory_block *reallocate_block_(qword address, std::size_t size);

		memory_block *reallocate_block_(memory_block &block, std::size_t size);

		void deallocate_block_(qword address);

		void deallocate_block_(memory_block &block);

		void add_free_space_(qword address);

		qword extract_free_space_(std::size_t size);

		memory_block *find_block_(qword address) const;

		memory_block *find_block_containing_address_(qword address) const;

		void set_range_attributes_(qword start, qword end, unsigned int attributes);

		unsigned int get_range_attributes_(qword start, qword end) const;

		std::unordered_map<qword, std::shared_ptr<memory_block>> blocks_;
		std::map<qword, size_info> available_sizes_;
		std::list<range_info> range_attributes_;

		qword next_address_ = 0u;
		mutable std::shared_mutex lock_;
	};
}
