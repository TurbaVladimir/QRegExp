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
	if ((ui->regexp->text().isEmpty()) || (ui->testText->document()->isEmpty()))
	{
		return;
	}

	QTime timer;

	if (ui->regexp->text().size() == 0)
	{
		ui->statusBar->showMessage("Eneter regexp", 5000);
		return;
	}
	if (ui->testText->toPlainText().size() == 0)
	{
		ui->statusBar->showMessage("Enter testing text", 5000);
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
		ui->statusBar->showMessage(QString("%1 at %2 symbol").arg(exp.errorString()).arg(exp.patternErrorOffset()), 5000);
		ui->regexp->setFocus(Qt::OtherFocusReason);
		ui->regexp->setCursorPosition(exp.patternErrorOffset() - 1);
		return;
	}

	timer.start();
	QRegularExpressionMatch match = exp.match(ui->testText->toPlainText());
	long elapsed = timer.elapsed();

	ui->resultText->clear();
	for (int i = 0; i < match.capturedTexts().size(); i++)
	{
		QString string = QString("[%1][%2-%3] '%4'\n").
				arg(i).
				arg(match.capturedStart(i)).
				arg(match.capturedEnd(i)).
				arg(match.captured(i));

		ui->resultText->setText(QString("%1%2").arg(ui->resultText->toPlainText()).arg(string));
	}

	ui->statusBar->showMessage(QString("Captured %1 texts in %2 msec").arg(match.capturedTexts().size()).arg(elapsed), 5000);
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
