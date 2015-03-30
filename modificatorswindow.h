#ifndef MODIFICATORSWINDOW_H
#define MODIFICATORSWINDOW_H

#include <QWidget>

#include "ui_modificators.h"

class ModificatorsWindow : public QWidget
{
	Q_OBJECT
public:
	explicit ModificatorsWindow();
	~ModificatorsWindow();

	Ui_modificatorsWindow* ui;
};

#endif // MODIFICATORSWINDOW_H
