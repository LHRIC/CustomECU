# CustomECU

[![CI](https://github.com/LHRIC/CustomECU/actions/workflows/ci.yml/badge.svg)](https://github.com/LHRIC/CustomECU/actions/workflows/ci.yml)
[![Unit Tests](https://github.com/LHRIC/CustomECU/actions/workflows/test.yml/badge.svg)](https://github.com/LHRIC/CustomECU/actions/workflows/test.yml)
[![Build Firmware](https://github.com/LHRIC/CustomECU/actions/workflows/build.yml/badge.svg)](https://github.com/LHRIC/CustomECU/actions/workflows/build.yml)

Custom ECU firmware for Honda CBR600CC engine control using STM32F407VET6.

## Features

- **Crank/Cam Synchronization**: Detects engine position using 12-tooth crank wheel and 3-tooth cam wheel
- **Real-time Engine Angle Calculation**: High-precision angle tracking with sub-tooth resolution
- **FreeRTOS**: Real-time operating system for deterministic task scheduling
- **Comprehensive Testing**: Unity-based unit tests with full mock infrastructure

## Hardware

- **MCU**: STM32F407VET6 (ARM Cortex-M4F, 168MHz)
- **Engine**: Honda CBR600CC
- **Sensors**:
  - Crank position sensor (12 equally-spaced teeth)
  - Cam position sensor (3 teeth, one offset 30°)

## Building

### Firmware

```bash
./build_firmware.sh
```

Or manually:

```bash
cd build
cmake ..
make
```

### Unit Tests

```bash
./run_tests.sh
```

Or manually:

```bash
cd test/build
cmake ..
make
ctest --verbose
```

## Project Structure

```
CustomECU/
├── Core/
│   ├── Inc/              # Header files
│   │   ├── engine.h      # Engine control functions
│   │   ├── sampling.hpp  # Sensor sampling and sync
│   │   └── ...
│   └── Src/              # Source files
│       ├── engine.cpp    # Engine control implementation
│       ├── sampling.cpp  # Sensor sampling implementation
│       └── ...
├── test/                 # Unit tests
│   ├── unit/             # Test files
│   ├── mocks/            # Mock implementations
│   └── frameworks/       # Unity & CMock
├── Drivers/              # STM32 HAL drivers
└── Middlewares/          # FreeRTOS
```

## Development

### Prerequisites

- ARM GCC toolchain (`gcc-arm-none-eabi`)
- CMake (≥ 3.22)
- Make or Ninja
- Python 3 (for code generation tools)

### Debugging

Debug configuration is available for VS Code with Cortex-Debug extension:

```bash
# Launch debug session
# Press F5 in VS Code
```

### Testing

See [test/README.md](test/README.md) for detailed testing documentation.

## License

See [LICENSE](LICENSE) file for details.

## Contributing

1. Create a feature branch
2. Make your changes
3. Ensure tests pass: `./run_tests.sh`
4. Ensure firmware builds: `./build_firmware.sh`
5. Submit a pull request

## CI/CD

GitHub Actions automatically:
- Runs unit tests on every push
- Builds firmware
- Uploads artifacts
- Reports test results

Check the Actions tab for build status.
