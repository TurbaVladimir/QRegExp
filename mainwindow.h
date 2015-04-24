#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegExp>
#include <QString>
#include <QWidget>
#include <QList>

#include "modificatorswindow.h"
#include "storagewindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow* ui;
	QRegularExpressionMatch lastMatch;
	ModificatorsWindow* modificatorsWindow;
	StorageWindow* storageWindow;
	QList<QString> captures;

public slots:
	void check();
	void setStatusBarText(QString text, int time);
	void setRegExp(QString regexp);

private slots:
	void switchCaptureGroup(int index);
	void saveRegExp();
	void switchModificatorsWindow();
	void switchStorageWindow();
};

#endif // MAINWINDOW_H
