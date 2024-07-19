#ifndef SUCCEED_H
#define SUCCEED_H

#include <QWidget>

namespace Ui {
class succeed;
}

class succeed : public QWidget
{
    Q_OBJECT

public:
    explicit succeed(QWidget *parent = nullptr);
    ~succeed();

private:
    Ui::succeed *ui;
};

#endif // SUCCEED_H
