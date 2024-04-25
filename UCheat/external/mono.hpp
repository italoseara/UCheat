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

inline string readWidechar(const uintptr_t address, const size_t size)
{
	const auto buffer = make_unique<char[]>(size);
	ReadProcessMemory(Memory::hProc, (LPVOID)address, buffer.get(), size, NULL);
	return string(buffer.get());
}

struct GList
{
	OFFSET(data, uintptr_t, 0x0);
	OFFSET(next, uintptr_t, 0x8);
};

struct MonoRootDomain
{
	OFFSET(domainAssemblies, GList*, 0xA0);
	OFFSET(domainId, int, 0x94);
	OFFSET(jittedFunctionTable, uintptr_t, 0x120);
};

struct MonoTableInfo
{
	int getRows()
	{
		return Memory::read<int>(THISPTR + 0x8) & 0xFFFFFF;
	}
};

struct MonoMethod
{
	string name()
	{
		auto name = readWidechar(Memory::read<uintptr_t>(THISPTR + 0x18), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, 32, "\\u%04X", utf8_to_utf16(const_cast<char *>(name.c_str())));
			name = nameBuffer;
		}

		return name;
	}
};

struct MonoClassField
{
	OFFSET(offset, int, 0x18);

	string name()
	{
		auto name = readWidechar(Memory::read<uintptr_t>(THISPTR + 0x8), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char name_buff[32];
			sprintf_s(name_buff, 32, "\\u%04X", utf8_to_utf16(const_cast<char*>(name.c_str())));
			name = name_buff;
		}

		return name;
	}
};

struct MonoClassRuntimeInfo
{
	OFFSET(maxDomain, int, 0x0);
};

struct MonoVTable
{
	uintptr_t getStaticFieldData()
	{
		if (((Memory::read<uintptr_t>(THISPTR + 48)) & 4) != 0)
			return Memory::read<uintptr_t>(THISPTR + 0x48 + 8 * Memory::read<int>(Memory::read<uintptr_t>(THISPTR + 0x0) + 0x5c));

		return 0;
	}
};

struct MonoClass
{
	OFFSET(numFields, int, 0x100);
	OFFSET(runtimeInfo, MonoClassRuntimeInfo*, 0xD0);

	string getName()
	{
		auto name = readWidechar(Memory::read<uintptr_t>(THISPTR + 0x48), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, 32, "\\u%04X", utf8_to_utf16(const_cast<char*>(name.c_str())));
			name = nameBuffer;
		}

		return name;
	}

	string getNamespace()
	{
		auto name = readWidechar(Memory::read<uintptr_t>(THISPTR + 0x50), 128);
		if (static_cast<uint8_t>(name[0]) == 0xEE)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, 32, ("\\u%04X"), utf8_to_utf16(const_cast<char*>(name.c_str())));
			name = nameBuffer;
		}

		return name;
	}

	int getNumMethods()
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

	MonoMethod* getMethod(const int i)
	{
		return reinterpret_cast<MonoMethod*>(Memory::read<uintptr_t>(Memory::read<uintptr_t>(THISPTR + 0xA0) + 0x8 * i));
	}

	MonoClassField* getField(const int i)
	{
		return reinterpret_cast<MonoClassField*>(Memory::read<uintptr_t>(THISPTR + 0x98) + 0x20 * i);
	}

	MonoVTable* getVTable(MonoRootDomain* domain)
	{
		const auto runtimeInfo = this->runtimeInfo();
		if (!runtimeInfo)
			return nullptr;

		const auto domainId = domain->domainId();
		if (runtimeInfo->maxDomain() < domainId)
			return nullptr;

		return reinterpret_cast<MonoVTable*>(Memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(runtimeInfo) + 8 * domainId + 8));
	}

	MonoMethod* findMethod(const char* methodName)
	{
		auto monoPtr = uintptr_t();
		for (auto i = 0; i < this->getNumMethods(); i++)
		{
			const auto method = this->getMethod(i);
			if (!method)
				continue;

			if (!strcmp(method->name().c_str(), methodName))
				monoPtr = reinterpret_cast<uintptr_t>(method);
		}

		return reinterpret_cast<MonoMethod*>(functions[monoPtr]);
	}

	MonoClassField* findField(const char* fieldName)
	{
		for (auto i = 0; i < this->numFields(); i++)
		{
			const auto field = this->getField(i);
			if (!field)
				continue;

			if (!strcmp(field->name().c_str(), fieldName))
				return field;
		}

		return nullptr;
	}
};

