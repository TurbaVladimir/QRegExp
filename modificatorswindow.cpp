#include "modificatorswindow.h"

ModificatorsWindow::ModificatorsWindow() : ui(new Ui_modificatorsWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::SubWindow | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
}

ModificatorsWindow::~ModificatorsWindow()
{
	delete ui;
}

