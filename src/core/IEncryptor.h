#pragma once

#include <vector>
#include <cstdint>

/**
 * @brief Interface for AES encryptor implementations.
 * 
 * Using the Strategy design pattern to allow switching between
 * sequential and parallel implementations of AES encryption.
 */
class IEncryptor {
public:
    virtual ~IEncryptor() = default;

    /**
     * @brief Encrypts the provided data using the given key.
     * 
     * @param data The input plaintext data.
     * @param key The 16-byte AES-128 key.
     * @return std::vector<uint8_t> The encrypted ciphertext.
     */
    virtual std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) = 0;

    /**
     * @brief Decrypts the provided data using the given key.
     * 
     * @param data The input ciphertext data.
     * @param key The 16-byte AES-128 key.
     * @return std::vector<uint8_t> The decrypted plaintext.
     */
    virtual std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) = 0;
};
