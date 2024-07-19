#ifndef WARNING_H
#define WARNING_H

#include <QWidget>

namespace Ui {
class Warning;
}

class Warning : public QWidget
{
    Q_OBJECT

    friend class Form;
    friend class Log;
public:
    explicit Warning(QWidget *parent = nullptr);
    ~Warning();

private:
    Ui::Warning *ui;
    void warning(QString);
};

#endif // WARNING_H
