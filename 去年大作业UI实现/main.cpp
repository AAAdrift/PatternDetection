#include <algorithm>
#include <fstream>
#include <functional>
#include <utility>
#include <thread>
#include <mutex>
#include <unistd.h>

using namespace std;
#include "task_manager.h"

map<ll, task> list, cls[9];//list是加载得到的任务列表，cls是按不同任务属性保存的任务，如cls[low]即cls[1]中保存类型为low的所有任务，按时间排序
string id_present = "";//当前用户名
int _argc;//操作数
char **_argv;//操作
mutex mtx;
void add();
void _add();
void dele();
void _dele();
void inquiry(string s, int mode);
void read();
void write();
void win(string s);
char* type_itoc(type );
char* prior_itoc(prior );
void reminder();
void showhelp();

void showbegin()//进入软件程序的初始提醒
{
    cout << "\033[96m" << "欢迎使用日程管理大师！" << endl;
    cout << "如果要登录，请输入log in，如果要注册，请输入sign up" << "\033[0m" << endl;
}

void sign_up()
{
    string id, password;
    cout << "\033[96m" << "请输入您的用户名：" << "\033[0m" << endl;

    fstream fo;
    bool flag = false;

    //判断用户名是否已经被使用
    while(true)
    {
        cin >> id;
        flag = false;
        fo.open("users.txt", ios::in);
        while(!flag && !fo.eof())
        {
            string tmp_id;
            size_t tmp_word;
            fo >> tmp_id >> tmp_word;
            if(id == tmp_id) flag = true;
        }
        fo.close();
        if(flag)
        {
            cout << "\033[96m" << "这个用户名已经被使用了，请换一个!" << endl;
            cout << "如果您已经注册，请输入quit退出返回上一步！" << "\033[0m" << endl;
            string tmp;
            cin >> tmp;
            if(tmp == "quit") return;
        }
        else break;
    }

    fo.open("users.txt", ios::app);
    fo << id << " ";

    cout << "\033[96m" << "输入成功" << endl;
    cout << "现在请输入您的密码：" << "\033[0m" << endl;
    cin >> password;

    //调用STL自带的hash进行密码加密
    hash<string> change;
    size_t word = change(password);
    fo << word << endl;
    fo.close();
    cout << "\033[96m" << "注册成功!" << "\033[0m" << endl;
}

//查找用户名是否存在
bool check(string id)
{
    fstream fi;
    bool flag = false;
    fi.open("users.txt", ios::in);

    //若users.txt不存在则创建并返回
    if(!fi.is_open()) {
        fi.open("users.txt", ios::app);
        fi.close();
        return false;
    }

    while(!flag && !fi.eof())
    {
        string tmp_id;
        size_t tmp_word;
        fi >> tmp_id >> tmp_word;
        if(id == tmp_id) flag = true;
    }
    fi.close();
    return flag;
}

bool log_in(string id, string password)
{
    fstream fi;
    fi.open("users.txt", ios::in);
    hash<string> change;
    size_t word = change(password);

    //读入用户名单
    map<string, size_t> info;
    while(!fi.eof())
    {
        string tmp_id;
        size_t tmp_word;
        fi >> tmp_id >> tmp_word;
        info.insert(make_pair(tmp_id, tmp_word));
    }
    map<string, size_t>::iterator it = info.find(id);

    //判断密码是否正确
    if(word != it -> second)
    {
        return false;
    }
    fi.close();
    cout << "\033[96m" << "登录成功！" << "\033[0m" << endl;
    return true;
}

