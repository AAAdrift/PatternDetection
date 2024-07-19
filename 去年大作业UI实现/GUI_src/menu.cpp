#include "menu.h"
#include "ui_menu.h"
#include "auxiliary.h"
#include "addtask.h"
#include "deltask.h"
#include "task_manager.h"
#include "inquiry.h"
#include "reminder.h"
#include "rm_thread.h"
#include "add_task.h"

//登录后的主菜单
Menu::Menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menu)
{
    //设置显示背景
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);

    //新建子线程，执行提醒功能
    Rm_thread *rmThread = new Rm_thread;
    rmThread->start();
    //连接信号函数sgn和打印任务的槽函数print_with_list
    connect(rmThread, &Rm_thread::sgn, this, [=]() {
        Reminder *rm_window = new Reminder;
        rm_window->print_remind_list();
        rm_window->show();
    });
}

Menu::~Menu()
{
    delete ui;
}

void Menu::on_pushButton_2_clicked()
{
    Add_task *config = new Add_task;
    config -> show();
}


void Menu::on_pushButton_3_clicked()
{
    Deltask *config = new Deltask;
    config -> show();
}

void Menu::slot1() {
    list1.clear();
    for(int i = 0; i < 9; i ++) {
        cls[i].clear();
    }
}


void Menu::on_pushButton_clicked()
{
    Inquiry *config = new Inquiry;
    config -> show();
}

