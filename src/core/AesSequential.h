#pragma once

#include "IEncryptor.h"
#include <vector>
#include <cstdint>

/**
 * @brief Sequential implementation of AES encryption.
 * 
 * Uses CTR (Counter) mode, executing block operations sequentially on a single thread.
 */
class AesSequential : public IEncryptor {
public:
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) override;
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) override;

private:
    std::vector<uint8_t> processCtrMode(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);
};
