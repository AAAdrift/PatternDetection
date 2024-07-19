#include "warning.h"
#include "ui_warning.h"

Warning::Warning(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Warning)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);
}

Warning::~Warning()
{
    delete ui;
}

void Warning::warning(QString qstr)
{
    ui -> textBrowser -> clear();
    ui -> textBrowser -> setText(qstr);
    Warning *configWindow = new Warning;
    configWindow -> show();
}