//线程一
void thrd1() {
    while(1) {
        //注册与登录
        showbegin();
        string cmd;
        bool flag = false;
        string password;

        //如果是以run的形式进入程序再管理任务
        if(_argc == 2 && !strcmp(_argv[1], "run"))
            while(true) {
                getline(cin, cmd);
                if(cmd == "sign up") {
                    sign_up();
                    cout << "\033[96m" << "如果您想要立即登录，请输入log in" << "\033[0m" << endl;
                }
                if(cmd == "log in") {
                    cout << "\033[96m" << "请输入您的用户名：" << "\033[0m" << endl;
                    getline(cin, id_present);
                    while(!check(id_present)) {
                        cout << "\033[96m" << "用户名错误，请检查您的输入!" << endl;
                        cout << "您可以再次输入用户名或者输入sign up来创建用户" << "\033[0m" << endl;
                        string tmp_cmd;
                        //cin.ignore(1024, '\n');
                        getline(cin, tmp_cmd);
                        if(tmp_cmd == "sign up")
                        {
                            sign_up();
                        }
                        else id_present = tmp_cmd;
                    }
                    cout << "\033[96m" << "请输入您的密码" << "\033[0m" << endl;
                    cin >> password;
                    while(!log_in(id_present, password)) {
                        cout << "\033[96m" << "密码错误，请检查您的输入并再次输入" << "\033[0m" << endl;
                        cin >> password;
                    }
                    break;
                }
            }

        //如果是直接以命令行形式管理任务
        else {
            id_present = _argv[1];

            //登录账号
            if(!check(_argv[1])) {
                cout << "\033[96m" << "用户名错误，请检查您的输入!" << endl;
                exit(0);
            }
            if(!log_in(_argv[1], _argv[2])) {
                cout << "\033[96m" << "密码错误，请检查您的输入!" << "\033[0m" << endl;
                exit(0);
            }
        }

        //加载用户文件
        cout << "\033[96m" << "加载中......" << endl;
        read();
        cout << "加载完成！" << "\033[0m" << endl;

        cin.ignore(1024, '\n');
        //正常登录后开始操作
        if(_argc == 2 && !strcmp(_argv[1], "run"))
            while(true) {
                printf("\033[96m提示：命令的格式为[命令] ([参数])\n");
                printf("其中，命令及其参数为可以为：\n  addtask\n  showtask，接下来根据提示进行\n  showtask [数字](1-low, 2-middle, 3-high, 4-emergent, 5-study, 6-play, 7-life, 8-work),\n  showtask [日期，如2023 7 10],\n  showtask [星期几，如Mon]\n  deltask\n请输入您的命令:\n\033[0m");
                string s;

                //接受命令
                getline(cin, s);
                if(!strcasecmp(s.substr(0, 7).c_str(), "addTask")) {
                    mtx.lock();
                    add();
                    mtx.unlock();
                }else if (!strcasecmp(s.substr(0, 7).c_str(), "delTask")) {
                    mtx.lock();
                    dele();
                    mtx.unlock();
                }else if (!strcasecmp(s.substr(0, 8).c_str(), "showTask")) {
                    inquiry(s, 1);
                }else if (!strcasecmp(s.substr(0, 7).c_str(), "log out")) {
                    printf("\033[96m是否要登出: y / n ?\n\033[0m");
                    char ch;
                    cin >> ch;
                    mtx.lock();
                    if(ch == 'y') {
                        list.clear();
                        for(int i = 0; i < 8; ++ i) {
                            cls[i].clear();
                        }
                        printf("\033[96m您已登出。\n\033[0m");
                    }
                    mtx.unlock();
                    break;
                }else printf("\033[96m无效输入！\n\033[0m");
            }

        //直接命令行操作
        else {
            mtx.lock();
            if(!strcasecmp(_argv[3], "addTask")) {
                _add();
            }else if (!strcasecmp(_argv[3], "delTask")) {
                _dele();
            }else if (!strcasecmp(_argv[3], "showTask")) {
                string s;
                s = _argv[3];
                for(int i = 4; i < _argc; ++ i) {
                    s += " ";
                    s += _argv[i];
                }
                inquiry(s, 2);
                exit(0);
            }
            mtx.unlock();
            break;
        }
    }
}

//线程二：不断扫面任务以判断任务是否到达提醒时间
void thrd2() {
    while(true) {
        mtx.lock();
        reminder();
        mtx.unlock();
        sleep(1);
    }
}

int main(int argc, char*argv[]) {
    _argc = argc;
    _argv = argv;

    //抛出说明书
    if(argc == 2 && !strcmp(argv[1], "-h")) {
        showhelp();
        return 0;
    }

    //开启多线程任务
    else {
        thread thread1(thrd1);
        thread thread2(thrd2);

        // 主线程等待子线程完成
        thread1.join();
        thread2.join();
        
        return 0;
    }
}
