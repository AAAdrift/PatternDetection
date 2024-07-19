#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "form.h"
#include "help.h"
#include "log.h"
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("初始界面");
    //设置按钮字体大小
    ui->pushButton->setFont(QFont("宋体", 18));
    ui->pushButton_2->setFont(QFont("宋体", 18));
    ui->pushButton_3->setFont(QFont("宋体", 18));
    //设置背景图
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//打开注册界面
void MainWindow::on_pushButton_clicked()
{
    Form *configWindow = new Form;
    configWindow -> show();
}

//打开登录界面
void MainWindow::on_pushButton_2_clicked()
{
    Log *configWindow = new Log;
    configWindow -> show();
}

//打开帮助界面
void MainWindow::on_pushButton_3_clicked()
{
    Help *configWindow = new Help;
    configWindow -> show();
}
