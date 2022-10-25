#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    calculus = new Calculus();
    ui->expressionLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Z&!\\(\\)\\s]*"),this));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete calculus;
}


void MainWindow::on_pushButton_clicked()
{
    calculus->setExpression(ui->expressionLineEdit->text());
    calculus->priorityActions();
    calculus->fillingTable(*ui->tableWidget);
}

