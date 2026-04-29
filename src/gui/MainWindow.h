#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QProgressBar>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onEncryptFileClicked();
    void onDecryptFileClicked();
    void onRunBenchmarkClicked();

private:
    void setupUi();
    std::vector<uint8_t> getKeyFromInput();
    void processFile(bool isEncrypt);

    // UI Elements
    QLineEdit* keyInput;
    QComboBox* modeSelector;
    QSpinBox* threadSpinner;
    
    QPushButton* btnEncryptFile;
    QPushButton* btnDecryptFile;
    QPushButton* btnRunBenchmark;

    QTextEdit* logOutput;
    QProgressBar* progressBar;
};
