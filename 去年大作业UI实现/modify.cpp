#include "task_manager.h"
#include <SFML/Graphics.hpp>
using namespace std;

void win(string);
void write();
void read();

//在程序中添加文件
void add() {

    task tmp;

    ll id = list.size();

    while(true) {

        map<ll, task>::iterator iter;

        //创建任务名称
        while(true) {
            cout << "\033[96m" << "请输入任务名称：" << "\033[0m" << endl;
            cin >> tmp.taskName;
            bool flag = true;
            for(iter = list.begin(); iter != list.end(); iter ++) {
                if(strcmp(iter->second.taskName, tmp.taskName) == 0) {
                    cout << "\033[96m" << "任务名称重复!" << "\033[0m" << endl;
                    win("The task name already exists!");
                    flag = false;
                    break;
                }
            }
            if(flag) break;
        }

        //创建任务优先级
        cout << "\033[96m" << "请输入优先级(low/middle/high/emergent, 回车则默认middle):" << "\033[0m" << endl;
        char priorityGet[10];
        cin.ignore();
        cin.getline(priorityGet, 10);
        //cin >> priorityGet;
        if(strcmp("low", priorityGet) == 0) tmp.pr = low;
        else if(strcmp("middle", priorityGet) == 0) tmp.pr = middle;
        else if(strcmp("high", priorityGet) == 0) tmp.pr = high;
        else if(strcmp("emergent", priorityGet) == 0) tmp.pr = emergent;
        else 
            /*cout << "\033[96m" << "错误！请重新输入！" << "\033[0m" << endl;
            win("Error!Please input again!");
            cin >> priorityGet;*/
            tmp.pr = middle;

        //创建任务类型
        cout << "\033[96m" << "请输入类型(study/life/play/work, 回车则默认life):" << "\033[0m" << endl;
        char typeGet[10];
        cin.getline(typeGet, 10);
        //cin >> typeGet;
        if(strcmp("study", typeGet) == 0) tmp.tp = study;
        else if(strcmp("play", typeGet) == 0) tmp.tp = play;
        else if(strcmp("life", typeGet) == 0) tmp.tp = life;
        else if(strcmp("work", typeGet) == 0) tmp.tp = work;
        else 
            /*cout << "\033[96m" << "错误！请重新输入！" << "\033[0m" << endl;
            win("Error!Please input again!");*/
            tmp.tp = life;

        //创建任务启动时间和提醒时间
        while(true) {
            cout << "\033[96m" << "请输入开始时间(顺序为:年 月 日 时 分, 示例:2023 10 08 10 00):" << "\033[0m" << endl;
            cin >> tmp.st_year >> tmp.st_month >> tmp.st_day >> tmp.st_hour >> tmp.st_min;

            time_t currentTime = time(NULL);
            struct tm userTime;

            bool flag = true;
            userTime.tm_year = tmp.st_year - 1900;  // 年份需要减去 1900
            userTime.tm_mon = tmp.st_month - 1;     // 月份需要减去 1
            if(tmp.st_month<1 || tmp.st_month>11) flag = false;
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
                cout << "\033[96m" << "时间错误！" << "\033[0m" << endl;
                win("Invalid time!");
                flag = false;
            }

            for(iter = list.begin(); iter != list.end(); iter ++) {
                if(iter->second.st_day == tmp.st_day && iter->second.st_month == tmp.st_month 
                && iter->second.st_year == tmp.st_year && iter->second.st_hour == tmp.st_hour 
                && iter->second.st_min == tmp.st_min) {
                    cout << "\033[96m" << "任务开始时间重合!" << "\033[0m" << endl;
                    flag = false;
                    break;
                }
            }
            if(flag) break;
        }

        while(true) {
            cout << "\033[96m" << "请输入提醒时间(顺序为:年 月 日 时 分, 示例:2023 10 08 10 00):" << "\033[0m" << endl;
            cin >> tmp.rm_year >> tmp.rm_month >> tmp.rm_day >> tmp.rm_hour >> tmp.rm_min;
            time_t currentTime = time(NULL);
            struct tm userTime;

            bool flag = true;

            userTime.tm_year = tmp.rm_year - 1900;  // 年份需要减去 1900
            userTime.tm_mon = tmp.rm_month - 1;     // 月份需要减去 1
            if(tmp.rm_month<1 || tmp.rm_month>11) flag = false;
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
                cout << "\033[96m" << "时间错误!" << "\033[0m" << endl;
                win("Invalid time!");
            }
        }

        id ++;

        for(iter = list.begin(); iter != list.end(); iter ++) {
            while(iter->first == id)
                id++;
        }

        tmp.reminded = false;
        
        list.insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.tp].insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.pr].insert(map<ll, task>::value_type(id, tmp));
        
        
        cout << "\033[96m" << "是否继续添加任务? Y or n: " << "\033[0m" << endl;
        char conti[10];
        cin >> conti;
        if(strcasecmp("y", conti) != 0) break;
        
    }

    write();
}

