#include "log.h"
#include "ui_log.h"
#include <functional>
#include <fstream>
#include <map>
#include "Warning.h"
#include "task_manager.h"
#include "auxiliary.h"
#include "menu.h"

using namespace std;

bool flag = false;

void errorWindow(const QString windowName, const QString context);

Log::Log(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Log)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);

    //设置文本字体
    ui->pushButton->setFont(QFont("宋体", 18));
    //将密码行改为隐式文本
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
}

Log::~Log()
{
    delete ui;
}

//查询用户是否存在
bool Log::check(string id)
{
    fstream fi;
    bool flag = false;
    fi.open("users.txt", ios::in);
    while(!flag && !fi.eof())
    {
        string tmp_id;
        size_t tmp_word;
        fi >> tmp_id >> tmp_word;
        if(id == tmp_id) flag = true;
    }
    fi.close();
    return flag;
}

void Log::on_pushButton_clicked()
{
    Warning w;
    //接受用户名和密码的输入
    QString qid, qpassword;
    qid = ui -> lineEdit -> text();
    qpassword = ui -> lineEdit_2 -> text();
    string id = qid.toStdString();
    string password = qpassword.toStdString();

    //将读入的密码加密
    fstream fi;
    fi.open("users.txt", ios::in);
    if(!fi.is_open()) {
        fi.open("users.txt", ios::app);
        fi.close();
        errorWindow("错误信息", "用户名不存在");
        return;
    }
    hash<string> change;
    size_t word = change(password);

    //读入存放用户的文件到内存
    map<string, size_t> info;
    while(!fi.eof())
    {
        string tmp_id;
        size_t tmp_word;
        fi >> tmp_id >> tmp_word;
        info.insert(make_pair(tmp_id, tmp_word));
    }
    map<string, size_t>::iterator it = info.find(id);
    fi.close();

    //判断密码是否正确
    if(word != it -> second)
    {
        errorWindow("报错信息", "密码错误");
        return;
    }

    id_present = id;

    Auxiliary aux;

    //读入存放用户任务的文件
    aux.read();

    Menu *config = new Menu;
    config -> show();
}


void Log::on_checkBox_stateChanged(int arg1)
{
    if(!flag)
    {
        ui -> lineEdit_2 -> setEchoMode(QLineEdit::Normal);
    }
    else ui -> lineEdit_2 -> setEchoMode(QLineEdit::Password);
    flag = !flag;
}

