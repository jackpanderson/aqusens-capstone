@echo off
setlocal

:: make sure python is installed
echo Checking if Python is installed...
where python >nul 2>nul
if %errorlevel% neq 0 (
    echo Python not found. Downloading Python installer...
    powershell -Command "(New-Object System.Net.WebClient).DownloadFile('https://www.python.org/ftp/python/3.12.2/python-3.12.2-amd64.exe', 'python_installer.exe')"

    echo Installing Python...
    start /wait python_installer.exe /quiet InstallAllUsers=1 PrependPath=1 Include_pip=1
    del python_installer.exe
) else (
    echo Python is already installed.
)

:: make sure pip is installed
echo Updating pip...
python -m ensurepip
python -m pip install --upgrade pip

:: install requests and serial
echo Installing required Python packages...
python -m pip install requests pyserial

:: change this to the command that is used to run the program on the actual Aqusens computer
echo Running AqusensComm.py...
"C:/Users/deeba/AppData/Local/Microsoft/WindowsApps/python3.11.exe" "\\wsl.localhost\Ubuntu\home\deebakhosravi\cpe350\aqusens-capstone\AqusensComm.py"

endlocal
