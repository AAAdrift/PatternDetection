#include "inquiry.h"
#include "ui_inquiry.h"
#include "mainwindow.h"
#include <QString>
#include<stdio.h>
#include<cstdio>
#include<iostream>
#include<string.h>

using namespace std;


const char* type_itoc(type x){ //将任务类型的枚举类型转换为对应名称的string
    switch (x) {
        case 5: return "study"; break;
        case 6: return "play"; break;
        case 7: return "life"; break;
        case 8: return "work"; break;
        default: return "others";break;
    }
}

const char* prior_itoc(prior x) {//将任务优先级的枚举类型转换为对应名称的string
    switch (x) {
        case 1: return "low";
        case 2: return "middle";
        case 3: return "high";
        case 4: return "emergent";
        default: return "others";
    }
}

Inquiry::Inquiry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Inquiry)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);
    ui->result_list->verticalHeader()->setVisible(false);
    ui->result_list->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置表格不可修改
    ui->result_list->setStyleSheet("selection-background-color: rgb(255, 125, 0);");
    ui->result_list->horizontalHeader()->setStyleSheet("QHeaderView::section{background:orange;}");
    ui->inquiryCal->setMaximumDate(QDate(2025,12,31));
    ui->inquiryCal->setMinimumDate(QDate(1999,1,1));
    //两种查询方式，一种搜索名字，一种点击日历，而优先级和类型都是附加的筛选条件
    connect(ui->inquiryCal, SIGNAL(clicked(QDate)), this, SLOT(print_with_date()));
    connect(ui->toSearch, SIGNAL(clicked()), this, SLOT(print_with_name()));
}

Inquiry::~Inquiry()
{
    delete ui;
}

void Inquiry::print_task(ll a, task b) {
    int row=ui->result_list->rowCount();
    //添加新的一行
    ui->result_list->insertRow(row);

    //ID
    //QTableWidgetItem* item=new QTableWidgetItem("QString::number(a)");
    //item->setText(QString::number(a));
    //ui->db->setText(QString::number(row));
    ui->result_list->setItem(row, 0, new QTableWidgetItem(QString::number(a)));
    ui->result_list->item(row, 0)->setTextAlignment(Qt::AlignCenter);

    //name
    QString name = QString(QLatin1String(b.taskName));
    ui->result_list->setItem(row, 1, new QTableWidgetItem(name));
    ui->result_list->item(row, 1)->setTextAlignment(Qt::AlignCenter);

    //beginning time
    QString st_time = QString::number(b.st_year) + "." + QString::number(b.st_month) + "." + QString::number(b.st_day) + " ";
    if(b.st_hour < 10) st_time += "0";
    st_time += QString::number(b.st_hour) + ":";
    if(b.st_min < 10) st_time += "0";
    st_time += QString::number(b.st_min);
    ui->result_list->setItem(row, 2, new QTableWidgetItem(st_time));
    ui->result_list->item(row, 2)->setTextAlignment(Qt::AlignCenter);

    //reminding time
    QString rm_time = QString::number(b.rm_year) + "." + QString::number(b.rm_month) + "." + QString::number(b.rm_day) + " ";
    if(b.rm_hour < 10) rm_time += "0";
    rm_time += QString::number(b.rm_hour) + ":";
    if(b.rm_min < 10) rm_time += "0";
    rm_time += QString::number(b.rm_min);
    ui->result_list->setItem(row, 3, new QTableWidgetItem(rm_time));
    ui->result_list->item(row, 3)->setTextAlignment(Qt::AlignCenter);

    //type
    QString tp = type_itoc(b.tp);
    ui->result_list->setItem(row, 4, new QTableWidgetItem(tp));
    ui->result_list->item(row, 4)->setTextAlignment(Qt::AlignCenter);

    //priority
    QString pr = prior_itoc(b.pr);
    ui->result_list->setItem(row, 5, new QTableWidgetItem(pr));
    ui->result_list->item(row, 5)->setTextAlignment(Qt::AlignCenter);
}
void Inquiry::print_with_date() {
    ui->result_list->clearContents();
    QDate date = ui->inquiryCal->selectedDate();
    int year = date.year();
    int month = date.month();
    int day = date.day();
    ui->result_list->setRowCount(0);
    ui->result_list->setColumnCount(6);
    ui->result_list->setColumnWidth(0,300);
    QStringList listHeader;
    listHeader<<"任务ID"<<"任务名称"<<"启动时间"<<"提醒时间"<<"类型"<<"优先级";
    ui->result_list->setHorizontalHeaderLabels(listHeader);
    ui->result_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //map<ll, task> search_result;
    /*int pr = ui->search_type->currentIndex();
    prior tg_prior;
    switch(pr) {
    case 1:
        tg_prior = low;
        break;
    case 2:
        tg_prior = middle;
        break;
    case 3:
        tg_prior = high;
        break;
    case 4:
        tg_prior = emergent;
    default:
        tg_prior
    }

    int  tp = ui->search_prior->currentIndex();
    type tg_tp;
    switch(tp) {

    }*/

    int target_prior = ui->search_prior->currentIndex();
    int target_type = ui->search_type->currentIndex();

    bool prior_flag = (ui->prior_valid->isChecked())?(target_prior == 0):1;
    bool type_flag = (ui->type_valid->isChecked())?(target_type == 0):1;
    target_type += 4;
    //cout<<prior_flag<<" "<<type_flag<<" "<<target_prior<<" "<<target_type<<endl;

    map<ll, task>::iterator iter;
    for(iter = list1.begin(); iter != list1.end(); ++iter) {
        ll a = iter->first;
        task b = iter -> second;
        if ((prior_flag || cls[target_prior].count(a)) && (type_flag || cls[target_type].count(a)) && b.st_year == year && b.st_month == month && b.st_day == day) {
            print_task(a, b);
        }
    }
}

void Inquiry::print_with_name(){
    ui->result_list->clearContents();
    ui->result_list->setRowCount(0);
    ui->result_list->setColumnCount(6);
    ui->result_list->setColumnWidth(0,300);
    QStringList listHeader;
    listHeader<<"任务ID"<<"任务名称"<<"启动时间"<<"提醒时间"<<"类型"<<"优先级";
    ui->result_list->setHorizontalHeaderLabels(listHeader);
    ui->result_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QString searchname = ui->searchName->text();
    string target_name = searchname.toStdString();

    int target_prior = ui->search_prior->currentIndex();
    int target_type = ui->search_type->currentIndex();
    bool prior_flag = (ui->prior_valid->isChecked())?(target_prior == 0):1;
    bool type_flag = (ui->type_valid->isChecked())?(target_type == 0):1;
    target_type += 4;

    map<ll, task>::iterator iter;
    for(iter = list1.begin(); iter != list1.end(); ++iter) {
        ll a = iter->first;
        task b = iter -> second;
        QString name = QString::fromStdString(b.taskName);
        if ((prior_flag || cls[target_prior].count(a)) && (type_flag || cls[target_type].count(a)) && name.contains(searchname, Qt::CaseInsensitive)) {
            print_task(a, b);
        }
    }
}
