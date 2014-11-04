#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->checkButton, SIGNAL(clicked()), this, SLOT(check()));
    connect(ui->regexp, SIGNAL(returnPressed()), this, SLOT(check()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::check()
{
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
    QRegularExpressionMatch match = exp.match(ui->testText->toPlainText());

    ui->resultText->clear();
    ui->statusBar->showMessage(QString("Captured %1 texts").arg(match.capturedTexts().size()), 5000);

    for (int i = 0; i < match.capturedTexts().size(); i++)
    {
        QString string = QString("[%1][%2-%3] '%4'\n").
                arg(i).
                arg(match.capturedStart(i)).
                arg(match.capturedEnd(i)).
                arg(match.captured(i));

        ui->resultText->setText(QString("%1%2").arg(ui->resultText->toPlainText()).arg(string));
    }
}
