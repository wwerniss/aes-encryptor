#include "EncryptorFactory.h"
#include "AesSequential.h"
#include "AesParallel.h"
#include <stdexcept>

std::unique_ptr<IEncryptor> EncryptorFactory::create(EncryptorType type, size_t numThreads) {
    switch (type) {
        case EncryptorType::Sequential:
            return std::make_unique<AesSequential>();
        case EncryptorType::Parallel:
            return std::make_unique<AesParallel>(numThreads);
        default:
            throw std::invalid_argument("Unknown EncryptorType");
    }
}
