#pragma once

#include "IEncryptor.h"
#include <vector>
#include <cstdint>

/**
 * @brief Multithreaded implementation of AES encryption.
 * 
 * Uses CTR (Counter) mode, executing block operations concurrently across multiple threads.
 */
class AesParallel : public IEncryptor {
public:
    /**
     * @brief Constructor for AesParallel.
     * @param numThreads The number of threads to use. If 0, it uses hardware concurrency.
     */
    explicit AesParallel(size_t numThreads = 0);

    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) override;
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) override;

    void setNumThreads(size_t numThreads);
    size_t getNumThreads() const;

private:
    std::vector<uint8_t> processCtrMode(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);

    size_t m_numThreads;
};
