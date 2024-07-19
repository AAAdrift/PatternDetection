#include "addtask.h"
#include "task_manager.h"
#include "auxiliary.h"

Addtask::Addtask(QWidget *parent) :
    QWidget(parent)
//    ui(new Ui::Addtask)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    QWidget* centralWidget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QLabel *label12 = new QLabel("Enter the information of the task.");
    layout->addWidget(label12);

    QLabel *label1 = new QLabel("taskname:");
    layout->addWidget(label1);
    nameLineEdit = new QLineEdit(this);
    layout->addWidget(nameLineEdit);

    QLabel *label2 = new QLabel("priority:");
    layout->addWidget(label2);
    prioLineEdit = new QLineEdit(this);
    layout->addWidget(prioLineEdit);

    QLabel *label3 = new QLabel("type:");
    layout->addWidget(label3);
    typeLineEdit = new QLineEdit(this);
    layout->addWidget(typeLineEdit);

    QLabel *label10 = new QLabel(" ");
    layout->addWidget(label10);

    QLabel *label9 = new QLabel("time to remind you:");
    layout->addWidget(label9);

    QLabel *label4 = new QLabel("year:");
    layout->addWidget(label4);
    yearLineEdit = new QLineEdit(this);
    layout->addWidget(yearLineEdit);

    QLabel *label5 = new QLabel("month:");
    layout->addWidget(label5);
    monLineEdit = new QLineEdit(this);
    layout->addWidget(monLineEdit);

    QLabel *label6 = new QLabel("day:");
    layout->addWidget(label6);
    dayLineEdit = new QLineEdit(this);
    layout->addWidget(dayLineEdit);

    QLabel *label7 = new QLabel("hour:");
    layout->addWidget(label7);
    hourLineEdit = new QLineEdit(this);
    layout->addWidget(hourLineEdit);

    QLabel *label8 = new QLabel("minute:");
    layout->addWidget(label8);
    minLineEdit = new QLineEdit(this);
    layout->addWidget(minLineEdit);

    QLabel *label11 = new QLabel(" ");
    layout->addWidget(label11);

    QPushButton* submitButton = new QPushButton("Submit", this);
    connect(submitButton, &QPushButton::clicked, this, &Addtask::submit);
    layout->addWidget(submitButton);
    setLayout(layout);
}

Addtask::~Addtask()
{
    //    delete ui;
}

