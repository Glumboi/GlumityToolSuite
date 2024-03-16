#pragma once
//Common hooks are used to hook to things outside of il2cpp, basically anything we know the name and signature of

//Doesnt work yet
template <typename fnReturnType, typename... args>
struct FunctionHook
{
    using originalFunctionType = fnReturnType(_fastcall*)(args...);
    originalFunctionType originalFunction;
    LPVOID hook;


    uintptr_t StringToUintPtr(std::string value)
    {
        return std::stoull(value, nullptr, 16);
    }

    void LoadHook(LPVOID* targetAddress, LPVOID hook)
    {
        this->hook = hook;

        MH_CreateHook(
            targetAddress,
            &hook,
            (LPVOID*)&originalFunction);
        MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS); //MH_EnableHook(*targetAddress);

        QUICKDEBUG("Status of hook: " << status);
    }
};

//Common windows api hooks here, do not use the template from above to create them

using MessageBoxA_t = int(WINAPI*)(HWND, LPCSTR, LPCSTR, UINT);
MessageBoxA_t MessageBoxA_o;

int WINAPI MessageBoxA_hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    QUICKDEBUG("MessageBoxA hook called!");
    return MessageBoxA_o(hWnd, lpText, lpCaption, uType);
}

void CreateCommonHooks()
{
    // Get the address of MessageBoxA
    HMODULE user32Module = GetModuleHandle("User32.dll");
    MessageBoxA_o = reinterpret_cast<MessageBoxA_t>(GetProcAddress(user32Module, "MessageBoxA"));
    // Create the MessageBoxA hook
    MH_CreateHook(MessageBoxA_o, &MessageBoxA_hook, reinterpret_cast<LPVOID*>(&MessageBoxA_o));
}
