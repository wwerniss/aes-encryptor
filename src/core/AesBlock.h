#pragma once

#include <vector>
#include <cstdint>
#include <array>

/**
 * @brief Class representing a single 128-bit block of AES encryption.
 * 
 * Provides the core transformations (SubBytes, ShiftRows, MixColumns, AddRoundKey, KeyExpansion)
 * necessary for AES-128 encryption. This implementation focuses on simplicity and correctness
 * rather than maximum performance, to be used as a building block for sequential and parallel
 * modes of operation.
 */
class AesBlock {
public:
    static constexpr size_t BLOCK_SIZE = 16;
    static constexpr size_t KEY_SIZE = 16;
    static constexpr size_t NUM_ROUNDS = 10;

    /**
     * @brief Encrypts a single 16-byte block.
     * 
     * @param input The 16-byte input block (plaintext).
     * @param key The 16-byte key.
     * @return std::array<uint8_t, BLOCK_SIZE> The 16-byte encrypted block (ciphertext).
     */
    static std::array<uint8_t, BLOCK_SIZE> encryptBlock(
        const std::array<uint8_t, BLOCK_SIZE>& input,
        const std::array<uint8_t, KEY_SIZE>& key);

private:
    static void addRoundKey(std::array<uint8_t, BLOCK_SIZE>& state, const std::vector<uint8_t>& roundKey, size_t round);
    static void subBytes(std::array<uint8_t, BLOCK_SIZE>& state);
    static void shiftRows(std::array<uint8_t, BLOCK_SIZE>& state);
    static void mixColumns(std::array<uint8_t, BLOCK_SIZE>& state);
    static std::vector<uint8_t> expandKey(const std::array<uint8_t, KEY_SIZE>& key);
};
