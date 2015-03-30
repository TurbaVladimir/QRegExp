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

	modificatorsWindow = new ModificatorsWindow;
	storageWindow = new StorageWindow;

	connect(modificatorsWindow->ui->caseInsensitive, &QCheckBox::clicked, this, &MainWindow::check);
	connect(modificatorsWindow->ui->DontCapture, &QCheckBox::clicked, this, &MainWindow::check);
	connect(modificatorsWindow->ui->dotMatch, &QCheckBox::clicked, this, &MainWindow::check);
	connect(modificatorsWindow->ui->extendPattern, &QCheckBox::clicked, this, &MainWindow::check);
	connect(modificatorsWindow->ui->invertedGreediness, &QCheckBox::clicked, this, &MainWindow::check);
	connect(modificatorsWindow->ui->multiLine, &QCheckBox::clicked, this, &MainWindow::check);
	connect(modificatorsWindow->ui->useUnicode, &QCheckBox::clicked, this, &MainWindow::check);

	connect(ui->regexp, &QLineEdit::textChanged, this, &MainWindow::check);
	connect(ui->testText, &QTextEdit::textChanged, this, &MainWindow::check);
	connect(ui->storageButton, &QPushButton::clicked, this, &MainWindow::switchStorageWindow);
	connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveRegExp);
	connect(ui->modificatorsButton, &QPushButton::clicked, this, &MainWindow::switchModificatorsWindow);
	connect(ui->captures, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::switchCaptureGroup);

	connect(storageWindow, &StorageWindow::regExp, this, &MainWindow::setRegExp);
	connect(storageWindow, &StorageWindow::regExp, this, &MainWindow::check);
	connect(storageWindow, &StorageWindow::statusBarText, this, &MainWindow::setStatusBarText);
	connect(ui->saveButton, &QPushButton::clicked, storageWindow, &StorageWindow::fillRegExpList);
}

MainWindow::~MainWindow()
{
	delete ui;
	storageWindow->deleteLater();
	modificatorsWindow->deleteLater();
}

void MainWindow::check()
{
	if (ui->regexp->text().isEmpty())
	{
		ui->regexp->setStyleSheet("");
		return;
	}

	QRegularExpression exp(ui->regexp->text());

	if (modificatorsWindow->ui->caseInsensitive->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}
	if (modificatorsWindow->ui->DontCapture->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::DontCaptureOption);
	}
	if (modificatorsWindow->ui->dotMatch->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
	}
	if (modificatorsWindow->ui->extendPattern->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}
	if (modificatorsWindow->ui->invertedGreediness->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
	}
	if (modificatorsWindow->ui->multiLine->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::MultilineOption);
	}
	if (modificatorsWindow->ui->useUnicode->isChecked())
	{
		exp.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
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

void MainWindow::setStatusBarText(QString text, int time = 0)
{
	ui->statusBar->showMessage(text, time);
}

void MainWindow::setRegExp(QString regexp)
{
	ui->regexp->setText(regexp);
}

void MainWindow::switchCaptureGroup(int index)
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

	QFile file("STORAGE_FILENAME");
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

void MainWindow::switchModificatorsWindow()
{
	if (!modificatorsWindow->isVisible())
	{
		modificatorsWindow->show();
	}
	else
	{
		modificatorsWindow->close();
	}
}

void MainWindow::switchStorageWindow()
{
	if (!storageWindow->isVisible())
	{
		storageWindow->fillRegExpList();
		storageWindow->show();
	}
	else
	{
		storageWindow->close();
	}
}
