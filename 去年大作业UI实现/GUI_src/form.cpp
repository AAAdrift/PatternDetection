//注册界面
#include "form.h"
#include "ui_form.h"
#include <fstream>
#include <functional>
#include "warning.h"
#include "succeed.h"

using namespace std;

bool judge = false;

void errorWindow(const QString windowName, const QString context);

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);
    //设置文本字体
    ui->pushButton->setFont(QFont("宋体", 18));
    //将密码输入行设为隐式文本
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    ui->lineEdit_3->setEchoMode(QLineEdit::Password);
}

Form::~Form()
{
    delete ui;
}

//注册
void Form::on_pushButton_clicked()
{
    Warning w;
    fstream f;
    f.open("users.txt", ios::in|ios::app);
    QString qid, qpassword, qvpassword;
    //接受输入的用户名和密码
    qid = ui -> lineEdit -> text();
    qpassword = ui -> lineEdit_2 -> text();
    qvpassword = ui -> lineEdit_3 -> text();
    //判断两次输入是否一致
    if(qpassword != qvpassword)
    {
        //报错窗口并清除密码输入
        errorWindow("报错信息", "两次密码不一致");
        ui -> lineEdit_2 -> clear();
        ui -> lineEdit_3 -> clear();
        return;
    }
    //QString和string转换
    string id = qid.toStdString();
    string password = qpassword.toStdString();
    bool flag = false;
    //判断用户名是否已存在
    while(!flag && !f.eof())
    {
        string tmp_id;
        size_t tmp_password;
        f >> tmp_id >> tmp_password;
        if(id == tmp_id) flag = true;
    }
    f.close();
    if(flag)
    {
        errorWindow("报错信息", "用户名已被使用");
        ui -> lineEdit -> clear();
        return;
    }
    //密码加密保存
    hash<string> change;
    size_t word = change(password);
    //将用户名和密码写入文本
    f.open("users.txt", ios::in|ios::app);
    f << id << " " << word << endl;

    succeed *config = new succeed;
    config -> show();

}

//将密码在显式文本和隐式文本间切换
void Form::on_checkBox_stateChanged(int arg1)
{
    if(!judge)
    {
        ui -> lineEdit_2 -> setEchoMode(QLineEdit::Normal);
        ui -> lineEdit_3 -> setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui -> lineEdit_2 -> setEchoMode(QLineEdit::Password);
        ui -> lineEdit_3 -> setEchoMode(QLineEdit::Password);
    }
    judge = !judge;
}
