#include <cstdio>
#include <algorithm>
#include <iomanip>
using namespace std;
#include "task_manager.h"

//将星期几对应数值，方便操作
map<string, int> wday{ {"Mon", 1}, {"Tue", 2}, {"Wed", 3}, {"Thu", 4}, {"Fri", 5}, {"Sat", 6}, {"Sun", 7}};
bool cmp(pair<ll, task> , pair<ll, task> );
vector<pair<ll, task> > list_sort();

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

void print_title() {
    printf("\033[93m查询结果：\n");
    printf("任务ID\t任务名称\t启动时间           \t提醒时间   \t类型\t优先级\033[0m\n");
}

void print_task(ll a, task b) {
     cout << "\033[96m" << a << "\t";
     cout << b.taskName << "        " << "\t" << b.st_year << "." << b.st_month << "." << b.st_day << " ";
     cout<< setw(2) << setfill('0') << b.st_hour << ":" << setw(2) << setfill('0') << b.st_min;
     cout << "       " << "\t" << b.rm_year << "." << b.rm_month << "." << b.rm_day << " ";
     cout << setw(2) << setfill('0') << b.rm_hour << ":" << setw(2) << setfill('0') << b.rm_min << "\t";
     cout << type_itoc(b.tp) << "\t" << prior_itoc(b.pr) << "\033[0m" << endl;
}

//默认以时间顺序展示任务日程
void print_with_time (vector<pair<ll, task> > x) {
    print_title();
    for(int i = 0; i < x.size(); ++ i) {
        print_task(x[i].first, x[i].second);
    }
    return;
}

//展示某天的任务日程
void print_with_date(int year, int month, int day){
    print_title();
    map<ll, task>::iterator iter;
    for(iter = list.begin(); iter != list.end(); ++iter) {
        ll a = iter->first;
        task b = iter -> second;
        if (b.st_year == year && b.st_month == month && b.st_day == day) {
            print_task(a, b);
        }
    }
}
void print_with_date() {
    int y, m, d;
    scanf("%d%d%d", &y, &m, &d);
    print_with_date(y, m, d);
}

//展示给定星期几的任务
//计算给定日期是周几
int return_weekday(int y,int m,int d)
{
    if( m == 1 || m == 2) {
        m += 12;
        y--;
    }
    //使用基姆拉尔森计算公式
    int week = ( d + 2 * m + 3 * ( m + 1 ) / 5 + y + y / 4 - y / 100 + y / 400 ) % 7 + 1;
    return week;
}

void print_with_weekday(string x) {
    if(!wday.count(x)) {
        printf("无效参数！\n");
        return;
    }
    print_title();
    int tg = wday[x];

    //找出是给定星期几的任务
    map<ll, task>::iterator iter;
    for(iter = list.begin(); iter != list.end(); iter++){
        ll a = iter -> first;
        task b = iter -> second;
        int wd = return_weekday(b.st_year, b.st_month, b.st_day);
        if (wd == tg)
            print_task(iter->first, iter->second);
    }
    return;
}
//外层函数
void print_with_weekday() {
    string x;
    cin.ignore();
    getline(cin, x);
    if(!wday.count(x)) {
        printf("Invalid Input!\n");
        return;
    }
    print_with_weekday(x);
}

//展示与给定优先级/任务类型相同的任务
void print_with_command(int cmd) {
    print_title();
    map<ll, task>::iterator iter;
    for(iter = cls[cmd].begin(); iter != cls[cmd].end(); ++iter) {
        print_task(iter->first, iter->second);
    }
}
//外层函数
void print_with_command(){
    int x;
    scanf("%d", &x);
    print_with_command(x);
}

//整体的查询函数
void inquiry(string arg, int mode) {
    vector<pair<ll, task> > sorted = list_sort();
    //+cout << arg <<endl;
    //若只输入了“inquiry”
    if(arg.find(' ') == arg.npos) {
        printf("默认按提醒时间的顺序输出所有任务：\n");

        //mode==2代表此时是直接通过命令行参数查询任务
        if(mode == 2){
            print_with_time(sorted);
            return;
        }

        //此时是通过循环来查询任务
        printf("y / n ?\n");
        char ch;
        cin >> ch;
        while(ch != 'y' && ch != 'n' ){
            printf("错误！请输入 y / n: \n");
            cin >> ch;
        }
        switch(ch){
            case 'y':
                print_with_time(sorted); cout<<endl;return;
            case 'n':
                break;
        }

        //打印某一日期的任务
        printf("提示：若要查询某一特定日期的任务，请按以下格式输入日期:\n2023 06 30\ny / n ?\n");
        cin >> ch;
        while(ch != 'y' && ch != 'n' ){
            printf("错误！请输入 y / n: \n");
            cin >> ch;
        }
        if(ch == 'y'){
            print_with_date();
            cout << endl;
            return;
        }

        //打印星期x的任务
        printf("若要查询某一个星期几的任务，请按以下格式输入: Mon / Tue / Wed / Thu / Fri/ Sat/ Sun\ny / n ?\n");
        cin >> ch;
        while(ch != 'y' && ch != 'n' ){
            printf("错误！请输入 y / n: \n");
            cin >> ch;
        }
        if(ch == 'y'){
            print_with_weekday();
            cout << endl;
            return;
        }

        //打印某一类型/优先级的任务
        printf("若要查询某一优先级的任务，请输入对应的数字:1 low, 2 middle, 3 high, 4 emergent.\n");
        printf("若要查询某一类型的任务,请输入对应的数字:5 study, 6 play, 7 life, 8 work.\ny / n ?\n");
        cin >> ch;
        while(ch != 'y' && ch != 'n' ){
            printf("错误！请输入 y / n: \n");
            cin >> ch;
        }
        if(ch == 'y'){
            print_with_command();
            cout << endl;
            return;
        }
    }else{
        //若inquiry后面自带查询的参数，如：inquiry Mon/inquiry 2023 06 30/...
        if(arg.size() == 10) {//打印某一类型的任务
            int cmd = arg[9] - 48;
            if(cmd<1 || cmd>8) {
                printf("无效参数！\n");
                return;
            }
            print_with_command(cmd);
            cout<<endl;
        }else if(arg.size() == 12) {//打印星期几的任务
            string x= arg.substr(9, 3);
            print_with_weekday(x);
            cout<<endl;
        }else{
            //打印某一日期的任务
            int y = 0, m = 0, d = 0;
            const char* tmp;
            arg = arg.substr(9, arg.size() - 8);
            tmp = arg.c_str();
            sscanf(tmp, "%d %d %d", &y, &m, &d);
            if(m < 1 || m > 12 || d < 1 || d > 31){
                printf("无效输入！\n");//参数无效
                return;
            }
            print_with_date(y, m, d);
            cout<<endl;
        }
    }
    return;
}