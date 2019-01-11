#pragma once

#include <memory>

#include "../io/io_reader.h"
#include "../io/io_writer.h"

namespace mach::machine{
	class word_integer_register;
	class dword_integer_register;
	class qword_integer_register;
	class qword_float_register;

	template <class buffer_type>
	struct register_child_buffer_type{
		using type = void;
	};

	template <>
	struct register_child_buffer_type<unsigned __int16>{
		using type = unsigned __int8;
	};

	template <>
	struct register_child_buffer_type<unsigned __int32>{
		using type = unsigned __int16;
	};

	template <>
	struct register_child_buffer_type<unsigned __int64>{
		using type = unsigned __int32;
	};

	template <>
	struct register_child_buffer_type<long double>{
		using type = float;
	};

	class register_object : public io::reader, public io::writer{
	public:
		using byte = unsigned __int8;

		virtual std::size_t get_bytes_remaining() const override;

		virtual std::size_t get_size() const = 0;

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

		template <typename target_type>
		bool write_scalar(target_type buffer){
			return (write(reinterpret_cast<const byte *>(&buffer), sizeof(target_type)) == sizeof(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(const target_type *buffer, std::size_t size){
			return write(reinterpret_cast<const byte *>(buffer), (sizeof(target_type) * size));
		}
	};

	template <class buffer_type, class qword_type>
	class generic_register : public register_object{
	public:
		using m_buffer_type = buffer_type;
		using m_qword_type = qword_type;

		virtual std::size_t get_size() const override{
			return sizeof(m_buffer_type);
		}

		virtual generic_register<typename register_child_buffer_type<buffer_type>::type, qword_type> *get_child() const{
			return nullptr;
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

	class byte_integer_register : public generic_register<unsigned __int8, unsigned __int64>{
	public:
		virtual byte *get_data() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

		virtual std::size_t write(const byte *buffer, std::size_t size) override;

	protected:
		friend class word_integer_register;

		explicit byte_integer_register(word_integer_register &parent);

		word_integer_register &parent_;
	};

	class word_integer_register : public generic_register<unsigned __int16, unsigned __int64>{
	public:
		virtual byte *get_data() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

		virtual std::size_t write(const byte *buffer, std::size_t size) override;

		virtual generic_register<unsigned __int8, unsigned __int64> *get_child() const override;

	protected:
		friend class dword_integer_register;

		explicit word_integer_register(dword_integer_register &parent);

		dword_integer_register &parent_;
		std::shared_ptr<byte_integer_register> child_;
	};

	class dword_integer_register : public generic_register<unsigned __int32, unsigned __int64>{
	public:
		virtual byte *get_data() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

		virtual std::size_t write(const byte *buffer, std::size_t size) override;

		virtual generic_register<unsigned __int16, unsigned __int64> *get_child() const override;

	protected:
		friend class qword_integer_register;

		explicit dword_integer_register(qword_integer_register &parent);

		qword_integer_register &parent_;
		std::shared_ptr<word_integer_register> child_;
	};

	class qword_integer_register : public generic_register<unsigned __int64, unsigned __int64>{
	public:
		qword_integer_register();

		qword_integer_register(const qword_integer_register &) = delete;

		qword_integer_register &operator =(const qword_integer_register &) = delete;

		virtual byte *get_data() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

		virtual std::size_t write(const byte *buffer, std::size_t size) override;

		virtual generic_register<unsigned __int32, unsigned __int64> *get_child() const override;

	protected:
		m_buffer_type buffer_ = 0u;
		std::shared_ptr<dword_integer_register> child_;
	};

	class dword_float_register : public generic_register<float, long double>{
	public:
		virtual byte *get_data() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

		virtual std::size_t write(const byte *buffer, std::size_t size) override;

	protected:
		friend class qword_float_register;

		explicit dword_float_register(qword_float_register &parent);

		qword_float_register &parent_;
	};

	class qword_float_register : public generic_register<long double, long double>{
	public:
		qword_float_register();

		qword_float_register(const qword_float_register &) = delete;

		qword_float_register &operator =(const qword_float_register &) = delete;

		virtual byte *get_data() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

		virtual std::size_t write(const byte *buffer, std::size_t size) override;

		virtual generic_register<float, long double> *get_child() const override;

	protected:
		m_buffer_type buffer_ = 0.0l;
		std::shared_ptr<dword_float_register> child_;
	};
}
