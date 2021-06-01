#include "DebuggerForm.h"
#include "MemoryView.h"
#include <QLayout>
#include <QListWidget>
#include <QList>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <sstream>
#include <iomanip>
#include "Chip8Cpu.h"
#include "Chip8Disassembler.h"

DebuggerForm::DebuggerForm(QWidget *parent, std::shared_ptr<Chip8Cpu> cpu)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowTitle("Debugger");

	memoryViewer = new MemoryView(this);	
	toolbar = new QToolBar(this);
	toolbar->setMovable(true);
	stackListView = new QListWidget(this);
	stackListView->setMinimumHeight(50);
	stackListView->setMinimumWidth(250);
	QGroupBox* registersGroupBox = new QGroupBox(this);	
	QVBoxLayout* cpuRegistersLayout = new QVBoxLayout(this);
	QHBoxLayout* registerLabelsLayout = new QHBoxLayout(this);
	QHBoxLayout* registersLayout = new QHBoxLayout(this);
	QHBoxLayout* indexLabelLayout = new QHBoxLayout(this);
	QHBoxLayout* indexRegisterLayout = new QHBoxLayout(this);	
	registersGroupBox->setLayout(cpuRegistersLayout);
	cpuRegistersLayout->addLayout(registerLabelsLayout);
	cpuRegistersLayout->addLayout(registersLayout);
	cpuRegistersLayout->addLayout(indexLabelLayout);
	cpuRegistersLayout->addLayout(indexRegisterLayout);
	cpuRegistersLayout->addStretch();
	
	for (int i = 0; i < 16; i++) {		
		QLineEdit* lineEdit = new QLineEdit(this);
		lineEdit->setFixedWidth(20);
		lineEdit->setReadOnly(true);
		QLabel* label = new QLabel(this);
		std::stringstream ss;
		ss << std::hex << std::uppercase << i;
		label->setText(ss.str().c_str());
		registerBoxes.push_back(lineEdit);
		registerLabelsLayout->addWidget(label);
		registersLayout->addWidget(lineEdit);
	}	
	indexBox = new QLineEdit(this);
	indexBox->setFixedWidth(30);
	indexBox->setReadOnly(true);
	QLabel* indexLabel = new QLabel(this);
	indexLabel->setText("I");
	indexLabelLayout->addWidget(indexLabel);
	indexRegisterLayout->addWidget(indexBox);
	indexRegisterLayout->addStretch();
	disassemblyList = new QTreeWidget(this);
	disassemblyList->setColumnCount(4);
	QStringList headerStringList;
	headerStringList.append(" ");
	headerStringList.append("Adress");
	headerStringList.append("Bytes");
	headerStringList.append("Disassembly");
	disassemblyList->setHeaderLabels(headerStringList);	

	QLabel* memoryLabel = new QLabel("Memory", this);
	QLabel* disassemblyLabel = new QLabel("Disassembly", this);
	QLabel* registersLabel = new QLabel("Registers", this);
	QLabel* stackLabel = new QLabel("Stack", this);
	ui.gridLayout->addWidget(memoryLabel, 1, 0);
	ui.gridLayout->addWidget(disassemblyLabel, 1, 1);
	ui.gridLayout->addWidget(toolbar, 0, 0, 1, 2);
	ui.gridLayout->addWidget(memoryViewer,2,0);
	ui.gridLayout->addWidget(disassemblyList, 2, 1);
	ui.gridLayout->addWidget(registersLabel, 3, 0);
	ui.gridLayout->addWidget(stackLabel, 3, 1);
	ui.gridLayout->addWidget(registersGroupBox, 4, 0);
	ui.gridLayout->addWidget(stackListView, 4, 1);

	QAction* reset = toolbar->addAction("Reset");	
	QAction* step = toolbar->addAction("Step");
	connect(reset, &QAction::triggered, this, &DebuggerForm::reset);	
	connect(step, &QAction::triggered, this, &DebuggerForm::step);

	this->cpu = cpu;
	createDisassembly();
	memoryViewer->setBuffer(cpu->GetMemory(), cpu->MemorySize);
	updateDebugFields(0x200);
}
void DebuggerForm::reset() {
	auto prevPC = cpu->PC;	
	cpu->ResetAndReloadCurrentRom();
	updateDebugFields(prevPC);
}
void DebuggerForm::step() {
	auto prevPC = cpu->PC;
	cpu->Run(1);
	cpu->GetScreenBGRABuffer();
	updateDebugFields(prevPC);
}
void DebuggerForm::updateDebugFields(unsigned int prevPC) {
	for (int i = 0; i < 16; i++) {
		std::stringstream ss;
		ss << std::hex << std::uppercase << (unsigned short)cpu->registers[i];
		registerBoxes[i]->setText(ss.str().c_str());
	}
	std::stringstream iss;
	iss << std::hex << std::uppercase << (unsigned short)cpu->I;
	indexBox->setText(iss.str().c_str());
	updateStepCursor(prevPC);
	memoryViewer->update();
	stackListView->clear();
	for (auto& stackAdress : cpu->stack) {
		std::stringstream ss;
		ss << std::hex << std::uppercase << (unsigned short)stackAdress;
		stackListView->addItem(new QListWidgetItem(ss.str().c_str(), stackListView));
	}
}
void DebuggerForm::updateStepCursor(unsigned int prevPC)
{	
	addressItemMap[prevPC]->setText(0, " ");
	addressItemMap[prevPC]->setBackgroundColor(0, QColor(255, 255, 255));
	addressItemMap[prevPC]->setBackgroundColor(1, QColor(255, 255, 255));
	addressItemMap[prevPC]->setBackgroundColor(2, QColor(255, 255, 255));
	addressItemMap[prevPC]->setBackgroundColor(3, QColor(255, 255, 255));

	addressItemMap[cpu->PC]->setText(0, ">>");
	addressItemMap[cpu->PC]->setBackgroundColor(0, QColor(255, 255, 170));
	addressItemMap[cpu->PC]->setBackgroundColor(1, QColor(255, 255, 170));
	addressItemMap[cpu->PC]->setBackgroundColor(2, QColor(255, 255, 170));
	addressItemMap[cpu->PC]->setBackgroundColor(3, QColor(255, 255, 170));
	disassemblyList->scrollToItem(addressItemMap[cpu->PC], QAbstractItemView::ScrollHint::EnsureVisible);
}
void DebuggerForm::createDisassembly() {
	disassembler = new Chip8Disassembler();
	auto disassembly = disassembler->Disassemble(cpu->GetMemory(), cpu->MemorySize);
	QList<QTreeWidgetItem*> items;
	QModelIndex startIndex;
	QTreeWidgetItem* startItem = NULL;
	for (auto line : disassembly) {
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::setw(4) << line.address;
		QStringList stringList;
		if (line.address == 0x200) {
			stringList.append(">>");
		}
		else {
			stringList.append(" ");
		}
		stringList.append(ss.str().c_str());
		stringList.append(line.byteString.c_str());
		stringList.append(line.disassembly.c_str());

		auto item = new QTreeWidgetItem((QTreeWidgetItem*)NULL, stringList);		
		if (line.address == 0x200) {
			startItem = item;
		}
		addressItemMap[line.address]=item;
		items.append(item);
	}
	disassemblyList->insertTopLevelItems(0, items);
	for (int i = 0; i < 3; i++) {
		disassemblyList->resizeColumnToContents(i);
	}
	if (startItem != NULL) {
		//disassemblyList->setCurrentItem(startItem);
		startItem->setBackgroundColor(0, QColor(255, 255, 170));
		disassemblyList->scrollToItem(startItem, QAbstractItemView::ScrollHint::PositionAtTop);
	}
}
DebuggerForm::~DebuggerForm()
{
}
