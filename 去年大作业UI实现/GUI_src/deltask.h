#ifndef DELTASK_H
#define DELTASK_H

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
class Deltask;
}

class Deltask : public QWidget
{
    Q_OBJECT

public:
    explicit Deltask(QWidget *parent = nullptr);
    ~Deltask();

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

#endif // DELTASK_H
