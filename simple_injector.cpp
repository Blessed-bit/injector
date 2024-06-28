#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>;
#include <string>

void error(const char* error_title, const char* error_message) {
	MessageBoxA(0, error_message, error_title, 0);
	exit(0);
}

HANDLE get_h_process(DWORD proc_id) {
	HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, 0, proc_id);
	if (h_process == NULL) {
		error("h_process", "Failed to get process handle");
	}
	return h_process;
}

void get_proc_id(const char* window_title, DWORD& processID) {
	GetWindowThreadProcessId(FindWindowA(NULL, window_title), &processID);
	if (processID == NULL) {
		error("get_proc_id", "Failed to get process ID");
	}
}

HANDLE get_h_thread(HANDLE h_process, void* allocated_memory) {
	HANDLE h_Thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
	if (!h_Thread) {
		error("CreateThread", "Filed Create Thread");
	}
	return h_Thread;
}

bool file_exists(std::string file_name) {
	struct stat buffer;
	return (stat(file_name.c_str(), &buffer) == 0);
}


int main() {
	DWORD proc_id = NULL;
	char dll_path[MAX_PATH];
	const char* dll_name = "test.dll";
	const char* window_title = "Безымянный - Paint";



	if (!file_exists(dll_name)){
		error("File Exists", "File doesn't exists");
	}

	if (!GetFullPathName(dll_name, MAX_PATH, dll_path, nullptr)) {
		error("GetFullPathName", "Failed to get full path");
	}

	get_proc_id(window_title, proc_id);
	HANDLE h_process = get_h_process(proc_id);

	void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!allocated_memory) {
		error("allocated_memory", "Filed Allocated_memory");
	}

	printf("0x%p", allocated_memory);

	if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr)) {
		error("WriteMemory", "Filed Write Memory");
	}

	HANDLE h_Thread = get_h_thread(h_process, allocated_memory);

	CloseHandle(h_process);
	VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);

	MessageBox(0, "Succesfully inject", "success", 0);
}


