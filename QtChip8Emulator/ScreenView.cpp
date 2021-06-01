#include "ScreenView.h"
#include <Windows.h>

ScreenView::ScreenView(QWidget* parent) : QWidget(parent) {

    for (int cnt = 0, a, r, g, b; cnt < chip8ScreenWidth * chip8ScreenHeight * 4;) {
        a = 255; //alpha
        r = 0;   //red
        g = 0;   //green
        b = 0;   //blue

        screenBuffer[cnt] = b; cnt++;
        screenBuffer[cnt] = g; cnt++;
        screenBuffer[cnt] = r; cnt++;
        screenBuffer[cnt] = a; cnt++;
    }
    image = new QImage(screenBuffer, chip8ScreenWidth, chip8ScreenHeight, QImage::Format_ARGB32);
   

    // connect QTimer.timeout to my doPaint method
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(doPaint()));
    timer->start(1);
    QSizePolicy policy(sizePolicy());
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
};
ScreenView::~ScreenView()
{
}
void ScreenView::setScreenBuffer(char* buffer, int bufferSize) {
    memcpy(screenBuffer, buffer, bufferSize);
}
 QSize ScreenView::sizeHint() const {
    return QSize(64, 32);
}
 QSize ScreenView::minimumSizeHint() const  {
    return QSize(64, 32);
}
 int ScreenView::heightForWidth(int w) const {
     return w / 2;
 }
 void ScreenView::paintEvent(QPaintEvent* e)
 {
     QPainter p(this);
     p.drawImage(rect(), *image);     
}
void ScreenView::doPaint() {
    update();
};
