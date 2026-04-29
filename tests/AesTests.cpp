#include <gtest/gtest.h>
#include "core/AesBlock.h"
#include "core/EncryptorFactory.h"
#include <array>
#include <vector>

// Helper to convert hex string to byte array
std::vector<uint8_t> hexToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

TEST(AesBlockTest, EncryptCorrectness) {
    // Standard AES-128 NIST test vector
    std::string keyHex = "2b7e151628aed2a6abf7158809cf4f3c";
    std::string ptHex  = "6bc1bee22e409f96e93d7e117393172a";
    std::string ctHex  = "3ad77bb40d7a3660a89ecaf32466ef97";

    auto keyBytes = hexToBytes(keyHex);
    auto ptBytes = hexToBytes(ptHex);
    auto ctBytes = hexToBytes(ctHex);

    std::array<uint8_t, 16> keyArray, ptArray, ctArray;
    std::copy_n(keyBytes.begin(), 16, keyArray.begin());
    std::copy_n(ptBytes.begin(), 16, ptArray.begin());
    std::copy_n(ctBytes.begin(), 16, ctArray.begin());

    auto result = AesBlock::encryptBlock(ptArray, keyArray);
    
    EXPECT_EQ(result, ctArray);
}

TEST(AesEncryptorTest, SequentialVsParallel) {
    std::string keyHex = "00112233445566778899aabbccddeeff";
    auto key = hexToBytes(keyHex);

    // Large payload to ensure multi-threading logic triggers
    std::vector<uint8_t> plaintext(1024 * 1024, 0x42); // 1 MB of 0x42

    auto seq = EncryptorFactory::create(EncryptorType::Sequential);
    auto par = EncryptorFactory::create(EncryptorType::Parallel, 4);

    auto seqCipher = seq->encrypt(plaintext, key);
    auto parCipher = par->encrypt(plaintext, key);

    // Ensure parallel matches sequential perfectly
    EXPECT_EQ(seqCipher, parCipher);

    // Decrypt and match original
    auto decPlaintext = par->decrypt(parCipher, key);
    EXPECT_EQ(plaintext, decPlaintext);
}

TEST(AesEncryptorTest, InvalidKeySize) {
    auto seq = EncryptorFactory::create(EncryptorType::Sequential);
    std::vector<uint8_t> data(16, 0);
    std::vector<uint8_t> badKey(15, 0); // 15 bytes instead of 16
    
    EXPECT_THROW(seq->encrypt(data, badKey), std::invalid_argument);
}

TEST(AesEncryptorTest, EmptyData) {
    auto seq = EncryptorFactory::create(EncryptorType::Sequential);
    auto par = EncryptorFactory::create(EncryptorType::Parallel, 2);
    
    std::string keyHex = "00112233445566778899aabbccddeeff";
    auto key = hexToBytes(keyHex);
    std::vector<uint8_t> emptyData;

    auto seqCipher = seq->encrypt(emptyData, key);
    auto parCipher = par->encrypt(emptyData, key);

    EXPECT_TRUE(seqCipher.empty());
    EXPECT_TRUE(parCipher.empty());
}

TEST(AesEncryptorTest, UnalignedDataSize) {
    auto seq = EncryptorFactory::create(EncryptorType::Sequential);
    auto par = EncryptorFactory::create(EncryptorType::Parallel, 4);

    std::string keyHex = "00112233445566778899aabbccddeeff";
    auto key = hexToBytes(keyHex);

    // 1023 bytes (not a multiple of 16)
    std::vector<uint8_t> plaintext(1023, 0x77);

    auto seqCipher = seq->encrypt(plaintext, key);
    auto parCipher = par->encrypt(plaintext, key);

    EXPECT_EQ(seqCipher.size(), 1023);
    EXPECT_EQ(parCipher.size(), 1023);
    EXPECT_EQ(seqCipher, parCipher);

    auto seqDec = seq->decrypt(seqCipher, key);
    EXPECT_EQ(seqDec, plaintext);
}

TEST(AesEncryptorTest, ParallelDecryption) {
    auto par = EncryptorFactory::create(EncryptorType::Parallel, 8);
    std::string keyHex = "00112233445566778899aabbccddeeff";
    auto key = hexToBytes(keyHex);

    std::vector<uint8_t> plaintext(5000, 0x12);
    auto cipher = par->encrypt(plaintext, key);
    auto decrypted = par->decrypt(cipher, key);

    EXPECT_EQ(plaintext, decrypted);
}

TEST(AesEncryptorTest, DifferentThreadCounts) {
    std::string keyHex = "00112233445566778899aabbccddeeff";
    auto key = hexToBytes(keyHex);
    std::vector<uint8_t> plaintext(1024 * 10, 0x99); // 10 KB

    auto seq = EncryptorFactory::create(EncryptorType::Sequential);
    auto seqCipher = seq->encrypt(plaintext, key);

    std::vector<size_t> threadCounts = {1, 2, 3, 4, 8, 16, 64};
    for (size_t threads : threadCounts) {
        auto par = EncryptorFactory::create(EncryptorType::Parallel, threads);
        auto parCipher = par->encrypt(plaintext, key);
        EXPECT_EQ(seqCipher, parCipher) << "Mismatch with thread count: " << threads;
    }
}
