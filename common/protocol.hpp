#pragma once

#include <stdexcept>
#include <cstdint> // for unit_32
#include <vector>
#include <optional>


constexpr uint32_t MAGIC = 0xDEADBEEF;
constexpr uint32_t MAX_PAYLOAD_SIZE = 1'000'000;

inline std::vector<uint8_t> packMessage(int64_t timestamp_ms) {
    std::vector<uint8_t> frame;

    // MAGIC: writing down into the frame 4 bytes with little-endian 
    frame.push_back(MAGIC & 0xFF);
    frame.push_back((MAGIC >> 8) & 0xFF);
    frame.push_back((MAGIC >> 16) & 0xFF);
    frame.push_back((MAGIC >> 24) & 0xFF);

    // SIZE: 
    constexpr int64_t PAYLOAD_SIZE = 8;
    uint32_t payloadSize = static_cast<uint32_t>(PAYLOAD_SIZE);
    frame.push_back(payloadSize & 0xFF);
    frame.push_back((payloadSize >> 8) & 0xFF);
    frame.push_back((payloadSize >> 16) & 0xFF);
    frame.push_back((payloadSize >> 24) & 0xFF);

    // PAYLOAD: 
    for (int i = 0; i < 8; ++i) {
        frame.push_back(timestamp_ms & 0xFF);
        timestamp_ms >>= 8;
    }
    
    // result frame represent like: frame = [EF, BE, AD, DE, 08, 00, 00, 00, B0, B1, B2, B3, B4, B5, B6, B7]
    return frame;
}


inline std::optional<int64_t>unpackMessage(const std::vector<uint8_t>& buffer) {
}