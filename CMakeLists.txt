cmake_minimum_required(VERSION 3.16)
project(IPCExchanger LANGUAGES CXX)

# Because #include "receiver.moc"  did not work
set(CMAKE_AUTOMOC ON)
# It just for seting basic requirements for compilation
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We need to find Boost with Boost System for linking 
find_package(Boost REQUIRED COMPONENTS system)

# We need to find Qt5 with these require modules
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

# App A - Sender (console) (We're building a sender file from main.cpp here)
add_executable(sender sender/main.cpp)
target_link_libraries(sender PRIVATE Boost::system)

# App B - Receiver (GUI) (We're building a receiver as well)
add_executable(receiver receiver/main.cpp)
target_link_libraries(receiver PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)

include_directories(${CMAKE_SOURCE_DIR}/common)