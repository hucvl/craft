@ECHO OFF
ECHO [INFO] Checking Python version...
python --version | FIND /i "Python 3"

IF ERRORLEVEL 1 (
   ECHO We cannot find the Python 3 in your system.
   ECHO  - If it is not installed, go to https://www.python.org/downloads/
   ECHO    and install the latest version of Python 3.
   ECHO  - If it is installed, make sure it is version 3, and added to PATH.
   GOTO end
)

ECHO [INFO] Installing requirements...
pip install -r requirements.txt

ECHO [INFO] Running SVQA Dataset Viewer...
python dataset_viewer.py

:end
ECHO Exiting...
PAUSE