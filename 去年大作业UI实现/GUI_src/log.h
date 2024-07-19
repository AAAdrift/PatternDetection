#ifndef LOG_H
#define LOG_H

#include <QWidget>

namespace Ui {
class Log;
}

class Log : public QWidget
{
    Q_OBJECT

public:
    explicit Log(QWidget *parent = nullptr);
    ~Log();

private slots:
    void on_pushButton_clicked();
    bool check(std::string id);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::Log *ui;
};

#endif // LOG_H
