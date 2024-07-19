#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <QWidget>

namespace Ui {
class Auxiliary;
}

class Auxiliary : public QWidget
{
    Q_OBJECT

public:
    explicit Auxiliary(QWidget *parent = nullptr);
    ~Auxiliary();
    void read();
    void write();

private:
    Ui::Auxiliary *ui;
};

#endif // AUXILIARY_H
