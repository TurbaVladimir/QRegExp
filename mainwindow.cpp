#include <QTime>
#include <QFile>
#include <QList>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	storageWindow = new QWidget;
	storageUi = new Ui_storageWindow;
	storageUi->setupUi(storageWindow);

	connect(ui->regexp, &QLineEdit::textChanged, this, &MainWindow::check);
	connect(ui->caseInsensitive, &QCheckBox::clicked, this, &MainWindow::check);
	connect(ui->dotMatch, &QCheckBox::clicked, this, &MainWindow::check);
	connect(ui->multiLine, &QCheckBox::clicked, this, &MainWindow::check);
	connect(ui->testText, &QTextEdit::textChanged, this, &MainWindow::check);
	connect(ui->storageButton, &QPushButton::clicked, this, &MainWindow::openStorage);
	connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveRegExp);
	connect(ui->captures, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::loadCapture);
	connect(storageUi->loadButton, &QPushButton::clicked, this, &MainWindow::loadRegExp);
	connect(storageUi->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteRegExp);
}

MainWindow::~MainWindow()
{
	delete ui;
	delete storageUi;
	storageWindow->deleteLater();
}

void MainWindow::check()
{
	if (ui->regexp->text().isEmpty())
	{
		ui->regexp->setStyleSheet("");
		return;
	}

	QRegularExpression exp(ui->regexp->text());
	if (ui->multiLine->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::MultilineOption);
	}
	if (ui->caseInsensitive->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}
	if (ui->dotMatch->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
	}
	if (!exp.isValid())
	{
		ui->regexp->setStyleSheet("QLineEdit{background: red;}");

		ui->statusBar->showMessage(QString("%1 at %2 symbol").arg(exp.errorString()).arg(exp.patternErrorOffset()), 5000);
		ui->regexp->setFocus(Qt::OtherFocusReason);
		return;
	}
	else
	{
		ui->regexp->setStyleSheet("QLineEdit{background: green;}");
	}

	if (ui->testText->toPlainText().isEmpty())
	{
		return;
	}

	QTime timer;
	timer.start();
	QRegularExpressionMatch match = exp.match(ui->testText->toPlainText());
	int elapsed = timer.elapsed();

	int captureIndex = ui->captures->currentIndex();
	ui->resultText->clear();
	captures.clear();
	ui->captures->clear();
	for (int i = 0; i < match.capturedTexts().size(); i++)
	{
		ui->captures->addItem(QString("[%1][%2-%3]").
							  arg(i).
							  arg(match.capturedStart(i)).
							  arg(match.capturedEnd(i)));
		captures.append(match.captured(i));
	}

	if ((captures.size() > captureIndex) && (captureIndex >= 0))
	{
		ui->captures->setCurrentIndex(captureIndex);
		ui->resultText->setText(captures.at(captureIndex));
	}
	else
	{
		if (match.hasMatch())
		{
			ui->resultText->setText(captures.at(0));
		}
	}
	ui->statusBar->showMessage(QString("Captured %1 texts in %2 msec").arg(match.capturedTexts().size()).arg(elapsed), 5000);
}

void MainWindow::loadCapture(int index)
{
	if (!captures.isEmpty())
	{
		if (index < 0)
		{
			ui->captures->setCurrentIndex(0);
			index = 0;
		}

		ui->resultText->setText(captures.at(index));
	}
}

void MainWindow::saveRegExp()
{
	if (ui->regexp->text().isEmpty())
	{
		return;
	}

	QFile file("storage");
	file.open(QFile::ReadWrite);
	if (!file.isOpen())
	{
		ui->statusBar->showMessage(file.errorString(), 3000);
		return;
	}

	while (!file.atEnd())
	{
		QString line = QString(file.readLine());
		line.resize(line.size() - 1);
		if (line == ui->regexp->text())
		{
			file.close();
			ui->statusBar->showMessage("RegExp already saved", 3000);
			return;
		}
	}

	file.write(ui->regexp->text().toUtf8());
	file.write("\n");
	file.close();
	ui->statusBar->showMessage("RegExp saved", 3000);
}

void MainWindow::openStorage()
{
	storageUi->regexpList->clear();

	QFile file("storage");
	file.open(QFile::ReadWrite);
	if (!file.isOpen())
	{
		ui->statusBar->showMessage(file.errorString(), 3000);
		return;
	}

	while (!file.atEnd())
	{
		QString line = QString(file.readLine());
		line.resize(line.size() - 1);
		storageUi->regexpList->addItem(line);
	}

	file.close();
	storageWindow->show();
}

void MainWindow::loadRegExp()
{
	ui->regexp->setText(storageUi->regexpList->currentItem()->text());
	storageWindow->close();
	check();
}

void MainWindow::deleteRegExp()
{
	QFile file("storage");
	file.open(QFile::ReadWrite);
	if (!file.isOpen())
	{
		ui->statusBar->showMessage(file.errorString(), 3000);
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
		if (list.at(i) == storageUi->regexpList->currentItem()->text())
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
	delete storageUi->regexpList->currentItem();
	ui->statusBar->showMessage("RegExp deleted", 3000);
}

void MainWindow::closeEvent(QCloseEvent*)
{
	storageWindow->close();
}
