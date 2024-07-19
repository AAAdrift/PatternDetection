#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT
    friend class Warning;

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

private slots:
    void on_pushButton_clicked();
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::Form *ui;
};

#endif // FORM_H
