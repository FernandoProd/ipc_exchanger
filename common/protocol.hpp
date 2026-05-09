#pragma once

#include <stdexcept>
#include <cstdint> // for unit_32
#include <vector>
#include <optional>

namespace ipc {
namespace protocol {

constexpr uint32_t MAGIC = 0xDEADBEEF;   // This value calculated in compilation time and doesnt keep in memory
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


inline std::optional<int64_t> unpackMessage(const std::vector<uint8_t>& buffer) {
    // Mimimal length: 4 (MAGIC) + 4 (SIZE) + 8 (PAYLOAD)
    if (buffer.size() < 12) {
        return std::nullopt;
    }
    
    // Check MAGIC
    uint32_t magic = 
        (static_cast<uint32_t>(buffer[0]) << 0) |
        (static_cast<uint32_t>(buffer[1]) << 8) |
        (static_cast<uint32_t>(buffer[2]) << 16) |
        (static_cast<uint32_t>(buffer[3]) << 24);
    if (magic != MAGIC) {
        return std::nullopt;  // not ours frame
    }
    
    // Read SIZE
    uint32_t size = 
        (static_cast<uint32_t>(buffer[4]) << 0) |
        (static_cast<uint32_t>(buffer[5]) << 8) |
        (static_cast<uint32_t>(buffer[6]) << 16) |
        (static_cast<uint32_t>(buffer[7]) << 24);
    
    // Ceck limit
    if (size > MAX_PAYLOAD_SIZE) {
        return std::nullopt;
    }
    
    // check that buffer size is big enough
    if (buffer.size() < 8 + size) {
        return std::nullopt;
    }
    
    
    if (size != 8) {
        return std::nullopt;  
    }
    
    // Keep timestamp (little-endian)
    int64_t timestamp = 0;
    for (int i = 0; i < 8; ++i) {
        timestamp |= (static_cast<int64_t>(buffer[8 + i]) << (i * 8));
    }
    
    return timestamp;
}

}  
} 