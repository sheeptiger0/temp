#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_server10.h"

class server10 : public QMainWindow
{
    Q_OBJECT

public:
    server10(QWidget *parent = nullptr);
    ~server10();

private:
    Ui::server10Class ui;
};