struct MonoHashTable
{
	OFFSET(size, uint32_t, 0x18);
	OFFSET(data, uintptr_t, 0x20);
	OFFSET(nextValue, void*, 0x108);
	OFFSET(keyExtract, unsigned int, 0x58);

	template <typename T>
	T* lookup(void* key)
	{
		auto v4 = static_cast<MonoHashTable*>(Memory::read<void*>(data() + 0x8 * (reinterpret_cast<unsigned int>(key) % this->size())));
		if (!v4)
			return nullptr;

		while (reinterpret_cast<void*>(v4->keyExtract()) != key)
		{
			v4 = static_cast<MonoHashTable*>(v4->nextValue());
			if (!v4)
				return nullptr;
		}

		return reinterpret_cast<T*>(v4);
	}
};

struct MonoImage
{
	OFFSET(flags, int, 0x1C);

	MonoTableInfo* getTableInfo(const int tableId)
	{
		if (tableId > 55)
			return nullptr;
		return reinterpret_cast<MonoTableInfo*>(THISPTR + 0x10 * (static_cast<int>(tableId) + 0xE));
	}

	MonoClass* get(const int typeId)
	{
		if ((this->flags() & 0x20) != 0)
			return nullptr;

		if ((typeId & 0xFF000000) != 0x2000000)
			return nullptr;

		return reinterpret_cast<MonoHashTable*>(this + 0x4D0)->lookup<MonoClass>(reinterpret_cast<void*>(typeId));
	}
};

struct mono_assembly_t
{
	OFFSET(monoImage, MonoImage*, 0x60);
};

namespace Mono
{
	inline MonoRootDomain* getRootDomain()
	{
		return reinterpret_cast<MonoRootDomain*>(Memory::read<uintptr_t>(Memory::baseAddress + GET_ROOT_DOMAIN_OFFSET));
	}

	// credits: niceone1 (https://www.unknowncheats.me/forum/3434741-post11.html)
	inline void initFunctions()
	{
		const auto jittedTable = getRootDomain()->jittedFunctionTable();
		for (auto i = 0; i < Memory::read<int>(jittedTable + 0x8); i++)
		{
			const auto entry = Memory::read<uintptr_t>(jittedTable + 0x10 + i * 0x8);
			if (!entry)
				continue;

			for (auto j = 0; j < Memory::read<int>(entry + 0x4); j++)
			{
				const auto function = Memory::read<uintptr_t>(entry + 0x18 + j * 0x8);
				if (!function)
					continue;

				const auto monoPtr = Memory::read<uintptr_t>(function + 0x0);
				const auto jittedPtr = Memory::read<uintptr_t>(function + 0x10);
				functions[monoPtr] = jittedPtr;
			}
		}
	}

	inline mono_assembly_t* domainAssemblyOpen(MonoRootDomain* domain, const char* name)
	{
		auto domainAssemblies = domain->domainAssemblies();
		if (!domainAssemblies)
			return nullptr;

		auto data = uintptr_t();
		while (true)
		{
			data = domainAssemblies->data();
			if (!data)
				continue;

			const auto dataName = readWidechar(Memory::read<uintptr_t>(data + 0x10), 128);
			if (!strcmp(dataName.c_str(), name))
				break;

			domainAssemblies = reinterpret_cast<GList*>(domainAssemblies->next());
			if (!domainAssemblies)
				break;
		}

		return reinterpret_cast<mono_assembly_t*>(data);
	}

	inline MonoClass* findClass(const char* assemblyName, const char* className)
	{
		const auto rootDomain = getRootDomain();
		if (!rootDomain)
			return nullptr;

		const auto domain_assembly = domainAssemblyOpen(rootDomain, assemblyName);
		if (!domain_assembly)
			return nullptr;

		const auto monoImage = domain_assembly->monoImage();
		if (!monoImage)
			return nullptr;

		const auto tableInfo = monoImage->getTableInfo(3); // 3 now?
		if (!tableInfo)
			return nullptr;

		auto tbl = static_cast<MonoHashTable*>(reinterpret_cast<void*>(monoImage + 0x4D0));
		for (int i = 0; i < tableInfo->getRows(); i++)
		{
			const auto ptr = tbl->lookup<MonoClass>(reinterpret_cast<void*>(0x02000000 | i + 1));
			if (!ptr)
				continue;

			auto name = ptr->getName();
			if (!ptr->getNamespace().empty())
				name = ptr->getNamespace().append(".").append(ptr->getName());

			if (!strcmp(name.c_str(), className))
				return ptr;
		}

		return nullptr;
	}
}

#endif // MONO_HPP
