#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>
#include <filesystem>
#include <chrono>
#include <QPainter>
#include <QThread>
#include <QTimer>

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Устанавливаем моноширинный шрифт, чтобы колонки были ровными
    ui->textBrowser->setFontFamily("Courier New");
    QTimer::singleShot(500, this, &MainWindow::runTests);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runTests()
{
    ui->textBrowser->clear();

    // --- ТЕСТ 1 (Calc) ---
    auto start1 = std::chrono::high_resolution_clock::now();
    volatile double result = 0;
    for (int i = 0; i <= 8000000; ++i) {
        result = std::pow(0.5, i);
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = end1 - start1;

    // --- ТЕСТ 2 (Write) ---
    auto start2 = std::chrono::high_resolution_clock::now();
    std::string src = "video.mp4";
    std::string dst = "video_copy.mp4";
    double time2 = 0;

    if (fs::exists(src)) {
        try {
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
            auto end2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed2 = end2 - start2;
            time2 = elapsed2.count();
        } catch (fs::filesystem_error& e) {
            ui->textBrowser->append("Ошибка Copy: " + QString::fromStdString(e.what()));
        }
    }

    // --- ТЕСТ 3 (Draw) ---
    showCircles = true;
    doLatencyTest = true;
    auto start3 = std::chrono::high_resolution_clock::now();
    repaint();
    auto end3 = std::chrono::high_resolution_clock::now();
    doLatencyTest = false;
    std::chrono::duration<double> elapsed3 = end3 - start3;

    // --- ВЫЧИСЛЕНИЯ ПАКТ ---
    // Сырое среднее
    double geomMeanRaw = std::pow(elapsed1.count() * time2 * elapsed3.count(), 1.0/3.0);

    // Нормированные значения (относительно ПАК2: ПАКТ / ПАК2)
    // Данные ПАК2: 0.6612, 15.8437, 0.5027
    double ratio1 = elapsed1.count() / 0.6612;
    double ratio2 = time2 / 15.8437;
    double ratio3 = elapsed3.count() / 0.5027;
    double geomMeanRatio = std::pow(ratio1 * ratio2 * ratio3, 1.0/3.0);

    // Шаблон для ровной строки
    QString rowFormat = "%1 | %2 | %3 | %4 | %5";


    // ==========================================================
    // --- ВЫВОД ПЕРВОЙ ТАБЛИЦЫ (Сырые данные) ---
    // ==========================================================
    ui->textBrowser->append("=== Сырые данные ===");

    // Заголовки
    ui->textBrowser->append(rowFormat.arg("", -8).arg("Calc", -14).arg("Write", -14).arg("Draw", -14).arg("I", -10));
    ui->textBrowser->append("----------------------------------------------------------------------");

    // Данные ПАК1
    ui->textBrowser->append(rowFormat.arg("ПАК1", -8)
                                .arg("0.3752 сек", -14)
                                .arg("0.3437 сек", -14)
                                .arg("0.1542 сек", -14)
                                .arg("≈0.2709", -10));
    // Данные ПАК2 (ОБНОВЛЕНО: Write = 15.8437, I = 1.740)
    ui->textBrowser->append(rowFormat.arg("ПАК2", -8)
                                .arg("0.6612 сек", -14)
                                .arg("15.8437 сек", -14)
                                .arg("0.5027 сек", -14)
                                .arg("≈1.740", -10));
    // Данные ПАКТ
    ui->textBrowser->append(rowFormat.arg("ПАКТ", -8)
                                .arg(QString::number(elapsed1.count(), 'f', 4) + " сек", -14)
                                .arg(QString::number(time2, 'f', 4) + " сек", -14)
                                .arg(QString::number(elapsed3.count(), 'f', 4) + " сек", -14)
                                .arg(QString::number(geomMeanRaw, 'f', 4), -10));

    ui->textBrowser->append(""); // Пустая строка


    // ==========================================================
    // --- ВЫВОД ВТОРОЙ ТАБЛИЦЫ (Нормированные данные) ---
    // ==========================================================
    ui->textBrowser->append("=== Нормированные данные (Относительно ПАК2) ===");

    // Заголовки
    ui->textBrowser->append(rowFormat.arg("", -8).arg("Calc", -12).arg("Write", -12).arg("Draw", -12).arg("I", -10));
    ui->textBrowser->append("-----------------------------------------------------------");

    // Данные ПАК1 (ОБНОВЛЕНО: пересчитаны Write и I из-за изменения базового значения ПАК2)
    // Calc: 0.3752 / 0.6612 = 0.567453
    // Write: 0.3437 / 15.8437 = 0.021693
    // Draw: 0.1542 / 0.5027 = 0.306744
    // I: (0.567453 * 0.021693 * 0.306744) ^ (1/3) = 0.155708
    ui->textBrowser->append(rowFormat.arg("ПАК1", -8)
                                .arg(QString::number(0.567453, 'f', 6), -12)
                                .arg(QString::number(0.021693, 'f', 6), -12)
                                .arg(QString::number(0.306744, 'f', 6), -12)
                                .arg(QString::number(0.155708, 'f', 6), -10));
    // Данные ПАК2 (Наш эталон)
    ui->textBrowser->append(rowFormat.arg("ПАК2", -8)
                                .arg("1", -12)
                                .arg("1", -12)
                                .arg("1", -12)
                                .arg("1", -10));
    // Данные ПАКТ
    ui->textBrowser->append(rowFormat.arg("ПАКТ", -8)
                                .arg(QString::number(ratio1, 'f', 6), -12)
                                .arg(QString::number(ratio2, 'f', 6), -12)
                                .arg(QString::number(ratio3, 'f', 6), -12)
                                .arg(QString::number(geomMeanRatio, 'f', 6), -10));
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    if (!showCircles) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(0, 255, 0));
    painter.drawEllipse(400, 50, 140, 140);
    painter.drawEllipse(460, 50, 110, 110);
    painter.drawEllipse(400, 110, 80, 80);
    painter.drawEllipse(460, 110, 50, 50);
    if (doLatencyTest) {
        QThread::msleep(200);
    }
}
