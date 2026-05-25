# Deben API Control Panel

## Overview

The **Deben API Control Panel** is a Windows-based hardware-control prototype for interacting with a **Deben Microtest mechanical testing system** through the vendor-provided `DebenMT64.dll`.

This repository contains a C++ wrapper around the Deben DLL and a Qt-based graphical user interface for issuing basic control commands to the Deben Microtest software. The goal of this subsystem is to provide a safer and more usable software bridge between future machine-learning deformation analysis output and the physical materials-testing hardware.

This project is part of the Oregon State University capstone project:

**Applying a Machine Learning-powered Localized Deformation Analyzer for Digital Twin Applications in Materials Testing**

The larger project investigates how SEM image analysis, Digital Image Correlation, and machine-learning optical-flow models can support faster deformation analysis for materials testing. This repository focuses specifically on the Deben hardware-control portion of that pipeline.

---

## Current Status

The current implementation is a **hardware-control prototype**.

Implemented features include:

- Dynamic loading of the 64-bit `DebenMT64.dll`
- Connection to the Deben Microtest software interface
- Qt GUI prototype for user-facing control
- Basic command controls for applying load requests
- Emergency stop behavior
- Event/status logging
- Runtime deployment support through Qt `windeployqt`

The prototype has been tested with the Deben Microtest setup and confirmed to send direct load commands through the wrapper. Earlier testing showed that calling `MT_StartMotor()` before user input could cause unexpected motor movement, so the current intended behavior is to send explicit user-commanded load requests instead.

The full closed-loop system, where machine-learning output automatically informs Deben tester commands, has not yet been completed.

---

## Repository Structure

A typical structure is:

```text
Deben-API/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── GUImain.cpp
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   ├── DebenController.cpp
│   ├── DebenController.h
│   ├── Logger.cpp
│   ├── Logger.h
│   ├── MLCommand.cpp
│   ├── MLCommand.h
│   ├── Config.cpp
│   ├── Config.h
│   ├── menu.cpp
│   └── menu.h
└── build/
```

Main components:

| Component | Purpose |
|---|---|
| `GUImain.cpp` | Application entry point for the Qt GUI |
| `MainWindow.*` | Main Qt window and user interface behavior |
| `DebenController.*` | Wrapper/controller logic for communicating with the Deben DLL |
| `Logger.*` | Event and status logging support |
| `MLCommand.*` | Placeholder or bridge logic for future ML-driven commands |
| `Config.*` | Configuration-related support |
| `CMakeLists.txt` | CMake build configuration |

File names may differ slightly depending on the final repository state.

---

## System Requirements

This project is intended to run on a Windows workstation configured with the Deben Microtest software.

Required software:

- Windows 10 or Windows 11
- CMake
- 64-bit MinGW / UCRT64 toolchain
- Qt 6.11.0 or compatible Qt 6 version
- Deben Microtest software
- `DebenMT64.dll`

### Important Architecture Requirement

The application must be built as a **64-bit executable** because it loads the 64-bit Deben DLL:

```text
DebenMT64.dll
```

Using a 32-bit compiler or mismatched toolchain can cause DLL loading failures.

---

## Installing Dependencies

### 1. Install Qt

Install Qt 6 with the MinGW 64-bit kit.

The tested setup used:

```text
C:\Qt\6.11.0\mingw_64
```

If Qt is installed somewhere else, update the `CMAKE_PREFIX_PATH` in the build command.

### 2. Install MinGW / UCRT64

Install a 64-bit MinGW toolchain. One tested setup used MSYS2 UCRT64.

Check the compiler with:

```powershell
g++ --version
where g++
```

The compiler path should point to a 64-bit toolchain, such as:

```text
C:\msys64\ucrt64\bin\g++.exe
```

### 3. Confirm Deben DLL Availability

The Deben vendor DLL must be available on the workstation.

Expected DLL:

```text
DebenMT64.dll
```

Depending on the implementation, the DLL may need to be:

- In the same directory as `DebenGui.exe`
- In a configured DLL path
- In a vendor software installation directory available through the system path

Do **not** commit proprietary vendor DLLs to the public repository unless explicitly permitted.

---

## Building the Project

