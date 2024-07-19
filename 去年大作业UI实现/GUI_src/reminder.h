#include"task_manager.h"
#ifndef REMINDER_H
#define REMINDER_H

#include <QWidget>

namespace Ui {
class Reminder;
}

class Reminder : public QWidget
{
    Q_OBJECT

public:
    explicit Reminder(QWidget *parent = nullptr);
    void print_remind_list();
    ~Reminder();

private:
    Ui::Reminder *ui;
    void print_task(ll a, task b);
};

#endif // REMINDER_H
