#pragma once

#include <QWidget>
#include "ui_AboutForm.h"

class AboutForm : public QWidget
{
	Q_OBJECT

public:
	AboutForm(QWidget *parent = Q_NULLPTR);
	~AboutForm();

private:
	Ui::AboutForm ui;
};
