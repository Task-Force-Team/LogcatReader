# Task-Force Logcat Reader!
LogcatReader is a powerful and user-friendly GTKmm-based application designed to monitor and analyze log files from Android devices. It provides real-time log monitoring, filtering, and highlighting features to help developers and testers diagnose issues efficiently.

## Features
- **Real-time Log Monitoring**: Capture and display logs in real-time from connected Android devices.
- **Filtering Options**: Filter logs by type (Errors, Warnings, Info, Kernel logs, Driver log).
- **Highlighting**: Automatically highlight different types of log entries for easy identification.
- **Search Functionality**: Quickly search for specific log entries within the displayed logs.
- **User-friendly Interface**: Intuitive and easy-to-use GTKmm-based graphical user interface.

## Building
I see that youre intrested in this tool, for building follow this guide
- **Dependency's**: Theres only GTKmm for installing,
Debian/Ubuntu:
```sh
sudo apt-get install libgtkmm-3.0-dev
```

Arch Linux:
```sh
sudo pacman -Sy gtkmm3
```

Fedora:
```sh
sudo dnf install gtkmm30
```

openSUSE:
```sh
sudo zypper install gtkmm3-devel
```

CentOS/Red Hat:
```sh
sudo yum install gtkmm30
```

Gentoo:
```sh
sudo emerge --ask dev-cpp/gtkmm:3.0
```

Magenia
```sh
sudo urpmi gtkmm30
```
- **Build Time!**: Ok! its build time, do:
```sh
make
```

- **Launching**: Very good, for launching just do:
```sh
./logcatreader
```

##License

This project is licensed under the MIT License - see the LICENSE.md file for details.
