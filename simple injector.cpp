#include "headers/include.h"

void error(const char* error_title, const char* error_message) {
	MessageBoxA(0, error_message, error_title, 0);
	exit(0);
}

HANDLE get_h_process(DWORD proc_id) {
	HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, 0, proc_id);
	return h_process;
}

int get_proc_id(const char* window_title, DWORD& processID) {
	GetWindowThreadProcessId(FindWindowA(NULL, window_title), &processID);
	return processID;
}

HANDLE get_h_thread(HANDLE h_process, void* allocated_memory) {
	HANDLE h_Thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
	return h_Thread;
}

bool file_exists(std::string file_name) {
	struct stat buffer;
	return (stat(file_name.c_str(), &buffer) == 0);
}

void input_file(char** name_dll) {
	std::cout << "input name, dll file" << std::endl;
	std::cin >> *name_dll;

	if (!file_exists(*name_dll)) {
		error("File Exists", "File doesn't exists");
	}

}

void input_explore(char** name_explore) {
	std::cout << "input name, explore" << std::endl;
	std::cin >> *name_explore;
}

int main() {
	DWORD proc_id = NULL;
	char dll_path[MAX_PATH];
	char* dll_name;
	char* window_title;

	input_file(&dll_name);
	input_explore(&window_title);

	if (GetFullPathName(dll_name, MAX_PATH, dll_path, nullptr) == NULL) {
		error("GetFullPathName", "Failed to get full path");
	}

	if (get_proc_id(window_title, proc_id) == NULL) {
		error("get_proc_id", "Failed to get process ID");
	}

	HANDLE h_process = get_h_process(proc_id);
	if (h_process == NULL) {
		error("h_process", "Failed to get process handle");
	}

	void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!allocated_memory) {
		error("allocated_memory", "Filed Allocated_memory");
	}

	if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr)) {
		error("WriteMemory", "Filed Write Memory");
	}

	HANDLE h_Thread = get_h_thread(h_process, allocated_memory);
	if (!h_Thread) {
		error("CreateThread", "Filed Create Thread");
	}

	CloseHandle(h_process);

	VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);

	MessageBox(0, "Succesfully inject", "success", 0);
}


