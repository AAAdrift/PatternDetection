#include <algorithm>
#include <SFML/Graphics.hpp>
using namespace std;
#include "task_manager.h"

//按时间排序时的sort函数中cmp函数的重载
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
    if (y != yy) return (y < yy);
    if (m != mm) return (m < mm);
    if (d != dd) return (d < dd);
    if (hour != hhour) return (hour < hhour);
    if (min != mmin) return (min < mmin);
    return a.second.pr <= b.second.pr;
}

//按时间排序函数
vector<pair<ll, task> > list_sort() {
    vector <pair<ll, task> > v(list.begin(), list.end());
    sort(v.begin(), v.end(), cmp);
    return v;
}

//弹出窗口函数
void win(string s)
{
    sf::RenderWindow window(sf::VideoMode(400, 200), "");

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i windowPosition((desktop.width - window.getSize().x) / 2, (desktop.height - window.getSize().y) / 2);
    window.setPosition(windowPosition);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "加载字体失败！" << endl;
        return;
    }

    sf::Text errorMessage;
    errorMessage.setFont(font);
    errorMessage.setCharacterSize(20);
    errorMessage.setFillColor(sf::Color::White);
    errorMessage.setString(s);
    errorMessage.setPosition(20, 50);

    sf::RectangleShape buttonShape;
    buttonShape.setSize(sf::Vector2f(100, 40));
    buttonShape.setFillColor(sf::Color::Red);
    buttonShape.setPosition(150, 120);

    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setCharacterSize(16);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setString("OK");
    buttonText.setPosition(170, 128);

    bool showErrorDialog = true;

    sf::Clock timer; // 创建计时器
    sf::Time elapsed;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    if (buttonShape.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                        showErrorDialog = false;
                }
            }
        }

        elapsed = timer.getElapsedTime(); // 获取经过的时间

        if (elapsed.asSeconds() >= 10) // 判断时间是否超过 10 秒
        {
            showErrorDialog = false; // 关闭报错弹窗
        }

        window.clear(sf::Color::Black);

        if (showErrorDialog)
        {
            window.draw(errorMessage);
            window.draw(buttonShape);
            window.draw(buttonText);
        }
        else return;
        window.display();
    }

    return;
}

//读取文件函数
void read() {
    ifstream fr;

    //以可读可写方式打开存放用户任务日程的文件，若不存在则会以用户名创建
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
                cout << "\033[96m" << "ERROR! Enter again!" << "\033[0m" << endl;
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
        list.insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.tp].insert(map<ll, task>::value_type(id, tmp));
        cls[tmp.pr].insert(map<ll, task>::value_type(id, tmp));

    }
    fr.close();

    cin.clear();
}

//将新创建的任务写入文件
void write() {

    ofstream fo;
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
    for(iter = list.begin(); iter != list.end(); iter ++) {
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

void showhelp()
{
    cout << "\033[96m" << "本软件有两种使用方式：" << endl;
    cout << "第一种，直接使用命令行操作，格式为：\" ./task_master [用户名] [密码] [命令] [参数] \" " << endl;
    cout << "  其中，命令及其参数为可以为\" addtask [任务名] [优先级] [类型] [启动时间] [提醒时间],\n  showtask（默认以提醒时间的先后顺序展示）,\n  showtask [数字](1-low, 2-middle, 3-high, 4-emergent, 5-study, 6-play, 7-life, 8-work),\n  showtask [日期，如2023 7 10],\n  showtask [星期几，如Mon] deltask [任务名] \" " << endl;
    cout << "第二种方式为，进入软件操作，格式为：\" ./task_master run \"之后进入程序，根据指示操作 " << "\033[0m" << endl;
}