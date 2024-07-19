#include "add_task.h"
#include "ui_add_task.h"
#include "task_manager.h"
#include <QLabel>
#include <QDate>
#include <QTime>
#include "auxiliary.h"

#include <QPushButton>

void errorWindow(const QString windowName, const QString context);

Add_task::Add_task(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Add_task)
{
    //设置ui背景图片
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);

    //设置submit为添加按钮的槽函数
    connect(ui->submitButton, &QPushButton::clicked, this, &Add_task::submit);
    //设置添加界面中的默认选择时间为当前时间
    ui->getTime->setTime(QTime::currentTime());
}

Add_task::~Add_task()
{
    delete ui;
}

void Add_task::submit() {
    std::string name = ui->getName->text().toStdString();//name是用户在文本框中输入的任务名称
    if(name == "") {
        errorWindow("错误信息", "请输入任务名称");
        return;
    }
    int type = ui->getType->currentIndex();
    int prior = ui->getPrior->currentIndex();
    if(type == 0)  type = 7;else type += 4;//type=0转换为默认的middle
    if(prior == 0) prior = 2;//prior=0转换为默认的middle

    QDate date = ui->getDate->selectedDate();//date是用户在日历上选择的日期
    if(date.daysTo(QDate::currentDate())>0) {
        errorWindow("错误信息", "请选择今天或晚于今天的日期");
        return;
    }
    if(date.daysTo(QDate::currentDate()) == 0 && ui->getTime->time().secsTo(QTime::currentTime()) >= 0) {
        errorWindow("错误信息", "请选择晚于当前的时刻");
        return;
    }

    int year = date.year();
    int mon = date.month();
    int day = date.day();

    int hour = ui->getTime->time().hour();
    int minute = ui->getTime->time().minute();

    // 在这里执行对输入信息的处理，例如保存到文件、显示弹窗等
    task tmp;

    ll id = list1.size();
    map<ll, task>::iterator iter;

    tmp.taskName = name.c_str();
    for(iter = list1.begin(); iter != list1.end(); iter ++) {
        if(iter->second.taskName == tmp.taskName) {
            errorWindow("报错信息", "该任务名称已存在，请重新输入");
            return;
            }
    }
    switch(prior) {
    case low:
        tmp.pr = low;break;
    case middle:
        tmp.pr = middle;break;
    case high:
        tmp.pr = high;break;
    case emergent:
        tmp.pr = emergent;break;
    }
    switch(type) {
    case study:
        tmp.tp = study;break;
    case play:
        tmp.tp = play;break;
    case life:
        tmp.tp = life;break;
    case work:
        tmp.tp = work;break;
    }

    tmp.reminded = 0;
    for(iter = list1.begin(); iter != list1.end(); iter ++) {
        if(iter->second.st_day == day && iter->second.st_month == mon
        && iter->second.st_year == year && iter->second.st_hour == hour
        && iter->second.st_min == minute) {
            errorWindow("报错信息", "该开始时间已存在，请重新选择");
            return;
        }
    }
    tmp.st_year = year;
    tmp.st_month = mon;
    tmp.st_day = day;
    tmp.st_hour = hour;
    tmp.st_min= minute;
    tmp.rm_year = year;
    tmp.rm_month = mon;
    tmp.rm_day = day;
    tmp.rm_hour = hour;
    tmp.rm_min= minute;

    id ++;
    for(iter = list1.begin(); iter != list1.end(); iter ++) {
        while(iter->first == id)
            id++;
    }

    list1.insert(map<ll, task>::value_type(id, tmp));
    cls[tmp.tp].insert(map<ll, task>::value_type(id, tmp));
    cls[tmp.pr].insert(map<ll, task>::value_type(id, tmp));
    Auxiliary aux;
    aux.write();
    errorWindow("提示", "成功添加任务");
}
