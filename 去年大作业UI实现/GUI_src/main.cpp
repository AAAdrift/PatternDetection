#include "mainwindow.h"
#include "task_manager.h"
#include "reminder.h"

#include <QApplication>

map<ll, task> list1, cls[9];
vector<pair<ll, task> > rm_list;
string id_present;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
