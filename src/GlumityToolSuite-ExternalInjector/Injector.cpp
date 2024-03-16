#include "pch.h"
#include "Injector.h"

void CppCLRWinFormsProject::Injector::PopulateListWithProcesses(System::Windows::Forms::ListBox^ list)
{
	list->Items->Clear();

	auto prs =
		System::Diagnostics::Process::GetProcesses();
	for (size_t i = 0; i < prs->Length; i++)
	{
		list->Items->Add(prs[i]->ProcessName + " | Title: " + prs[i]->MainWindowTitle->ToString() + " | " + prs[i]->Id);
	}
}

void CppCLRWinFormsProject::Injector::Inject(System::String^ processTarget)
{
	System::Windows::Forms::MessageBox^ msgB; //Error/success messagebox

	char buff[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buff);
	strcat(buff, "\\version.dll");
	const char* dllPath = buff;

	HANDLE hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		System::Diagnostics::Process::GetProcessesByName(processTarget)[0]->Id);
	if (hProcess)
	{
		PVOID allocatedPath = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);

		if (allocatedPath != nullptr)
		{
			WriteProcessMemory(hProcess, allocatedPath, dllPath, strlen(dllPath) + 1, nullptr);

			HANDLE processthread = CreateRemoteThread(hProcess, nullptr, 0,
				(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"kernel32"), "LoadLibraryA"),
				allocatedPath, 0, nullptr);

			if (processthread != nullptr)
			{
				WaitForSingleObject(processthread, 200);

				char buff[MAX_PATH];
				ReadProcessMemory(hProcess, allocatedPath, buff, strlen(dllPath) + 1, nullptr);
				CloseHandle(processthread);
			}

			VirtualFreeEx(hProcess, allocatedPath, 0, MEM_RELEASE);

			msgB->Show(
				nullptr,
				"Injected!",
				"Info",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Information);
		}
		else
		{
		}

		CloseHandle(hProcess);
	}
	else
	{
		msgB->Show(
			nullptr,
			"Could not inject process!\nMake sure that it's a non system critical process!",
			"Error",
			System::Windows::Forms::MessageBoxButtons::OK,
			System::Windows::Forms::MessageBoxIcon::Error);
	}
}