//命令行直接添加任务
void _add() {

    task tmp;

    ll id = list.size();

    while(true) {

        map<ll, task>::iterator iter;

        while(true) {
            // cout << "\033[96m" << "请输入任务名称：" << "\033[0m" << endl;
            strcpy(tmp.taskName, _argv[4]);
            bool flag = true;
            for(iter = list.begin(); iter != list.end(); iter ++) {
                if(strcmp(iter->second.taskName, tmp.taskName) == 0) {
                    cout << "\033[96m" << "任务名称重复!" << "\033[0m" << endl;
                    win("The task name already exists!");
                    flag = false;
                    exit(0);
                    break;
                }
            }
            if(flag) break;
        }

        // cout << "\033[96m" << "请输入优先级(low, middle, high, emergent):" << "\033[0m" << endl;
        char priorityGet[10];
        strcpy(priorityGet, _argv[5]);
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
                cout << "\033[96m" << "错误！请重新输入！" << "\033[0m" << endl;
                win("Error!Please input again!");
                cin >> priorityGet;
                exit(0);
            }
        }
        
        // cout << "\033[96m" << "请输入类型(study/life/play/work):" << "\033[0m" << endl;
        char typeGet[10];
        strcpy(typeGet, _argv[6]);
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
                cout << "\033[96m" << "错误！请重新输入！" << "\033[0m" << endl;
                win("Error!Please input again!");
                exit(0);
            }
        }

        while(true) {
            // cout << "\033[96m" << "请输入开始时间(顺序为:年 月 日 时 分, 示例:2023 10 08 10 00):" << "\033[0m" << endl;
            tmp.st_year = atoi(_argv[7]);
            tmp.st_month = atoi(_argv[8]);
            tmp.st_day = atoi(_argv[9]);
            tmp.st_hour = atoi(_argv[10]);
            tmp.st_min= atoi(_argv[11]);

            time_t currentTime = time(NULL);
            struct tm userTime;

            bool flag = true;
            userTime.tm_year = tmp.st_year - 1900;  // 年份需要减去 1900
            userTime.tm_mon = tmp.st_month - 1;     // 月份需要减去 1
            if(tmp.st_month<1 || tmp.st_month>11) flag = false;
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
                cout << "\033[96m" << "时间错误！" << "\033[0m" << endl;
                win("Invalid time!");
                flag = false;
                exit(0);
            }

            for(iter = list.begin(); iter != list.end(); iter ++) {
                if(iter->second.st_day == tmp.st_day && iter->second.st_month == tmp.st_month 
                && iter->second.st_year == tmp.st_year && iter->second.st_hour == tmp.st_hour 
                && iter->second.st_min == tmp.st_min) {
                    cout << "\033[96m" << "任务开始时间重合!" << "\033[0m" << endl;
                    flag = false;
                    exit(0);
                    break;
                }
            }
            if(flag) exit(0);
        }

        while(true) {
            // cout << "\033[96m" << "请输入提醒时间(顺序为:年 月 日 时 分, 示例:2023 10 08 10 00):" << "\033[0m" << endl;
            tmp.rm_year = atoi(_argv[12]);
            tmp.rm_month = atoi(_argv[13]);
            tmp.rm_day = atoi(_argv[14]);
            tmp.rm_hour = atoi(_argv[15]);
            tmp.rm_min= atoi(_argv[16]);
            time_t currentTime = time(NULL);
            struct tm userTime;

            bool flag = true;

            userTime.tm_year = tmp.rm_year - 1900;  // 年份需要减去 1900
            userTime.tm_mon = tmp.rm_month - 1;     // 月份需要减去 1
            if(tmp.rm_month<1 || tmp.rm_month>11) flag = false;
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
                cout << "\033[96m" << "时间错误!" << "\033[0m" << endl;
                win("Invalid time!");
                exit(0);
            }
        }

        tmp.reminded = false;

        id ++;

        for(iter = list.begin(); iter != list.end(); iter ++) {
            while(iter->first == id)
                id++;
        }

        
        list.insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.tp].insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.pr].insert(map<ll, task>::value_type(id, tmp));
        
        break;
    }

    write();
    exit(0);
}

//在程序中删除任务
void dele() {
    // read();
    char nameGet[20];
    ll idGet;
    while(true) {
        cout << "\033[96m" << "输入您想要删除的任务的名称: " << "\033[0m" << endl;
        cin  >> nameGet;

        map<ll, task>::iterator iter;
        bool flag = true;
        for(iter = list.begin(); iter != list.end(); iter ++) {
            if(strcmp(iter->second.taskName, nameGet) == 0) {
                cout << "\033[96m" << "已找到该任务" << "\033[0m" << endl;
                idGet = iter->first;
                flag = false;
                break;
            }
        }
        if(flag) {
            cout << "\033[96m" << "无法找到该任务" << "\033[0m" << endl;
            win("Task not found!");
        }
        else {
            list.erase(iter);
            cls[iter->second.pr].erase(idGet);
            cls[iter->second.tp].erase(idGet);
        }
        
        cout << "\033[96m" << "是否继续删除任务? Y or n: " << "\033[0m" << endl;
        char conti[10];
        cin >> conti;
        if(strcasecmp("y", conti) != 0) break;
    }
    write();
}

//在命令行直接删除任务
void _dele() {
    // read();
    char nameGet[20];
    ll idGet;
    while(true) {
        strcpy(nameGet, _argv[4]);

        map<ll, task>::iterator iter;
        bool flag = true;
        for(iter = list.begin(); iter != list.end(); iter ++) {
            if(strcmp(iter->second.taskName, nameGet) == 0) {
                cout << "\033[96m" << "已找到该任务" << "\033[0m" << endl;
                idGet = iter->first;
                flag = false;
                break;
            }
        }
        if(flag) {
            cout << "\033[96m" << "无法找到该任务" << "\033[0m" << endl;
            win("Task not found!");
            exit(0);
        }
        else {
            list.erase(iter);
            cls[iter->second.pr].erase(idGet);
            cls[iter->second.tp].erase(idGet);
        }
        break;
    }
    write();
    exit(0);
}