// https://github.com/reahly/mono-external-lib
// Updated by Coopyy (https://github.com/Coopyy/)
// Rewritten by Ítalo Seara (https://github.com/italoseara)

#ifndef MONO_HPP
#define MONO_HPP

#include <Windows.h>
#include <codecvt>
#include <unordered_map>
#include "memory.hpp"

using namespace std;

#define THISPTR reinterpret_cast<uintptr_t>(this)

#define OFFSET(func, type, offset)                                             \
	type func() {                                                              \
		return Memory::read<type>(reinterpret_cast<uintptr_t>(this) + offset); \
	}

// NOTE: can easily update by finding the address that mono_get_root_domain returns by 
// disassembling the mono dll in ida (i dont see you needing to do this anyitme soon)
constexpr int GET_ROOT_DOMAIN_OFFSET = 0x72F020;

unordered_map<uintptr_t, uintptr_t> functions;

inline unsigned short utf8_to_utf16(const char* val)
{
	wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> convert;
	u16string dest = convert.from_bytes(val);
	return *reinterpret_cast<unsigned short*>(&dest[0]);
}

inline string read_widechar(const uintptr_t address, const size_t size)
{
	const auto buffer = make_unique<char[]>(size);
	ReadProcessMemory(Memory::proc_handle, (LPVOID)address, buffer.get(), size, NULL);
	return string(buffer.get());
}

struct glist_t
{
	OFFSET(data, uintptr_t, 0x0);
	OFFSET(next, uintptr_t, 0x8);
};

struct mono_root_domain_t
{
	OFFSET(domain_assemblies, glist_t*, 0xA0);
	OFFSET(domain_id, int, 0x94);
	OFFSET(jitted_function_table, uintptr_t, 0x120);
};

struct mono_table_info_t
{
	int get_rows()
	{
		return Memory::read<int>(THISPTR + 0x8) & 0xFFFFFF;
	}
};

struct mono_method_t
{
	string name()
	{
		auto name = read_widechar(Memory::read<uintptr_t>(THISPTR + 0x18), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char name_buff[32];
			sprintf_s(name_buff, 32, "\\u%04X", utf8_to_utf16(const_cast<char *>(name.c_str())));
			name = name_buff;
		}

		return name;
	}
};

struct mono_class_field_t
{
	OFFSET(offset, int, 0x18);

	string name()
	{
		auto name = read_widechar(Memory::read<uintptr_t>(THISPTR + 0x8), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char name_buff[32];
			sprintf_s(name_buff, 32, "\\u%04X", utf8_to_utf16(const_cast<char*>(name.c_str())));
			name = name_buff;
		}

		return name;
	}
};

struct mono_class_runtime_info_t
{
	OFFSET(max_domain, int, 0x0);
};

struct mono_vtable_t
{
	uintptr_t get_static_field_data()
	{
		if (((Memory::read<uintptr_t>(THISPTR + 48)) & 4) != 0)
			return Memory::read<uintptr_t>(THISPTR + 0x48 + 8 * Memory::read<int>(Memory::read<uintptr_t>(THISPTR + 0x0) + 0x5c));

		return 0;
	}
};

struct mono_class_t
{
	OFFSET(num_fields, int, 0x100);
	OFFSET(runtime_info, mono_class_runtime_info_t*, 0xD0);

	string name()
	{
		auto name = read_widechar(Memory::read<uintptr_t>(THISPTR + 0x48), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char name_buff[32];
			sprintf_s(name_buff, 32, "\\u%04X", utf8_to_utf16(const_cast<char*>(name.c_str())));
			name = name_buff;
		}

		return name;
	}

	string namespace_name()
	{
		auto name = read_widechar(Memory::read<uintptr_t>(THISPTR + 0x50), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char name_buff[32];
			sprintf_s(name_buff, 32, ("\\u%04X"), utf8_to_utf16(const_cast<char*>(name.c_str())));
			name = name_buff;
		}

		return name;
	}

	int get_num_methods()
	{
		const auto v2 = (Memory::read<int>(THISPTR + 0x2a) & 7) - 1;
		switch (v2)
		{
		case 0:
		case 1:
			return Memory::read<int>(THISPTR + 0xFC);

		case 3:
		case 5:
			return 0;

		case 4u:
			return Memory::read<int>(THISPTR + 0xF0);

		default:
			break;
		}

		return 0;
	}

	mono_method_t* get_method(const int i)
	{
		return reinterpret_cast<mono_method_t*>(Memory::read<uintptr_t>(Memory::read<uintptr_t>(THISPTR + 0xA0) + 0x8 * i));
	}

	mono_class_field_t* get_field(const int i)
	{
		return reinterpret_cast<mono_class_field_t*>(Memory::read<uintptr_t>(THISPTR + 0x98) + 0x20 * i);
	}

