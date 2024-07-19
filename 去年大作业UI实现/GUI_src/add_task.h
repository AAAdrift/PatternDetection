#ifndef ADD_TASK_H
#define ADD_TASK_H

#include <QWidget>

namespace Ui {
class Add_task;
}

class Add_task : public QWidget
{
    Q_OBJECT

public:
    explicit Add_task(QWidget *parent = nullptr);
    ~Add_task();

private:
    Ui::Add_task *ui;
private slots:
    void submit();
};

#endif // ADD_TASK_H
