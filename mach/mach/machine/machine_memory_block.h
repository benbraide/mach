#pragma once

#include <map>

#include "machine_register.h"

namespace mach::machine{
	class memory;

	class full_memory_block;
	class partial_memory_block;

	class memory_block{
	public:
		using byte = unsigned __int8;
		using qword = unsigned __int64;

		virtual ~memory_block();

		virtual qword get_address() const = 0;

		virtual qword get_address_offset(qword address) const;

		virtual bool address_is_inside_block(qword address) const;

		virtual bool address_range_intersects_block(qword first, qword last) const;

		virtual std::size_t get_size() const = 0;

		virtual std::size_t get_real_size() const = 0;

		virtual byte *get_data() const = 0;

		virtual unsigned int get_attributes() const = 0;

		virtual std::size_t read(qword offset, byte *buffer, std::size_t size, unsigned int *exception = nullptr) const = 0;

		virtual std::size_t write(qword offset, const byte *buffer, std::size_t size, unsigned int *exception = nullptr) = 0;

		virtual std::size_t set(qword offset, byte value, std::size_t size, unsigned int *exception = nullptr) = 0;

		template <typename target_type>
		target_type read_scalar(qword offset, unsigned int *exception = nullptr) const{
			auto buffer = target_type();
			return ((read(offset, reinterpret_cast<byte *>(&buffer), sizeof(target_type)) == sizeof(target_type)) ? buffer : target_type());
		}

		template <typename target_type>
		std::size_t read_buffer(qword offset, target_type *buffer, std::size_t size, unsigned int *exception = nullptr) const{
			return read(offset, reinterpret_cast<byte *>(buffer), (sizeof(target_type) * size));
		}

		template <typename target_type>
		bool write_scalar(qword offset, target_type buffer, unsigned int *exception = nullptr){
			return (write(offset, reinterpret_cast<const byte *>(&buffer), sizeof(target_type)) == sizeof(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(qword offset, const target_type *buffer, std::size_t size, unsigned int *exception = nullptr){
			return write(offset, reinterpret_cast<const byte *>(buffer), (sizeof(target_type) * size));
		}

		static const unsigned int access_protected			= (0u << 0x0000u);
		static const unsigned int write_protected			= (1u << 0x0000u);

	protected:
		friend class memory;

		friend class full_memory_block;
		friend class partial_memory_block;

		virtual bool offset_is_inside_block_(qword offset) const;

		virtual void set_previous_block_(memory_block *block);

		virtual void set_next_block_(memory_block *block);

		memory_block *previous_block_ = nullptr;
		memory_block *next_block_ = nullptr;
	};

	class full_memory_block : public memory_block{
	public:
		full_memory_block(qword address, std::size_t size, unsigned int attributes = 0u);

		virtual ~full_memory_block();

		full_memory_block(const full_memory_block &) = delete;

		full_memory_block &operator =(const full_memory_block &) = delete;

		virtual qword get_address() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_real_size() const override;

		virtual byte *get_data() const override;

		virtual unsigned int get_attributes() const override;

		virtual std::size_t read(qword offset, byte *buffer, std::size_t size, unsigned int *exception = nullptr) const override;

		virtual std::size_t write(qword offset, const byte *buffer, std::size_t size, unsigned int *exception = nullptr) override;

		virtual std::size_t set(qword offset, byte value, std::size_t size, unsigned int *exception = nullptr) override;

	protected:
		friend class memory;

		virtual memory_block *insert_partial_block_(qword offset, std::size_t size);

		virtual memory_block *find_partial_block_(qword offset) const;

		virtual memory_block *find_partial_block_containing_offset_(qword offset) const;

		virtual memory_block *find_next_partial_block_(qword offset) const;

		unsigned __int64 address_ = 0u;
		unsigned int attributes_ = 0u;

		std::size_t size_ = 0u;
		std::size_t real_size_ = 0u;

		std::unique_ptr<byte[]> data_;
		std::map<qword, std::shared_ptr<memory_block>> partial_blocks_;
	};

	class partial_memory_block : public memory_block{
	public:
		partial_memory_block(memory_block &block, qword offset, std::size_t size);

		virtual ~partial_memory_block();

		virtual qword get_address() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_real_size() const override;

		virtual byte *get_data() const override;

		virtual unsigned int get_attributes() const override;

		virtual std::size_t read(qword offset, byte *buffer, std::size_t size, unsigned int *exception = nullptr) const override;

		virtual std::size_t write(qword offset, const byte *buffer, std::size_t size, unsigned int *exception = nullptr) override;

		virtual std::size_t set(qword offset, byte value, std::size_t size, unsigned int *exception = nullptr) override;

	protected:
		friend class memory;
		friend class full_memory_block;

		memory_block &block_;
		qword offset_ = 0u;

		std::size_t size_ = 0u;
		std::size_t real_size_ = 0u;
	};
}
