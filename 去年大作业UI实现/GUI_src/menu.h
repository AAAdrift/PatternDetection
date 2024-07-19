#ifndef MENU_H
#define MENU_H

#include <QWidget>

namespace Ui {
class Menu;
}

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void slot1();

    void on_pushButton_clicked();

private:
    Ui::Menu *ui;
};

#endif // MENU_H
