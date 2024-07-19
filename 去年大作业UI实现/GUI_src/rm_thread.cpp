#include "rm_thread.h"
#include "task_manager.h"
#include "auxiliary.h"

vector<pair<ll, task> > list_sort();
const char* type_itoc(type x);
const char* prior_itoc(prior x);

Rm_thread::Rm_thread(QObject *parent): QThread(parent){}

//查看提醒时间已到的任务，并保存在全局变量rm_list中
void Rm_thread::check_remind() {
    vector<pair<ll, task> > sorted = list_sort();//将文件中已有任务以时间排序
    rm_list.clear();
    for(int i = 0; i < sorted.size(); ++ i) {
        ll a = sorted[i].first;
        task b = sorted[i].second;

        if (b.reminded) continue;//判断任务是否已经被提醒过

        time_t currentTime = time(NULL);//提取此时时间
        struct tm userTime;
        userTime.tm_year = b.rm_year - 1900;  // 年份需要减去 1900
        userTime.tm_mon = b.rm_month - 1;     // 月份需要减去 1
        userTime.tm_mday = b.rm_day;
        userTime.tm_hour = b.rm_hour;
        userTime.tm_min = b.rm_min;
        userTime.tm_sec = 0;
        userTime.tm_isdst = -1;          // 设置为 -1 表示不考虑夏令时
        time_t userTimestamp = mktime(&userTime);

        //当检测到任务提醒时间与系统此时时间相差20s以内时启动提醒
        if (abs(userTimestamp - currentTime) <= 20) {
            rm_list.push_back(make_pair(a, b));
            list1[a].reminded = true;//标记已提醒，以免重复扫描
        }
    }
}

//对QThread类run函数重写，每1秒判断一次是否需提醒
void Rm_thread::run() {
    while(1) {
        check_remind();
        if(!rm_list.empty()) emit sgn();
        QThread::sleep(1);
    }
}
