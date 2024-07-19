#include "auxiliary.h"
#include "ui_auxiliary.h"
#include <fstream>
#include <cstring>
#include "task_manager.h"
#include <QLabel>

using namespace std;

Auxiliary::Auxiliary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Auxiliary)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    ui->setupUi(this);
}

Auxiliary::~Auxiliary()
{
    delete ui;
}

void Auxiliary::read()
{
    fstream fr;

    fr.open(id_present, ios::in|ios::app);
    task tmp;

    //读入任务
    while(!fr.eof()) {
        ll id;
        fr >> id;
        if(fr.eof()) break;
        fr >> tmp.taskName;


        //读入任务优先级
        char priorityGet[10];
        fr >> priorityGet;
        while(true) {
            if(strcmp("low", priorityGet) == 0) {
                tmp.pr = low;
                break;
            }
            else if(strcmp("middle", priorityGet) == 0) {
                tmp.pr = middle;
                break;
            }
            else if(strcmp("high", priorityGet) == 0) {
                tmp.pr = high;
                break;
            }
            else if(strcmp("emergent", priorityGet) == 0) {
                tmp.pr = emergent;
                break;
            }
            else {
                cout << "ERROR! Enter again!" << endl;
                exit(0);
                // break;
            }
        }

        //读入任务类型
        char typeGet[10];
        fr >> typeGet;
        while(true) {
            if(strcmp("study", typeGet) == 0) {
                tmp.tp = study;
                break;
            }
            else if(strcmp("play", typeGet) == 0) {
                tmp.tp = play;
                break;
            }
            else if(strcmp("life", typeGet) == 0) {
                tmp.tp = life;
                break;
            }
            else if(strcmp("work", typeGet) == 0) {
                tmp.tp = work;
                break;
            }
            else {
                cout << "ERROR! Enter again!" << endl;
                exit(0);
            }
        }

        fr >> tmp.reminded;

        //读入任务的启动时间与提醒时间
        fr >> tmp.st_year >> tmp.st_month >> tmp.st_day >> tmp.st_hour >> tmp.st_min;

        fr >> tmp.rm_year >> tmp.rm_month >> tmp.rm_day >> tmp.rm_hour >> tmp.rm_min;

        //转换存储任务的优先级和类型
        list1.insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.tp].insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.pr].insert(map<ll, task>::value_type(id, tmp));

    }
    fr.close();

    cin.clear();
}

void Auxiliary::write() {
    fstream fo;
    //以从头开始写入的方式打开文件
    fo.open(id_present, ios::out);
    if(!fo.is_open()) {
        fo << "无法打开文件" << endl;
        fo.close();
        return;
    }

    //将现有任务依次写入文件
    map<ll, task>::iterator iter;
    bool flag = true;
    for(iter = list1.begin(); iter != list1.end(); iter ++) {
        fo << iter->first << " " << iter->second.taskName << " ";
        //转换并写入任务的优先级
        switch (iter->second.pr) {
        case 1:
            fo << "low" << " ";
            break;
        case 2:
            fo << "middle" << " ";
            break;
        case 3:
            fo << "high" << " ";
            break;
        case 4:
            fo << "emergent" << " ";
            break;
        default:
            break;
        }
        //转换并写入任务的类型
        switch (iter->second.tp) {
        case 5:
            fo << "study" << " ";
            break;
        case 6:
            fo << "play" << " ";
            break;
        case 7:
            fo << "life" << " ";
            break;
        case 8:
            fo << "work" << " ";
            break;
        default:
            break;
        }
        //写入任务的启动时间和提醒时间

        fo << (int)iter->second.reminded << " ";
        fo << iter->second.st_year << " " << iter->second.st_month << " "
           << iter->second.st_day << " " << iter->second.st_hour << " " << iter->second.st_min;
        fo << " " << iter->second.rm_year << " " << iter->second.rm_month << " "
           << iter->second.rm_day << " " << iter->second.rm_hour << " " << iter->second.rm_min << endl;
    }

    fo.close();
}

bool cmp(pair<ll, task> a, pair<ll, task> b) {
    int y = a.second.rm_year;
    int m = a.second.rm_month;
    int d = a.second.rm_day;
    int hour = a.second.rm_hour;
    int min = a.second.rm_min;
    int yy = b.second.rm_year;
    int mm = b.second.rm_month;
    int dd = b.second.rm_day;
    int hhour = b.second.rm_hour;
    int mmin = b.second.rm_min;
    //按提醒时间的顺序排序
    if (y != yy) return (y < yy);
    if (m != mm) return (m < mm);
    if (d != dd) return (d < dd);
    if (hour != hhour) return (hour < hhour);
    if (min != mmin) return (min < mmin);
    return a.second.pr <= b.second.pr;
}

//按时间排序函数
vector<pair<ll, task> > list_sort() {
    vector <pair<ll, task> > v(list1.begin(), list1.end());//将map赋给vector
    sort(v.begin(), v.end(), cmp);//对vector排序
    return v;
}

void errorWindow(const QString windowName, const QString context) {
    QWidget* Test;
    Test = new QWidget;
    Test->setWindowTitle(windowName);
    Test->resize(250, 100);
    Test->show();
    QLabel* ttest = new QLabel(Test);
    ttest->setAlignment(Qt::AlignCenter);
    ttest->resize(Test->width(), Test->height());
    ttest->setText(context);
    ttest->show();
}
