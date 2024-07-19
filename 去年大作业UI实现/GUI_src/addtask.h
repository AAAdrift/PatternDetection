#ifndef ADDTASK_H
#define ADDTASK_H

#include <QWidget>
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QApplication>
#include <qlabel.h>
#include <cstring>

namespace Ui {
class Addtask;
}

class Addtask : public QWidget
{
    Q_OBJECT

public:
    explicit Addtask(QWidget *parent = nullptr);
    ~Addtask();

private slots:
    void submit();

private:
    QLineEdit* nameLineEdit;
    QLineEdit* prioLineEdit;
    QLineEdit* typeLineEdit;
    QLineEdit* yearLineEdit;
    QLineEdit* monLineEdit;
    QLineEdit* dayLineEdit;
    QLineEdit* hourLineEdit;
    QLineEdit* minLineEdit;
    QWidget* Test;
};


#endif // ADDTASK_H
