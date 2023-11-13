# LLP 1-st laboratory work

Вариант: Реляционная база данных, чтение-запись

## Prerequisites

- CMake
- Ninja

## Setup on Windows

Make sure you have set the following environment variables:

1. **CMake:**
    - Example: `C:\Program Files\JetBrains\CLion 2022.2.1\bin\cmake\win\bin`

2. **Ninja:**
    - Example: `C:\Program Files\JetBrains\CLion 2022.2.1\bin\ninja\win\`

## How to Build and Run

### Windows

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S . -B build-dir
cmake --build build-dir --target all
```
### Linux
```bash
cmake . -B build-dir
cmake --build build-dir --target all
```
