#pragma once

#include <QAbstractScrollArea>

class MemoryView : public QAbstractScrollArea
{
	Q_OBJECT

public:
	MemoryView(QWidget *parent);
	~MemoryView();
	void setBuffer(unsigned char* buffer, int size);
	void update();
protected:
	void paintEvent(QPaintEvent*);
	void resizeEvent(QResizeEvent*);
private:
	void adjustContent();
	int fontCharacterWidth;
	int fontCharacterHeight;
	int startVisibleLine;
	int endVisibleLine;
	const int BytesPerLine = 16;
	unsigned char* buffer;
	int bufferSize;
};
