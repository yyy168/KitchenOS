#!/bin/bash

# 1. Setup & Dependencies
mkdir -p backend/include
if [ ! -f "backend/include/httplib.h" ]; then
    echo "Downloading networking library..."
    curl -sL https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -o backend/include/httplib.h
fi

# 2. Compile Everything (The Build Stage)
echo "Building KitchenOS components..."
g++ -std=c++17 backend/tests.cpp -Ibackend/include -lpthread -o unit_tests
TEST_BUILD_RESULT=$?

g++ -std=c++17 backend/main.cpp -Ibackend/include -lpthread -o recipe_server
SERVER_BUILD_RESULT=$?

if [ $TEST_BUILD_RESULT -ne 0 ] || [ $SERVER_BUILD_RESULT -ne 0 ]; then
    echo "❌ Compilation failed. Check your code for syntax errors."
    exit 1
fi

# 3. Run Unit Tests (The Verification Stage)
echo "Running Unit Tests..."
./unit_tests
if [ $? -ne 0 ]; then
    echo "❌ Unit tests failed! Aborting server launch to prevent deploying broken logic."
    exit 1
fi

# 4. Launch (The Deployment Stage)
echo "✅ All tests passed. Launching server on port 8080..."
chmod +x recipe_server
./recipe_server