From the root of the repository, run:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.0\mingw_64"
cmake --build build
```

If Qt is installed somewhere else, replace the path:

```powershell
-DCMAKE_PREFIX_PATH="C:\Path\To\Qt\mingw_64"
```

The executable should be generated under:

```text
build/DebenGui.exe
```

---

## Deploying Qt Runtime Files

After building, run Qt's deployment tool:

```powershell
C:\Qt\6.11.0\mingw_64\bin\windeployqt.exe .\build\DebenGui.exe
```

This copies the required Qt runtime DLLs and plugins beside the executable.

This step is important. Without it, the GUI may build successfully but fail to launch without showing a clear error message.

---

## Running the GUI

After building and deploying Qt runtime files, run:

```powershell
.\build\DebenGui.exe
```

The GUI should open and allow the user to connect to the Deben software interface and issue supported commands.

The Deben Microtest software should be installed and available before attempting hardware control.

---

## Basic Usage Workflow

A typical usage sequence is:

1. Start the Deben Microtest software.
2. Connect the workstation to the Deben Microtest hardware.
3. Launch `DebenGui.exe`.
4. Use the GUI to connect/link to the Deben software interface.
5. Confirm connection or status messages in the GUI.
6. Enter a safe load command.
7. Send the command through the GUI.
8. Monitor the tester and event log.
9. Use emergency stop if unsafe behavior occurs.

---

## Safety Notes

This software interacts with physical mechanical-testing hardware. Use caution.

Before sending commands:

- Confirm the Deben hardware is properly connected.
- Confirm the sample and fixture are safely mounted.
- Confirm the requested load is within safe limits.
- Confirm the Deben software is running correctly.
- Keep access to emergency stop controls.
- Do not run unattended tests with this prototype.

The current prototype includes emergency-stop behavior, but a full centralized safety-validation layer is still future work.

Future development should enforce limits for:

- Maximum force
- Extension
- Displacement rate
- Command frequency
- Human confirmation before actuation
- Safe shutdown behavior

---

## Known Limitations

The current Deben control panel is a prototype and has several limitations.

### 1. No closed-loop ML integration yet

The GUI currently accepts direct user commands. It is not yet connected to RAFT model predictions or an automated decision layer.

### 2. Limited hardware validation

The wrapper has been tested with the available Deben Microtest setup, but it has not been validated across all possible load profiles, displacement settings, long-duration experiments, or safety thresholds.

### 3. Windows-only hardware dependency

This subsystem depends on the vendor Deben DLL and Deben Microtest software stack, so it is intended for Windows workstations. It is not designed to run on the Linux/HPC environment used by the ML training pipeline.

### 4. Thin hardware abstraction

The GUI, controller, and DLL wrapper are functional but should be more cleanly separated in future versions.

### 5. Incomplete safety layer

The project should eventually centralize all hardware command validation before any command reaches the Deben DLL.

---

## Common Pitfalls

### GUI builds but does not open

Run:

```powershell
C:\Qt\6.11.0\mingw_64\bin\windeployqt.exe .\build\DebenGui.exe
```

Then try launching again:

```powershell
.\build\DebenGui.exe
```

### DLL loading fails

Confirm that:

- You are building a 64-bit executable.
- You are using a 64-bit MinGW/UCRT64 compiler.
- `DebenMT64.dll` is available to the executable.
- The DLL path is correct.
- The Deben vendor software is installed.

### CMake cannot find Qt

If CMake gives an error involving `Qt6Config.cmake`, pass the Qt installation path manually:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.0\mingw_64"
```

### Motor starts unexpectedly

Earlier testing showed that calling `MT_StartMotor()` before user input could cause unexpected motor movement.

The intended prototype behavior is to avoid starting the motor automatically and only send explicit user-commanded load requests.

---

## Development Notes

### Build System

This project uses CMake.

The CMake configuration should include Qt Widgets support, similar to:

```cmake
find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(DebenGui PRIVATE Qt6::Widgets)
```

For Qt-based applications, the project may also use:

```cmake
qt_standard_project_setup()
qt_add_executable(...)
```

### GUI Entry Point

The application should follow the normal Qt event-loop structure:

```cpp
QApplication app(argc, argv);
MainWindow window;
window.show();
return app.exec();
```

If debugging a silent launch failure, it can be useful to temporarily add `QMessageBox` checkpoints or logging statements around:

- Program start
- `QApplication` creation
- `MainWindow` construction
- `window.show()`
- `app.exec()`

---

## Relationship to the Full Capstone System

The full capstone project contains three major tracks:

1. DICe automation and training-data generation
2. RAFT-based machine-learning deformation prediction
3. Deben mechanical-tester control

This repository covers the third track.

The intended long-term workflow is:

```text
SEM image/video input
        ↓
DICe / RAFT deformation prediction
        ↓
Decision or command layer
        ↓
Deben API wrapper
        ↓
DebenMT64.dll
        ↓
Deben Microtest mechanical tester
```

At the time of handoff, the Deben control track is a working subsystem prototype, but the complete closed-loop integration is not finished.

---

## Future Work

Recommended next steps:

### 1. Add a formal safety-validation layer

Centralize checks for maximum force, extension, displacement rate, and command validity before sending commands to the Deben DLL.

### 2. Improve hardware abstraction

Separate the GUI, command validation, hardware communication, and logging layers more cleanly.

### 3. Define the ML-to-Deben interface

Specify how RAFT output should be converted into safe Deben commands. This should include data formats, command thresholds, and whether human confirmation is required.

### 4. Expand hardware testing

Test the wrapper across different load profiles, displacement settings, and longer experiments.

### 5. Improve operator documentation

Add screenshots of the GUI workflow and a short guide for future students operating the Deben Microtest system.

### 6. Add automated non-hardware tests

Add mock or simulated DLL behavior so future developers can test GUI and command logic without connecting to the physical hardware.

---

## Troubleshooting Checklist

Before reporting a bug, check:

- [ ] Is the project built with a 64-bit compiler?
- [ ] Is Qt installed correctly?
- [ ] Was `windeployqt` run on `DebenGui.exe`?
- [ ] Is `DebenMT64.dll` available to the executable?
- [ ] Is the Deben Microtest software installed?
- [ ] Is the Deben Microtest software running before the GUI connects?
- [ ] Is the hardware connected and powered?
- [ ] Are the requested load values within safe operating limits?
- [ ] Are there event log messages explaining the failure?

---

## Maintainers and Handoff

Original capstone contributor for this subsystem:

- Kyle Gemma
- Oregon State University
- Computer Science Capstone, Team CS.057

Faculty/project advisor:

- Dr. Tianyi Chen

The project is maintained under the OSU Enhancing Deformation Analysis GitHub organization.

---

## Disclaimer

This software is a research and educational prototype. It is not production-certified hardware-control software. Use caution when operating physical testing equipment, and always follow laboratory safety procedures and Deben Microtest operating guidelines.
