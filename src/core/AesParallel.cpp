#include "AesParallel.h"
#include "AesBlock.h"
#include <stdexcept>
#include <array>
#include <thread>
#include <future>
#include <algorithm>

AesParallel::AesParallel(size_t numThreads) {
    setNumThreads(numThreads);
}

void AesParallel::setNumThreads(size_t numThreads) {
    if (numThreads == 0) {
        m_numThreads = std::thread::hardware_concurrency();
        if (m_numThreads == 0) m_numThreads = 4; // Fallback
    } else {
        m_numThreads = numThreads;
    }
}

size_t AesParallel::getNumThreads() const {
    return m_numThreads;
}

std::vector<uint8_t> AesParallel::encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    return processCtrMode(data, key);
}

std::vector<uint8_t> AesParallel::decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    return processCtrMode(data, key);
}

std::vector<uint8_t> AesParallel::processCtrMode(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    if (key.size() != AesBlock::KEY_SIZE) {
        throw std::invalid_argument("Key must be exactly 16 bytes for AES-128.");
    }

    if (data.empty()) {
        return {};
    }

    std::vector<uint8_t> output(data.size());
    std::array<uint8_t, AesBlock::KEY_SIZE> keyArray;
    std::copy(key.begin(), key.end(), keyArray.begin());

    size_t numBlocks = (data.size() + AesBlock::BLOCK_SIZE - 1) / AesBlock::BLOCK_SIZE;

    // Distribute blocks among threads
    size_t actualThreads = std::min(m_numThreads, numBlocks);
    size_t blocksPerThread = numBlocks / actualThreads;
    size_t remainderBlocks = numBlocks % actualThreads;

    auto worker = [&](size_t startBlock, size_t endBlock) {
        std::array<uint8_t, AesBlock::BLOCK_SIZE> counter = {0};
        
        for (size_t i = startBlock; i < endBlock; ++i) {
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
    };

    std::vector<std::future<void>> futures;
    size_t currentBlock = 0;

    for (size_t i = 0; i < actualThreads; ++i) {
        size_t blocksForThisThread = blocksPerThread + (i < remainderBlocks ? 1 : 0);
        size_t endBlock = currentBlock + blocksForThisThread;

        futures.push_back(std::async(std::launch::async, worker, currentBlock, endBlock));
        currentBlock = endBlock;
    }

    for (auto& f : futures) {
        f.get();
    }

    return output;
}
