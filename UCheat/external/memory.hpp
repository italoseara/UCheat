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
	DWORD64 baseAddress = NULL;
	
	// Handles for the process and module
	HANDLE hSnapProc = NULL;
	PROCESSENTRY32 snapProcEntry;

	HANDLE hSnapModule = NULL;
	MODULEENTRY32 snapModuleEntry;

	// Process ID and handle
	DWORD procId = NULL;
	HANDLE hProc = NULL;

	inline bool attatch(const string& proc_name);
	inline DWORD64 getModule(const string& module_name);

	/*
	 * @brief Attatch to the process
	 */
	inline void init()
	{
		if (!attatch("Unturned.exe")) 
		{
			throw "Failed to attatch to the process";
		}
		
		baseAddress = getModule("mono-2.0-bdwgc.dll");
	}

	/*
	 * @brief Write a value to the memory
	 * @param value The value to write
	 * @param address The address to write to
	 */
	template <class T>
	inline void write(T value, DWORD64 address)
	{
		WriteProcessMemory(hProc, (LPVOID)address, &value, sizeof(T), NULL);
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
		ReadProcessMemory(hProc, (LPVOID)address, &value, sizeof(T), 0);
		return value;
	}

	/*
	 * @brief Read a buffer from the memory
	 * @param address The address to read from
	 * @param buffer The buffer to read to
	 * @param size The size of the buffer
	 */
	inline void readBuffer(DWORD64 address, void* buffer, size_t size)
	{
		ReadProcessMemory(hProc, (LPVOID)address, buffer, size, NULL);
	}

	/*
	 * @brief Attatch to the process
	 * @param processName The name of the process to attatch to
	 * @return True if the process was attatched correctly
	 */
	bool attatch(const string& procName)
	{
		snapProcEntry.dwSize = sizeof(PROCESSENTRY32);
		hSnapProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hSnapProc == INVALID_HANDLE_VALUE)
		{
			log("Failed to create process snapshot");
			return false;
		}

		while (Process32Next(hSnapProc, &snapProcEntry))
		{
			if (equals(procName.c_str(), snapProcEntry.szExeFile))
			{
				log("Found process %ls (%u)", snapProcEntry.szExeFile, snapProcEntry.th32ProcessID);

				procId = snapProcEntry.th32ProcessID;
				hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

				if (hProc == NULL)
				{
					log("Failed to open process handle");
					return false;
				}

				CloseHandle(hSnapProc);
				return true;
			}
		}

		log("Failed to find process %s", procName.c_str());
		CloseHandle(hSnapProc);
		return false;
	}
	
	/*
	 * @brief Get the base address of a module
	 * @param moduleName The name of the module
	 * @return The base address of the module
	 */
	DWORD64 getModule(const string& moduleName)
	{
		hSnapModule = INVALID_HANDLE_VALUE;
		hSnapModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procId);

		if (hSnapModule == INVALID_HANDLE_VALUE)
		{
			log("Failed to get modules (Try disabling BattleEye)");
			CloseHandle(hProc);
			return 0;
		}

		snapModuleEntry.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapModule, &snapModuleEntry))
		{
			if (equals(moduleName.c_str(), snapModuleEntry.szModule))
			{
				log("%ls -> 0x%p", snapModuleEntry.szModule, snapModuleEntry.modBaseAddr);
				CloseHandle(hSnapModule);
				return (DWORD64)snapModuleEntry.modBaseAddr;
			}
		}

		while (Module32Next(hSnapModule, &snapModuleEntry))
		{
			if (equals(moduleName.c_str(), snapModuleEntry.szModule))
			{
				log("%ls -> 0x%p", snapModuleEntry.szModule, snapModuleEntry.modBaseAddr);
				CloseHandle(hSnapModule);
				return (DWORD64)snapModuleEntry.modBaseAddr;
			}
		}

		log("Failed to find module %s", moduleName.c_str());
		CloseHandle(hSnapModule);
		return 0;
	}

	/*
	 * @brief Get the process ID of a process
	 * @param procName The name of the process
	 * @return The process ID
	 */
	uint32_t getProcId(const string& procName)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshot, &entry))
		{
			while (Process32Next(snapshot, &entry))
			{
				if (equals(procName.c_str(), entry.szExeFile))
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