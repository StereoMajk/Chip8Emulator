#pragma once

#include <QWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QLineEdit>
#include "ui_DebuggerForm.h"
#include "MemoryView.h"
#include "Chip8Cpu.h"
#include "Chip8Disassembler.h"
class DebuggerForm : public QWidget
{
	Q_OBJECT

public:
	DebuggerForm(QWidget *parent, std::shared_ptr<Chip8Cpu> cpu);
	void reset();
	void step();		
	void updateDebugFields(unsigned int prevPC);

	~DebuggerForm();

private:
	void updateStepCursor(unsigned int prevPC);
	void createDisassembly();
	Ui::DebuggerForm ui;
	MemoryView* memoryViewer;
	QTreeWidget* disassemblyList;
	QToolBar* toolbar;
	std::vector<QLineEdit*> registerBoxes;
	QLineEdit* indexBox;
	QListWidget* stackListView;
	std::map<unsigned short, QTreeWidgetItem*> addressItemMap;
	Chip8Disassembler* disassembler;
	std::shared_ptr<Chip8Cpu> cpu;
};
