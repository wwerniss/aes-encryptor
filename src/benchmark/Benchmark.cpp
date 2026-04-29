#include "Benchmark.h"
#include <chrono>
#include <random>
#include <fstream>
#include <iomanip>

BenchmarkResult Benchmark::run(IEncryptor& encryptor, const std::string& modeName, size_t dataSize, size_t numThreads) {
    // Generate dummy data
    std::vector<uint8_t> data(dataSize, 0xAB);
    std::vector<uint8_t> key(16, 0x11);

    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform encryption
    auto encrypted = encryptor.encrypt(data, key);

    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end - start;

    return BenchmarkResult{
        modeName,
        dataSize,
        numThreads,
        duration.count()
    };
}

void Benchmark::saveResultsToFile(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;

    file << "=== AES Encryption Benchmark Results ===\n";
    file << std::left << std::setw(25) << "Mode"
         << std::setw(15) << "Data Size (B)"
         << std::setw(15) << "Threads"
         << std::setw(15) << "Time (ms)" << "\n";
    file << std::string(70, '-') << "\n";

    for (const auto& res : results) {
        file << std::left << std::setw(25) << res.modeName
             << std::setw(15) << res.dataSizeBytes
             << std::setw(15) << res.numThreads
             << std::setw(15) << std::fixed << std::setprecision(3) << res.executionTimeMs << "\n";
    }
    file << "\n";
}
