#include "reminder.h"
#include "ui_reminder.h"
#include "task_manager.h"

vector<pair<ll, task> > list_sort();
const char* type_itoc(type x);
const char* prior_itoc(prior x);

Reminder::Reminder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Reminder)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);
}

Reminder::~Reminder()
{
    delete ui;
}

//在表格中添加新的一行，打印一个任务的信息
void Reminder::print_task(ll a, task b) {
    int row=ui->remind_list->rowCount();
    //添加新的一行
    ui->remind_list->insertRow(row);

    //ID
    ui->remind_list->setItem(row, 0, new QTableWidgetItem(QString::number(a)));
    ui->remind_list->item(row, 0)->setTextAlignment(Qt::AlignCenter);


    //name
    QString name = QString(QLatin1String(b.taskName));
    ui->remind_list->setItem(row, 1, new QTableWidgetItem(name));
    ui->remind_list->item(row, 1)->setTextAlignment(Qt::AlignCenter);

    //beginning time
    QString st_time = QString::number(b.st_year) + "." + QString::number(b.st_month) + "." + QString::number(b.st_day) + " ";
    if(b.st_hour < 10) st_time += "0";
    st_time += QString::number(b.st_hour) + ":";
    if(b.st_min < 10) st_time += "0";
    st_time += QString::number(b.st_min);\
    ui->remind_list->setItem(row, 2, new QTableWidgetItem(st_time));
    ui->remind_list->item(row, 2)->setTextAlignment(Qt::AlignCenter);

    //reminding time
    QString rm_time = QString::number(b.rm_year) + "." + QString::number(b.rm_month) + "." + QString::number(b.rm_day) + " ";
    if(b.rm_hour < 10) rm_time += "0";
    rm_time += QString::number(b.rm_hour) + ":";
    if(b.rm_min < 10) rm_time += "0";
    rm_time += QString::number(b.rm_min);
    ui->remind_list->setItem(row, 3, new QTableWidgetItem(rm_time));
    ui->remind_list->item(row, 3)->setTextAlignment(Qt::AlignCenter);

    //type
    QString tp = type_itoc(b.tp);
    ui->remind_list->setItem(row, 4, new QTableWidgetItem(tp));
    ui->remind_list->item(row, 4)->setTextAlignment(Qt::AlignCenter);

    //priority
    QString pr = prior_itoc(b.pr);
    ui->remind_list->setItem(row, 5, new QTableWidgetItem(pr));
    ui->remind_list->item(row, 5)->setTextAlignment(Qt::AlignCenter);
}

//打印需提醒的任务列表rm_list
void Reminder::print_remind_list() {
    ui->remind_list->clearContents();//清空原有表格
    //设置格式
    ui->remind_list->setRowCount(0);
    ui->remind_list->setColumnCount(6);
    ui->remind_list->setColumnWidth(0,300);
    //设置表头格式
    QStringList listHeader;
    listHeader<<"任务ID"<<"任务名称"<<"启动时间"<<"提醒时间"<<"类型"<<"优先级";
    ui->remind_list->setHorizontalHeaderLabels(listHeader);
    ui->remind_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //一一打印
    for(int i = 0; i < rm_list.size(); ++ i) {
        ll a = rm_list[i].first;
        task b = rm_list[i].second;
        print_task(a, b);
    }
}
