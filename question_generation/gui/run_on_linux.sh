#!/bin/bash

# Set minimum required versions
PYTHON_MINIMUM_MAJOR=3
PYTHON_MINIMUM_MINOR=5

# Get python references
PYTHON3_REF=$(which python3 | grep "/python3")
PYTHON_REF=$(which python | grep "/python")

error_msg() {
    echo We cannot find the Python 3 in your system.
    echo - If it is not installed, go to https://www.python.org/downloads/
    echo and install the latest version of Python 3.
    echo - If it is installed, make sure it is version 3, and added to PATH.
}

python_ref() {
    local my_ref=$1
    echo $($my_ref -c 'import platform; major, minor, patch = platform.python_version_tuple(); print(major); print(minor);')
}

install_dependencies() {
    echo [INFO] Installing dependencies. This may take a while.
    sudo apt-get install python-pyqt5
    sudo apt-get install vlc
    pip install -r requirements.txt
}

run_database_viewer() {
    echo [INFO] Running SVQA Database Viewer...
    local python_ref=$1
    local cmd="$python_ref dataset_viewer.py"
    eval $cmd
    exit 0
}

# Print success_msg/error_msg according to the provided minimum required versions
check_version() {
    echo [INFO] Checking Python version...
    local major=$1
    local minor=$2
    local python_ref=$3
    [[ $major -ge $PYTHON_MINIMUM_MAJOR && $minor -ge $PYTHON_MINIMUM_MINOR ]] && python3_has_found || error_msg
}

python3_has_found() {
    echo [INFO] Python 3 is found.
    echo [DEBUG] Python path: $python_ref 
    install_dependencies
    run_database_viewer $python_ref
}

# Logic
if [[ ! -z $PYTHON3_REF ]]; then
    version=($(python_ref python3))
    check_version ${version[0]} ${version[1]} $PYTHON3_REF
    # Found Python 3.
elif [[ ! -z $PYTHON_REF ]]; then
    # Didn't find python3, let's try python
    version=($(python_ref python))
    check_version ${version[0]} ${version[1]} $PYTHON_REF
else
    # Python is not installed at all
    error_msg
fi
