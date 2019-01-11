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

		std::size_t read(qword address, byte *buffer, std::size_t size, unsigned int *exception = nullptr) const;

		std::size_t write(qword address, const byte *buffer, std::size_t size, unsigned int *exception = nullptr);

		std::size_t set(qword address, byte value, std::size_t size, unsigned int *exception = nullptr);

		std::size_t copy(qword source, qword destination, std::size_t size, unsigned int *exception = nullptr);

		std::size_t copy(const io::reader &source, qword destination, std::size_t size, unsigned int *exception = nullptr);

		std::size_t copy(qword source, io::writer &destination, std::size_t size, unsigned int *exception = nullptr);

		memory_block *allocate_block(std::size_t size, unsigned int attributes = 0u);

		memory_block *allocate_partial_block(qword address, qword offset, std::size_t size);

		memory_block *allocate_partial_block(full_memory_block &block, qword offset, std::size_t size);

		memory_block *reallocate_block(qword address, std::size_t size);

		memory_block *reallocate_block(full_memory_block &block, std::size_t size);

		bool deallocate_block(qword address);

		full_memory_block *find_block(qword address) const;

		full_memory_block *find_block_containing_address(qword address) const;

		void set_range_attributes(qword start, qword end, unsigned int attributes);

		unsigned int get_range_attributes(qword start, qword end) const;

		template <typename target_type>
		target_type read_scalar(qword address, unsigned int *exception = nullptr) const{
			auto buffer = target_type();
			return ((read(address, reinterpret_cast<byte *>(&buffer), sizeof(target_type), exception) == sizeof(target_type)) ? buffer : target_type());
		}

		template <typename target_type>
		std::size_t read_buffer(qword address, target_type *buffer, std::size_t size, unsigned int *exception = nullptr) const{
			return read(address, reinterpret_cast<byte *>(buffer), (sizeof(target_type) * size), exception);
		}

		template <typename target_type>
		bool write_scalar(qword address, target_type buffer, unsigned int *exception = nullptr){
			return (write(address, reinterpret_cast<const byte *>(&buffer), sizeof(target_type), exception) == sizeof(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(qword address, const target_type *buffer, std::size_t size, unsigned int *exception = nullptr){
			return write(address, reinterpret_cast<const byte *>(buffer), (sizeof(target_type) * size), exception);
		}

		static bool address_is_inside_range(qword address, qword start, qword end);
		
	protected:
		std::size_t read_(qword address, byte *buffer, std::size_t size, unsigned int *exception) const;

		std::size_t write_(qword address, const byte *buffer, std::size_t size, bool is_array_buffer, unsigned int *exception);

		std::size_t copy_(qword source, qword destination, std::size_t size, unsigned int *exception);

		std::size_t copy_(const io::reader &source, qword destination, std::size_t size, unsigned int *exception);

		std::size_t copy_(qword source, io::writer &destination, std::size_t size, unsigned int *exception);

		memory_block *allocate_block_(std::size_t size, unsigned int attributes);

		memory_block *allocate_partial_block_(qword address, qword offset, std::size_t size);

		memory_block *allocate_partial_block_(full_memory_block &block, qword offset, std::size_t size);

		memory_block *reallocate_block_(qword address, std::size_t size);

		memory_block *reallocate_block_(full_memory_block &block, std::size_t size);

		bool deallocate_block_(qword address);

		bool deallocate_block_(full_memory_block &block);

		bool add_free_space_(qword address);

		qword extract_free_space_(std::size_t size);

		full_memory_block *find_block_(qword address) const;

		full_memory_block *find_block_containing_address_(qword address) const;

		void set_range_attributes_(qword start, qword end, unsigned int attributes);

		unsigned int get_range_attributes_(qword start, qword end) const;

		std::unordered_map<qword, std::shared_ptr<full_memory_block>> blocks_;
		std::map<qword, size_info> available_sizes_;
		std::list<range_info> range_attributes_;

		qword next_address_ = 0u;
		mutable std::shared_mutex lock_;
	};
}
