#include "MainWindow.h"
#include "core/EncryptorFactory.h"
#include "benchmark/Benchmark.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QElapsedTimer>
#include <thread>
#include <future>
#include <chrono>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("AES-128 Multithreaded Encryptor (Lab 3b)");
    setMinimumSize(800, 600);
    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    QFormLayout* formLayout = new QFormLayout();

    keyInput = new QLineEdit();
    keyInput->setText("00112233445566778899aabbccddeeff");
    keyInput->setPlaceholderText("Enter 32 hex characters (16 bytes)");
    formLayout->addRow("AES-128 Key (Hex):", keyInput);

    modeSelector = new QComboBox();
    modeSelector->addItem("Sequential");
    modeSelector->addItem("Parallel");
    formLayout->addRow("Execution Mode:", modeSelector);

    threadSpinner = new QSpinBox();
    unsigned int logicalProcessors = std::thread::hardware_concurrency();
    if (logicalProcessors == 0) logicalProcessors = 4; 
    
    threadSpinner->setMinimum(1);
    threadSpinner->setMaximum(logicalProcessors);
    threadSpinner->setValue(logicalProcessors);
    threadSpinner->setToolTip(QString("Available logical processors: %1").arg(logicalProcessors));
    formLayout->addRow("Thread Count (Parallel):", threadSpinner);

    mainLayout->addLayout(formLayout);

    QHBoxLayout* actionsLayout = new QHBoxLayout();
    
    btnEncryptFile = new QPushButton("Encrypt File...");
    btnDecryptFile = new QPushButton("Decrypt File...");
    btnRunBenchmark = new QPushButton("Run Benchmark (1KB-10MB)");

    actionsLayout->addWidget(btnEncryptFile);
    actionsLayout->addWidget(btnDecryptFile);
    actionsLayout->addWidget(btnRunBenchmark);

    mainLayout->addLayout(actionsLayout);

    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    mainLayout->addWidget(progressBar);

    logOutput = new QTextEdit();
    logOutput->setReadOnly(true);
    QFont font("Courier");
    font.setStyleHint(QFont::Monospace);
    logOutput->setFont(font);
    mainLayout->addWidget(logOutput);

    connect(btnEncryptFile, &QPushButton::clicked, this, &MainWindow::onEncryptFileClicked);
    connect(btnDecryptFile, &QPushButton::clicked, this, &MainWindow::onDecryptFileClicked);
    connect(btnRunBenchmark, &QPushButton::clicked, this, &MainWindow::onRunBenchmarkClicked);
}

std::vector<uint8_t> MainWindow::getKeyFromInput() {
    QString hexStr = keyInput->text().remove(' ');
    if (hexStr.length() != 32) {
        throw std::invalid_argument("Key must be exactly 32 hex characters (16 bytes).");
    }

    std::vector<uint8_t> key(16);
    for (int i = 0; i < 16; ++i) {
        bool ok;
        key[i] = hexStr.mid(i * 2, 2).toUInt(&ok, 16);
        if (!ok) {
            throw std::invalid_argument("Invalid hexadecimal character in key.");
        }
    }
    return key;
}

void MainWindow::onEncryptFileClicked() {
    processFile(true);
}

void MainWindow::onDecryptFileClicked() {
    processFile(false);
}

