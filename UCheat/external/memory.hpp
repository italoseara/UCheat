// Handle memory reading and writing
// TODO: Change the methods to use my own driver

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>

#include "log.hpp"

using namespace std;

bool equals(const char* a, WCHAR* b) {
	while (*a && *b) {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
	}

	return *a == *b;
}

namespace Memory
{
	// Base address of the module
	DWORD64 base_address = NULL;
	
	// Handles for the process and module
	HANDLE snap_proc_handle = NULL;
	PROCESSENTRY32 snap_proc_entry;

	HANDLE snap_module_handle = NULL;
	MODULEENTRY32 snap_module_entry;

	// Process ID and handle
	DWORD proc_id = NULL;
	HANDLE proc_handle = NULL;

	inline bool attatch(const string& proc_name);
	inline DWORD64 get_module(const string& module_name);

	/*
	 * @brief Attatch to the process
	 */
	inline void init()
	{
		if (!attatch("Unturned.exe")) 
		{
			throw "Failed to attatch to the process";
		}
		
		base_address = get_module("mono-2.0-bdwgc.dll");
	}

	/*
	 * @brief Write a value to the memory
	 * @param value The value to write
	 * @param address The address to write to
	 */
	template <class T>
	inline void write(T value, DWORD64 address)
	{
		WriteProcessMemory(proc_handle, (LPVOID)address, &value, sizeof(T), NULL);
	}

	/*
	 * @brief Read a value from the memory
	 * @param address The address to read from
	 * @return The value at the address
	 */
	template <class T>
	inline T read(DWORD64 address)
	{
		T value;
		ReadProcessMemory(proc_handle, (LPVOID)address, &value, sizeof(T), 0);
		return value;
	}

	/*
	 * @brief Read a buffer from the memory
	 * @param address The address to read from
	 * @param buffer The buffer to read to
	 * @param size The size of the buffer
	 */
	inline void read_buffer(DWORD64 address, void* buffer, size_t size)
	{
		ReadProcessMemory(proc_handle, (LPVOID)address, buffer, size, NULL);
	}

	/*
	 * @brief Attatch to the process
	 * @param processName The name of the process to attatch to
	 * @return True if the process was attatched correctly
	 */
	bool attatch(const string& proc_name)
	{
		snap_proc_entry.dwSize = sizeof(PROCESSENTRY32);
		snap_proc_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (snap_proc_handle == INVALID_HANDLE_VALUE)
		{
			log("Failed to create process snapshot");
			return false;
		}

		while (Process32Next(snap_proc_handle, &snap_proc_entry))
		{
			if (equals(proc_name.c_str(), snap_proc_entry.szExeFile))
			{
				log("Found process %ls (%u)", snap_proc_entry.szExeFile, snap_proc_entry.th32ProcessID);

				proc_id = snap_proc_entry.th32ProcessID;
				proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);

				if (proc_handle == NULL)
				{
					log("Failed to open process handle");
					return false;
				}

				CloseHandle(snap_proc_handle);
				return true;
			}
		}

		log("Failed to find process %s", proc_name.c_str());
		CloseHandle(snap_proc_handle);
		return false;
	}
	
	/*
	 * @brief Get the base address of a module
	 * @param moduleName The name of the module
	 * @return The base address of the module
	 */
	DWORD64 get_module(const string& module_name)
	{
		snap_module_handle = INVALID_HANDLE_VALUE;
		snap_module_handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, proc_id);

		if (snap_module_handle == INVALID_HANDLE_VALUE)
		{
			log("Failed to get modules (Try disabling BattleEye)");
			CloseHandle(proc_handle);
			return 0;
		}

		snap_module_entry.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(snap_module_handle, &snap_module_entry))
		{
			if (equals(module_name.c_str(), snap_module_entry.szModule))
			{
				log("%ls -> 0x%p", snap_module_entry.szModule, snap_module_entry.modBaseAddr);
				CloseHandle(snap_module_handle);
				return (DWORD64)snap_module_entry.modBaseAddr;
			}
		}

		while (Module32Next(snap_module_handle, &snap_module_entry))
		{
			if (equals(module_name.c_str(), snap_module_entry.szModule))
			{
				log("%ls -> 0x%p", snap_module_entry.szModule, snap_module_entry.modBaseAddr);
				CloseHandle(snap_module_handle);
				return (DWORD64)snap_module_entry.modBaseAddr;
			}
		}

		log("Failed to find module %s", module_name.c_str());
		CloseHandle(snap_module_handle);
		return 0;
	}

	/*
	 * @brief Get the process ID of a process
	 * @param processName The name of the process
	 * @return The process ID
	 */
	uint32_t get_proc_id(const string& proc_name)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshot, &entry))
		{
			while (Process32Next(snapshot, &entry))
			{
				if (equals(proc_name.c_str(), entry.szExeFile))
				{
					CloseHandle(snapshot);
					return entry.th32ProcessID;
				}
			}
		}

		CloseHandle(snapshot);
		return 0;
	}
};

#endif // MEMORY_HPP