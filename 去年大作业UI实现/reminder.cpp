#include <time.h>
#include "task_manager.h"
using namespace std;

vector<pair<ll, task> > list_sort();
const char* type_itoc(type x);
const char* prior_itoc(prior x);
void win(string s);

//提醒函数
void reminder() {
    vector<pair<ll, task> > sorted = list_sort();//将文件中已有任务以时间排序
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
            list[a].reminded = true;
            string remind_info = "Reminding time of task '";
            remind_info += b.taskName;
            remind_info += "' is now!\n";
            remind_info += "taskName\tbeginTime            \ttype\t\n";
            char tmp[400], tmp1[500], tmp2[100], tmp3[100];
            sprintf(tmp1, "%s       \t", b.taskName);
            sprintf(tmp2, "%d.%d.%d %02d:%02d\t", b.st_year, b.st_month, b.st_day, b.st_hour, b.st_min);
            sprintf(tmp3, "%s\t", type_itoc(b.tp));
            sprintf(tmp, "%s%s%s", tmp1, tmp2, tmp3);
            remind_info += tmp;
            win(remind_info);//弹窗提醒
        }
    }
}