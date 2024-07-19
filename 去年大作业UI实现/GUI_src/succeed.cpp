#include "succeed.h"
#include "ui_succeed.h"

succeed::succeed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::succeed)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);
}

succeed::~succeed()
{
    delete ui;
}