void MainWindow::processFile(bool isEncrypt) {
    QString title = isEncrypt ? "Select File to Encrypt" : "Select File to Decrypt";
    QString inputPath = QFileDialog::getOpenFileName(this, title, QDir::currentPath()); // QDir::homePath()
    
    if (inputPath.isEmpty()) return;

    QString outputPath = inputPath + (isEncrypt ? ".enc" : ".dec");

    try {
        std::vector<uint8_t> key = getKeyFromInput();
        
        QFile inFile(inputPath);
        if (!inFile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Could not open input file.");
        }
        QByteArray inData = inFile.readAll();
        inFile.close();

        std::vector<uint8_t> data(inData.begin(), inData.end());
        
        EncryptorType type = modeSelector->currentIndex() == 0 ? EncryptorType::Sequential : EncryptorType::Parallel;
        auto encryptor = EncryptorFactory::create(type, threadSpinner->value());

        logOutput->append(QString("Processing file: %1").arg(inputPath));
        logOutput->append(QString("Size: %1 bytes").arg(data.size()));
        logOutput->append("Working...");
        QApplication::processEvents();

        QElapsedTimer timer;
        timer.start();

        std::vector<uint8_t> result;
        if (isEncrypt) {
            result = encryptor->encrypt(data, key);
        } else {
            result = encryptor->decrypt(data, key);
        }

        qint64 elapsedMs = timer.elapsed();

        QFile outFile(outputPath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            throw std::runtime_error("Could not open output file.");
        }
        outFile.write(reinterpret_cast<const char*>(result.data()), result.size());
        outFile.close();

        logOutput->append(QString("Success! Time taken: %1 ms").arg(elapsedMs));
        logOutput->append(QString("Saved to: %1\n").arg(outputPath));
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        logOutput->append(QString("Error: %1\n").arg(e.what()));
    }
}

void MainWindow::onRunBenchmarkClicked() {
    try {
        std::vector<uint8_t> key = getKeyFromInput();
        std::vector<size_t> sizesToTest = {
            1 * 1024,             // 1 KB
            10 * 1024,            // 10 KB
            100 * 1024,           // 100 KB
            1 * 1024 * 1024,      // 1 MB
            10 * 1024 * 1024      // 10 MB
        };

        logOutput->append("=== Starting Benchmark ===");
        logOutput->append("Testing sizes: 1KB, 10KB, 100KB, 1MB, 10MB");
        logOutput->append("This might take a while...\n");
        QApplication::processEvents();

        std::vector<BenchmarkResult> results;
        int totalSteps = sizesToTest.size() * 2; // Sequential and Parallel
        int currentStep = 0;

        for (size_t size : sizesToTest) {
            std::vector<uint8_t> data(size, 0xAA);
            size_t threads = threadSpinner->value();
            if (threads == 0) threads = std::thread::hardware_concurrency();

            auto seq = EncryptorFactory::create(EncryptorType::Sequential);
            auto par = EncryptorFactory::create(EncryptorType::Parallel, threads);

            QString sizeStr = size >= (1024 * 1024) ? QString("%1 MB").arg(size / (1024 * 1024)) : QString("%1 KB").arg(size / 1024);

            logOutput->append(QString("Benchmarking %1 (Sequential)...").arg(sizeStr));
            QApplication::processEvents();
            
            auto startSeq = std::chrono::high_resolution_clock::now();
            seq->encrypt(data, key);
            auto endSeq = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> durSeq = endSeq - startSeq;
            
            BenchmarkResult resSeq{"Sequential", size, 1, durSeq.count()};
            results.push_back(resSeq);
            logOutput->append(QString("  -> Time: %1 ms").arg(resSeq.executionTimeMs));
            
            currentStep++;
            progressBar->setValue((currentStep * 100) / totalSteps);
            QApplication::processEvents();

            logOutput->append(QString("Benchmarking %1 (Parallel, %2 threads)...").arg(sizeStr).arg(threads));
            QApplication::processEvents();
            
            auto startPar = std::chrono::high_resolution_clock::now();
            par->encrypt(data, key);
            auto endPar = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> durPar = endPar - startPar;

            BenchmarkResult resPar{"Parallel", size, threads, durPar.count()};
            results.push_back(resPar);
            logOutput->append(QString("  -> Time: %1 ms").arg(resPar.executionTimeMs));
            
            currentStep++;
            progressBar->setValue((currentStep * 100) / totalSteps);
            QApplication::processEvents();
        }

        QString outFilePath = QDir::currentPath() + "/benchmark_results.txt";
        Benchmark::saveResultsToFile(results, outFilePath.toStdString());
        
        logOutput->append(QString("\nBenchmark finished. Results saved to: %1\n").arg(outFilePath));
        progressBar->setValue(100);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        logOutput->append(QString("Error: %1\n").arg(e.what()));
    }
}
