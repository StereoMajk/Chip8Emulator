#pragma once

#include <QtWidgets/QMainWindow>
#include <QVBoxLayout>
#include "ui_QtChip8Emulator.h"
#include <QTimer>
#include "ScreenView.h"
#include "Chip8Cpu.h"
#include "MemoryView.h"

class QtChip8Emulator : public QMainWindow
{
    Q_OBJECT

public:
    QtChip8Emulator(QWidget *parent = Q_NULLPTR);
public slots:
    void openFile();
    void openDebugger();
    void openAbout();
    void run();
    void runCpu();
    void refreshScreen();
    void quit();
private:
    bool eventFilter(QObject* Object, QEvent* Event);
    Ui::QtChip8EmulatorClass ui;
    ScreenView* screenImage;    
    std::shared_ptr<QTimer> screenRefreshTimer;
    std::shared_ptr<QTimer> cpuTimer;
    std::shared_ptr<Chip8Cpu> cpu;
    std::string currentLoadedFilename;
};
