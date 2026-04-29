#pragma once

#include "core/IEncryptor.h"
#include <vector>
#include <string>
#include <cstdint>

struct BenchmarkResult {
    std::string modeName;
    size_t dataSizeBytes;
    size_t numThreads;
    double executionTimeMs;
};

/**
 * @brief Utility class to measure execution time of encryption operations.
 */
class Benchmark {
public:
    /**
     * @brief Runs a benchmark on the provided encryptor.
     * 
     * @param encryptor The encryptor to test.
     * @param modeName Descriptive name (e.g., "Sequential", "Parallel (4 threads)").
     * @param dataSize Size of the data to test with, in bytes.
     * @param numThreads The number of threads used (for reporting).
     * @return BenchmarkResult The measured result.
     */
    static BenchmarkResult run(IEncryptor& encryptor, const std::string& modeName, size_t dataSize, size_t numThreads);

    /**
     * @brief Saves a collection of benchmark results to a file.
     * 
     * @param results The list of results to save.
     * @param filename The path to the file.
     */
    static void saveResultsToFile(const std::vector<BenchmarkResult>& results, const std::string& filename);
};
