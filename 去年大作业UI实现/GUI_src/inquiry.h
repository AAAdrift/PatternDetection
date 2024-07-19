#include "task_manager.h"
#ifndef INQUIRY_H
#define INQUIRY_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class Inquiry;
}

class Inquiry : public QWidget
{
    Q_OBJECT

public:
    explicit Inquiry(QWidget *parent = nullptr);
    ~Inquiry();

private:
    Ui::Inquiry *ui;
    void print_task(ll a, task b);

private slots:
    void print_with_date();
    void print_with_name();

};

#endif // INQUIRY_H