void Addtask::submit()
{
    std::string name = nameLineEdit->text().toStdString();
    std::string prio = prioLineEdit->text().toStdString();
    std::string type = typeLineEdit->text().toStdString();
    std::string year = yearLineEdit->text().toStdString();
    std::string mon = monLineEdit->text().toStdString();
    std::string day = dayLineEdit->text().toStdString();
    std::string hour = hourLineEdit->text().toStdString();
    std::string min = minLineEdit->text().toStdString();

    // 在这里执行对输入信息的处理，例如保存到文件、显示对话框等
    task tmp;

    ll id = list1.size();

    while(true) {

        map<ll, task>::iterator iter;

        while(true) {
            // cout << "\033[96m" << "请输入任务名称：" << "\033[0m" << endl;
            tmp.taskName = name.c_str();
            bool flag = true;
            for(iter = list1.begin(); iter != list1.end(); iter ++) {
                if(iter->second.taskName == tmp.taskName) {
                    cout << "\033[96m" << "name!" << "\033[0m" << endl;
                    flag = false;

                    Test = new QWidget(this, Qt::Dialog);
                    Test->resize(100, 100);
                    Test->show();
                    QLabel* ttest = new QLabel(Test);
                    ttest->resize(Test->width(), Test->height());
                    ttest->setText("Name ERROR");
                    ttest->show();
                    nameLineEdit->clear();
                    return;
                }
            }
            if(flag) break;
        }

        while(true) {
            if(strcmp("low", prio.c_str()) == 0) {
                tmp.pr = low;
                break;
            }
            else if(strcmp("middle", prio.c_str()) == 0) {
                tmp.pr = middle;
                break;
            }
            else if(strcmp("high", prio.c_str()) == 0) {
                tmp.pr = high;
                break;
            }
            else if(strcmp("emergent", prio.c_str()) == 0) {
                tmp.pr = emergent;
                break;
            }
            else {
                cout << "\033[96m" << "prio" << "\033[0m" << endl;
                Test = new QWidget(this, Qt::Dialog);
                Test->resize(100, 100);
                Test->show();
                QLabel* ttest = new QLabel(Test);
                ttest->resize(Test->width(), Test->height());
                ttest->setText("Priority ERROR");
                ttest->show();
                prioLineEdit->clear();
                return;
            }
        }

        // cout << "\033[96m" << "请输入类型(study/life/play/work):" << "\033[0m" << endl;

        while(true) {
            if(strcmp("study", type.c_str()) == 0) {
                tmp.tp = study;
                break;
            }
            else if(strcmp("play", type.c_str()) == 0) {
                tmp.tp = play;
                break;
            }
            else if(strcmp("life", type.c_str()) == 0) {
                tmp.tp = life;
                break;
            }
            else if(strcmp("work", type.c_str()) == 0) {
                tmp.tp = work;
                break;
            }
            else {
                cout << "\033[96m" << "type" << "\033[0m" << endl;
                Test = new QWidget(this, Qt::Dialog);
                Test->resize(100, 100);
                Test->show();
                QLabel* ttest = new QLabel(Test);
                ttest->resize(Test->width(), Test->height());
                ttest->setText("Type ERROR");
                ttest->show();
                typeLineEdit->clear();
                return;
            }
        }

        tmp.reminded = 0;
        while(true) {
            // cout << "\033[96m" << "请输入开始时间(顺序为:年 月 日 时 分, 示例:2023 10 08 10 00):" << "\033[0m" << endl;
            tmp.st_year = atoi(year.c_str());
            tmp.st_month = atoi(mon.c_str());
            tmp.st_day = atoi(day.c_str());
            tmp.st_hour = atoi(hour.c_str());
            tmp.st_min= atoi(min.c_str());

            time_t currentTime = time(NULL);
            struct tm userTime;

            bool flag = true;
            userTime.tm_year = tmp.st_year - 1900;  // 年份需要减去 1900
            userTime.tm_mon = tmp.st_month - 1;     // 月份需要减去 1
            if(tmp.st_month<1 || tmp.st_month>12) flag = false;
            userTime.tm_mday = tmp.st_day;
            if(tmp.st_day<0 || tmp.st_day>31) flag = false;
            userTime.tm_hour = tmp.st_hour;
            if(tmp.st_hour<0 || tmp.st_hour>23) flag = false;
            userTime.tm_min = tmp.st_min;
            if(tmp.st_min<0 || tmp.st_min>59) flag = false;
            userTime.tm_sec = 0;
            userTime.tm_isdst = -1;          // 设置为 -1 表示不考虑夏令时

            time_t userTimestamp = mktime(&userTime);
            if (userTimestamp != -1 && userTimestamp > currentTime && flag) {
                break;
            } else {
                cout << "\033[96m" << "time！" << "\033[0m" << endl;
                            flag = false;
                exit(0);
            }

            for(iter = list1.begin(); iter != list1.end(); iter ++) {
                if(iter->second.st_day == tmp.st_day && iter->second.st_month == tmp.st_month
                    && iter->second.st_year == tmp.st_year && iter->second.st_hour == tmp.st_hour
                    && iter->second.st_min == tmp.st_min) {
                    cout << "\033[96m" << "time2!" << "\033[0m" << endl;
                    Test = new QWidget(this, Qt::Dialog);
                    Test->resize(100, 100);
                    Test->show();
                    QLabel* ttest = new QLabel(Test);
                    ttest->resize(Test->width(), Test->height());
                    ttest->setText("Time ERROR");
                    ttest->show();
                    yearLineEdit->clear();
                    monLineEdit->clear();
                    dayLineEdit->clear();
                    hourLineEdit->clear();
                    minLineEdit->clear();
                    return;
                }
            }
            if(flag) exit(0);
        }

        while(true) {
            // cout << "\033[96m" << "请输入提醒时间(顺序为:年 月 日 时 分, 示例:2023 10 08 10 00):" << "\033[0m" << endl;
            tmp.rm_year = atoi(year.c_str());
            tmp.rm_month = atoi(mon.c_str());
            tmp.rm_day = atoi(day.c_str());
            tmp.rm_hour = atoi(hour.c_str());
            tmp.rm_min= atoi(min.c_str());
            time_t currentTime = time(NULL);
            struct tm userTime;

            bool flag = true;

            userTime.tm_year = tmp.rm_year - 1900;  // 年份需要减去 1900
            userTime.tm_mon = tmp.rm_month - 1;     // 月份需要减去 1
            if(tmp.rm_month<1 || tmp.rm_month>12) flag = false;
            userTime.tm_mday = tmp.rm_day;
            if(tmp.rm_day<0 || tmp.rm_day>31) flag = false;
            userTime.tm_hour = tmp.rm_hour;
            if(tmp.rm_hour<0 || tmp.rm_hour>23) flag = false;
            userTime.tm_min = tmp.rm_min;
            if(tmp.rm_min<0 || tmp.rm_min>59) flag = false;
            userTime.tm_sec = 0;
            userTime.tm_isdst = -1;          // 设置为 -1 表示不考虑夏令时

            time_t userTimestamp = mktime(&userTime);
            if (userTimestamp != -1 && userTimestamp > currentTime && flag) {
                break;
            } else {
                cout << "\033[96m" << "time!" << "\033[0m" << endl;
                Test = new QWidget(this, Qt::Dialog);
                Test->resize(100, 100);
                Test->show();
                QLabel* ttest = new QLabel(Test);
                ttest->resize(Test->width(), Test->height());
                ttest->setText("Time ERROR");
                ttest->show();
                yearLineEdit->clear();
                monLineEdit->clear();
                dayLineEdit->clear();
                hourLineEdit->clear();
                minLineEdit->clear();
                return;
            }
        }

        id ++;

        for(iter = list1.begin(); iter != list1.end(); iter ++) {
            while(iter->first == id)
                id++;
        }


        list1.insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.tp].insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.pr].insert(map<ll, task>::value_type(id, tmp));

        break;
    }
    Auxiliary aux;
    aux.write();
    Test = new QWidget(this, Qt::Dialog);
    Test->resize(100, 100);
    Test->show();
    QLabel* ttest = new QLabel(Test);
    ttest->resize(Test->width(), Test->height());
    ttest->setText("Success!");
    ttest->show();

    nameLineEdit->clear();
    prioLineEdit->clear();
    typeLineEdit->clear();
    yearLineEdit->clear();
    monLineEdit->clear();
    dayLineEdit->clear();
    hourLineEdit->clear();
    minLineEdit->clear();
}
