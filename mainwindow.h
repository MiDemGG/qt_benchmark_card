#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void runTests();

private:
    Ui::MainWindow *ui;

    // Флаг 1: Просто рисовать круги (чтобы они висели на экране)
    bool showCircles = false;

    // Флаг 2: Включить тормоза (нужен только на момент замера времени)
    bool doLatencyTest = false;
};

#endif // MAINWINDOW_H
