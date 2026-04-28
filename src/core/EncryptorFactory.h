#pragma once

#include "IEncryptor.h"
#include <memory>

enum class EncryptorType {
    Sequential,
    Parallel
};

/**
 * @brief Factory class to create encryptor instances.
 */
class EncryptorFactory {
public:
    /**
     * @brief Creates an IEncryptor instance.
     * 
     * @param type The type of encryptor to create.
     * @param numThreads For parallel encryptor, specifies thread count. 0 for auto.
     * @return std::unique_ptr<IEncryptor> The newly created encryptor.
     */
    static std::unique_ptr<IEncryptor> create(EncryptorType type, size_t numThreads = 0);
};
