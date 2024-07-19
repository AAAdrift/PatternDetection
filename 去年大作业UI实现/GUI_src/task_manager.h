#ifndef T_M_H
#define T_M_H
#define ll long long
    #include<cstring>
    #include<iostream>
    #include<vector>
    #include<map>
    #include<fstream>
    #include<time.h>

    using namespace std;

    //定义优先级、任务类型对应的数值以方便操作
    enum prior{low=1, middle, high, emergent};
    enum type{study=5, play, life, work};

    extern int _argc;
    extern char **_argv;

    //定义任务
    struct task{
        string taskName;//任务名称
        prior pr;//优先级
        type tp;//任务类型
        int st_year, st_month, st_day, st_hour, st_min;//开始年、月、日、时、分
        //此处保留upgrade的可能
        int rm_year, rm_month, rm_day, rm_hour, rm_min;//提醒年、月、日、时、分
        bool reminded;
    };
    
    //list是加载得到的任务列表，cls是按不同任务属性保存的任务，如cls[low]即cls[1]中保存类型为low的所有任务，按时间排序
    extern map<ll, task> list1, cls[9];
    extern vector<pair<ll, task> > rm_list;
    extern string id_present;//当前用户名


#endif
