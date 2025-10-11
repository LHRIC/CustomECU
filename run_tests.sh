#!/bin/bash

# Script to build and run Unity tests for CustomECU project

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Building and running CustomECU tests...${NC}\n"

# Navigate to test directory
cd "$(dirname "$0")/test"

# Clean and create build directory
if [ -d "build" ]; then
    echo -e "${YELLOW}Cleaning old build...${NC}"
    rm -rf build
fi

mkdir build
cd build

# Configure with CMake
echo -e "${YELLOW}Configuring tests with CMake...${NC}"
cmake ..

# Build tests
echo -e "${YELLOW}Building tests...${NC}"
make

# Create symlink to compile_commands.json for IDE support
if [ -f "compile_commands.json" ]; then
    echo -e "${YELLOW}Creating symlink to compile_commands.json...${NC}"
    ln -sf build/compile_commands.json ../compile_commands.json
fi

# Run tests
echo -e "\n${YELLOW}Running tests with CTest...${NC}\n"
if ctest --output-on-failure --verbose; then
    echo -e "\n${GREEN}✓ All tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}✗ Some tests failed!${NC}"
    exit 1
fi
