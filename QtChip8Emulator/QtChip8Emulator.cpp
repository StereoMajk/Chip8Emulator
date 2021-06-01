#include "QtChip8Emulator.h"
#include <QLayout>
#include <QFileDialog>
#include <QTimer>
#include <QKeyEvent>
#include <sstream>
#include "Chip8Cpu.h"
#include "DebuggerForm.h"
QtChip8Emulator::QtChip8Emulator(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);        
    this->setWindowTitle("Chip-8 Emulator");
    screenImage = new ScreenView(this);

    cpu = std::make_shared<Chip8Cpu>();
    screenRefreshTimer = std::make_shared<QTimer>();
    cpuTimer = std::make_shared<QTimer>();
    ui.gridLayout->addWidget(screenImage,0,0);
    QCoreApplication::instance()->installEventFilter(this);
    connect(screenRefreshTimer.get(), &QTimer::timeout, this, &QtChip8Emulator::refreshScreen);
    connect(cpuTimer.get(), &QTimer::timeout, this, &QtChip8Emulator::runCpu);
    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui.actionOpenDebugger, SIGNAL(triggered()), this, SLOT(openDebugger()));
    connect(ui.actionRun, SIGNAL(triggered()), this, SLOT(run()));
    connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    screenRefreshTimer->start(1);
    
}
void QtChip8Emulator::quit() {
    QApplication::quit();
}
void QtChip8Emulator::openDebugger() {
    DebuggerForm* form = new DebuggerForm(this, cpu);
    form->setWindowFlags(Qt::Window);
    form->show();
    cpuTimer->stop();
}
void QtChip8Emulator::runCpu() {
    cpu->Run(1);
}
void QtChip8Emulator::run() {
    cpuTimer->start(2);
    std::stringstream ss;
    ss << "Running " << currentLoadedFilename;
    ui.statusBar->showMessage(ss.str().c_str());
}
void QtChip8Emulator::refreshScreen() {
    screenImage->setScreenBuffer(cpu->GetScreenBGRABuffer(), cpu->GetScreenBGRABufferSize());
}
void QtChip8Emulator::openFile() {
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Chip8 ROM"), R"(D:\Projects\QtChip8Emulator\data)", "*.ch8");
    if (fileName.length() > 0) {
        currentLoadedFilename = fileName.toStdString();
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        auto byteArray = file.readAll();
        std::vector<unsigned char> rom;
        rom.assign(byteArray.data(), byteArray.data() + byteArray.size());
        cpu->LoadRom(rom);
        screenImage->setScreenBuffer(cpu->GetScreenBGRABuffer(), cpu->GetScreenBGRABufferSize());
        std::stringstream ss;
        ss << "Loaded ROM " << fileName.toStdString();
        
        ui.statusBar->showMessage(ss.str().c_str());
        run();
    }
}
bool QtChip8Emulator::eventFilter(QObject* Object, QEvent* Event)
{
    if (Event->type() == QEvent::KeyPress || Event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* KeyEvent = (QKeyEvent*)Event;
        bool down = false;
        if (Event->type() == QEvent::KeyPress) {
            down = true;
        }

        switch (KeyEvent->key())
        {
        case Qt::Key_1:
            cpu->SetKey(1, down);
            break;
        case Qt::Key_2:
            cpu->SetKey(2, down);
            break;
        case Qt::Key_3:
            cpu->SetKey(3, down);
            break;
        case Qt::Key_4:
            cpu->SetKey(12, down);
            break;
        case Qt::Key_Q:
            cpu->SetKey(4, down);
            break;
        case Qt::Key_W:
            cpu->SetKey(5, down);
            break;
        case Qt::Key_E:
            cpu->SetKey(6, down);
            break;
        case Qt::Key_R:
            cpu->SetKey(13, down);
            break;
        case Qt::Key_A:
            cpu->SetKey(7, down);
            break;
        case Qt::Key_S:
            cpu->SetKey(8, down);
            break;
        case Qt::Key_D:
            cpu->SetKey(9, down);
            break;
        case Qt::Key_F:
            cpu->SetKey(14, down);
            break;
        case Qt::Key_Z:
            cpu->SetKey(10, down);
            break;
        case Qt::Key_X:
            cpu->SetKey(0, down);
            break;
        case Qt::Key_C:
            cpu->SetKey(11, down);
            break;
        case Qt::Key_V:
            cpu->SetKey(15, down);
            break;
        default:
            break;
        }
    }
    return false;
}