#include <QFile>
#include <QList>

#include "storagewindow.h"

StorageWindow::StorageWindow() : ui(new Ui_storageWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::SubWindow | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

	connect(ui->loadButton, &QPushButton::clicked, this, &StorageWindow::loadRegExp);
	connect(ui->deleteButton, &QPushButton::clicked, this, &StorageWindow::deleteRegExp);
}

StorageWindow::~StorageWindow()
{
	delete ui;
}

void StorageWindow::fillRegExpList()
{
	ui->regexpList->clear();

	QFile file("STORAGE_FILENAME");
	file.open(QFile::ReadWrite);
	if (!file.isOpen())
	{
		emit statusBarText(file.errorString(), 3000);
		return;
	}

	while (!file.atEnd())
	{
		QString line = QString(file.readLine());
		line.resize(line.size() - 1);
		ui->regexpList->addItem(line);
	}

	file.close();
}

void StorageWindow::loadRegExp()
{
	if (ui->regexpList->count() <= 0)
	{
		return;
	}

	emit regExp(ui->regexpList->currentItem()->text());
	close();
}

void StorageWindow::deleteRegExp()
{
	if (ui->regexpList->count() <= 0)
	{
		return;
	}

	QFile file("STORAGE_FILENAME");
	file.open(QFile::ReadWrite);
	if (!file.isOpen())
	{
		emit statusBarText(file.errorString(), 3000);
		return;
	}

	QList<QString> list;

	while (!file.atEnd())
	{
		QString line = QString(file.readLine());
		line.resize(line.size() - 1);
		list.append(line);
	}

	file.resize(0);

	for (int i = 0; i < list.size(); i++)
	{
		if (list.at(i) == ui->regexpList->currentItem()->text())
		{
			list.removeAt(i);
		}
	}
	for (int i = 0; i < list.size(); i++)
	{
		file.write(list.at(i).toUtf8());
		file.write("\n");
	}

	file.close();
	delete ui->regexpList->currentItem();
	statusBarText("RegExp deleted", 3000);
}

