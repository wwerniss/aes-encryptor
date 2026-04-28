#include "AesSequential.h"
#include "AesBlock.h"
#include <stdexcept>
#include <array>

std::vector<uint8_t> AesSequential::encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    return processCtrMode(data, key);
}

std::vector<uint8_t> AesSequential::decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    // CTR mode encryption and decryption are identical
    return processCtrMode(data, key);
}

std::vector<uint8_t> AesSequential::processCtrMode(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    if (key.size() != AesBlock::KEY_SIZE) {
        throw std::invalid_argument("Key must be exactly 16 bytes for AES-128.");
    }

    std::vector<uint8_t> output(data.size());
    std::array<uint8_t, AesBlock::KEY_SIZE> keyArray;
    std::copy(key.begin(), key.end(), keyArray.begin());

    // Basic 16-byte counter (nonce + counter). initialized to zero for simplicity in this lab.
    std::array<uint8_t, AesBlock::BLOCK_SIZE> counter = {0};

    size_t numBlocks = (data.size() + AesBlock::BLOCK_SIZE - 1) / AesBlock::BLOCK_SIZE;

    for (size_t i = 0; i < numBlocks; ++i) {
        // Set counter value (using block index 'i' as the counter part)
        // Just incrementing the lower 8 bytes for simplicity
        uint64_t blockIndex = i;
        for (int j = 15; j >= 8; --j) {
            counter[j] = static_cast<uint8_t>(blockIndex & 0xFF);
            blockIndex >>= 8;
        }

        std::array<uint8_t, AesBlock::BLOCK_SIZE> encryptedCounter = AesBlock::encryptBlock(counter, keyArray);

        size_t offset = i * AesBlock::BLOCK_SIZE;
        size_t bytesToXor = std::min(AesBlock::BLOCK_SIZE, data.size() - offset);

        for (size_t j = 0; j < bytesToXor; ++j) {
            output[offset + j] = data[offset + j] ^ encryptedCounter[j];
        }
    }

    return output;
}
