#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->setDataDirAndLoad(QApplication::applicationDirPath() + "/data/dialogue");
}

MainWindow::~MainWindow()
{
    delete ui;
}

