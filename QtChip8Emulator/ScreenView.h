#pragma once
#include <QTime>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>

class ScreenView : public QWidget
{
	Q_OBJECT
private:
    const static int chip8ScreenWidth = 64;
    const static int chip8ScreenHeight = 32;
    QImage* image;
    uchar screenBuffer[chip8ScreenWidth * chip8ScreenHeight * 4];
    QTimer* timer;
    QTime time;
public slots:
    void doPaint();
public:
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual int heightForWidth(int w) const;
    virtual void paintEvent(QPaintEvent* e);
    void setScreenBuffer(char* buffer, int bufferSize);
	ScreenView(QWidget* parent);
	~ScreenView();
};
