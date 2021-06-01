#include "MemoryView.h"
#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QScrollBar>

MemoryView::MemoryView(QWidget *parent)
	: QAbstractScrollArea(parent)
{
	connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &MemoryView::adjustContent);
	connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &MemoryView::adjustContent);
	fontCharacterWidth = fontMetrics().width(QChar('0'));
	fontCharacterHeight = fontMetrics().height();
	bufferSize = 0;
}

MemoryView::~MemoryView()
{
}
void MemoryView::update() {
	adjustContent();
}
void MemoryView::setBuffer(unsigned char* buffer, int size) {
	this->buffer = buffer;
	this->bufferSize = size;
}
void MemoryView::resizeEvent(QResizeEvent* resizeEvent) {
	adjustContent();
}
void MemoryView::paintEvent(QPaintEvent* paintEvent) {
	QPainter painter(viewport());
	painter.setPen(viewport()->palette().color(QPalette::WindowText));
	int offsetX = horizontalScrollBar()->value() + 2;
	if (bufferSize == 0) {
		return;
	}
	int addressWidth = fontCharacterWidth * 5;
	char strbuf[100];
	int y = fontCharacterHeight;
	for (int i = startVisibleLine; i < endVisibleLine; i++) {		
		if (i % 2) {
			painter.fillRect(offsetX, y - fontCharacterHeight + 3, fontCharacterWidth * 60, fontCharacterHeight - 1, QBrush(QColor(240, 240, 240)));
		}
		snprintf(strbuf, 100, "%04X", i * BytesPerLine);		
		painter.drawText(offsetX, y, strbuf);	
		for (int j = 0; j < 16; j++) {
			if ((j + i * BytesPerLine) < bufferSize) {				
				snprintf(strbuf, 100, "%02X", (char)buffer[j + i * BytesPerLine] & 0xff);										
				painter.drawText(offsetX + addressWidth + j * (fontCharacterWidth + 3) * 2, y, strbuf);
				
			}
		}
		y += fontCharacterHeight;
	}	
	painter.drawLine(addressWidth - 2, 0, addressWidth - 2, viewport()->height());	
}
void MemoryView::adjustContent() {
	int rowsVisible = viewport()->height() / fontCharacterHeight;
	startVisibleLine = verticalScrollBar()->value();
	endVisibleLine = startVisibleLine + rowsVisible;
	verticalScrollBar()->setRange(0, bufferSize / BytesPerLine - rowsVisible);
	verticalScrollBar()->setPageStep(rowsVisible);
	viewport()->update();
}