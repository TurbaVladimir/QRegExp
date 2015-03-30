#ifndef STORAGEWINDOW_H
#define STORAGEWINDOW_H

#include <QWidget>
#include <QString>

#include "ui_storage.h"

class StorageWindow : public QWidget
{
	Q_OBJECT
public:
	explicit StorageWindow();
	~StorageWindow();

	Ui_storageWindow* ui;

public slots:
	void fillRegExpList();

private slots:
	void loadRegExp();
	void deleteRegExp();

signals:
	void regExp(QString pattern);
	void statusBarText(QString text, int time = 0);
};

#endif // STORAGEWINDOW_H
