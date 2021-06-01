#include "QtChip8Emulator.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtChip8Emulator w;
    w.show();
    return a.exec();
}