	mono_vtable_t* get_vtable(mono_root_domain_t* domain)
	{
		const auto runtime_info = this->runtime_info();
		if (!runtime_info)
			return nullptr;

		const auto domain_id = domain->domain_id();
		if (runtime_info->max_domain() < domain_id)
			return nullptr;

		return reinterpret_cast<mono_vtable_t*>(Memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(runtime_info) + 8 * domain_id + 8));
	}

	mono_method_t* find_method(const char* method_name)
	{
		auto mono_ptr = uintptr_t();
		for (auto i = 0; i < this->get_num_methods(); i++)
		{
			const auto method = this->get_method(i);
			if (!method)
				continue;

			if (!strcmp(method->name().c_str(), method_name))
				mono_ptr = reinterpret_cast<uintptr_t>(method);
		}

		return reinterpret_cast<mono_method_t*>(functions[mono_ptr]);
	}

	mono_class_field_t* find_field(const char* field_name)
	{
		for (auto i = 0; i < this->num_fields(); i++)
		{
			const auto field = this->get_field(i);
			if (!field)
				continue;

			if (!strcmp(field->name().c_str(), field_name))
				return field;
		}

		return nullptr;
	}
};

struct mono_hash_table_t
{
	OFFSET(size, uint32_t, 0x18);
	OFFSET(data, uintptr_t, 0x20);
	OFFSET(next_value, void*, 0x108);
	OFFSET(key_extract, unsigned int, 0x58);

	template <typename T>
	T* lookup(void* key)
	{
		auto v4 = static_cast<mono_hash_table_t*>(Memory::read<void*>(data() + 0x8 * (reinterpret_cast<unsigned int>(key) % this->size())));
		if (!v4)
			return nullptr;

		while (reinterpret_cast<void*>(v4->key_extract()) != key)
		{
			v4 = static_cast<mono_hash_table_t*>(v4->next_value());
			if (!v4)
				return nullptr;
		}

		return reinterpret_cast<T*>(v4);
	}
};

struct mono_image_t
{
	OFFSET(flags, int, 0x1C);

	mono_table_info_t* get_table_info(const int table_id)
	{
		if (table_id > 55)
			return nullptr;
		return reinterpret_cast<mono_table_info_t*>(THISPTR + 0x10 * (static_cast<int>(table_id) + 0xE));
	}

	mono_class_t* get(const int type_id)
	{
		if ((this->flags() & 0x20) != 0)
			return nullptr;

		if ((type_id & 0xFF000000) != 0x2000000)
			return nullptr;

		return reinterpret_cast<mono_hash_table_t*>(this + 0x4D0)->lookup<mono_class_t>(reinterpret_cast<void*>(type_id));
	}
};

struct mono_assembly_t
{
	OFFSET(mono_image, mono_image_t*, 0x60);
};

namespace Mono
{
	inline mono_root_domain_t* get_root_domain()
	{
		return reinterpret_cast<mono_root_domain_t*>(Memory::read<uintptr_t>(Memory::base_address + GET_ROOT_DOMAIN_OFFSET));
	}

	inline void init_functions()
	{ // credits: niceone1 (https://www.unknowncheats.me/forum/3434741-post11.html)
		const auto jitted_table = get_root_domain()->jitted_function_table();
		for (auto i = 0; i < Memory::read<int>(jitted_table + 0x8); i++)
		{
			const auto entry = Memory::read<uintptr_t>(jitted_table + 0x10 + i * 0x8);
			if (!entry)
				continue;

			for (auto j = 0; j < Memory::read<int>(entry + 0x4); j++)
			{
				const auto function = Memory::read<uintptr_t>(entry + 0x18 + j * 0x8);
				if (!function)
					continue;

				const auto mono_ptr = Memory::read<uintptr_t>(function + 0x0);
				const auto jitted_ptr = Memory::read<uintptr_t>(function + 0x10);
				functions[mono_ptr] = jitted_ptr;
			}
		}
	}

	inline mono_assembly_t* domain_assembly_open(mono_root_domain_t* domain, const char* name)
	{
		auto domain_assemblies = domain->domain_assemblies();
		if (!domain_assemblies)
			return nullptr;

		auto data = uintptr_t();
		while (true)
		{
			data = domain_assemblies->data();
			if (!data)
				continue;

			const auto data_name = read_widechar(Memory::read<uintptr_t>(data + 0x10), 128);
			if (!strcmp(data_name.c_str(), name))
				break;

			domain_assemblies = reinterpret_cast<glist_t*>(domain_assemblies->next());
			if (!domain_assemblies)
				break;
		}

		return reinterpret_cast<mono_assembly_t*>(data);
	}

	inline mono_class_t* find_class(const char* assembly_name, const char* class_name)
	{
		const auto root_domain = get_root_domain();
		if (!root_domain)
			return nullptr;

		const auto domain_assembly = domain_assembly_open(root_domain, assembly_name);
		if (!domain_assembly)
			return nullptr;

		const auto mono_image = domain_assembly->mono_image();
		if (!mono_image)
			return nullptr;

		const auto table_info = mono_image->get_table_info(3); // 3 now?
		if (!table_info)
			return nullptr;

		auto tbl = static_cast<mono_hash_table_t*>(reinterpret_cast<void*>(mono_image + 0x4D0));
		for (int i = 0; i < table_info->get_rows(); i++)
		{
			const auto ptr = tbl->lookup<mono_class_t>(reinterpret_cast<void*>(0x02000000 | i + 1));
			if (!ptr)
				continue;

			auto name = ptr->name();
			if (!ptr->namespace_name().empty())
				name = ptr->namespace_name().append(".").append(ptr->name());

			if (!strcmp(name.c_str(), class_name))
				return ptr;
		}

		return nullptr;
	}
}

#endif // MONO_HPP
