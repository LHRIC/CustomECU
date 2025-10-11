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

# Run tests
echo -e "\n${YELLOW}Running tests...${NC}\n"
if ./run_tests; then
    echo -e "\n${GREEN}✓ All tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}✗ Some tests failed!${NC}"
    exit 1
fi
