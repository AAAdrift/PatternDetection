#include "deltask.h"
#include "task_manager.h"
#include "auxiliary.h"


Deltask::Deltask(QWidget *parent) :
    QWidget(parent)
{
    this -> setAutoFillBackground(true);
    QPalette p = this -> palette();
    QPixmap pix(":/photo.png");
    p.setBrush(QPalette::Window, QBrush(pix));
    this -> setPalette(p);

    QWidget* centralWidget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QLabel *label12 = new QLabel("Enter the information of the task.");
    layout->addWidget(label12);

    QLabel *label1 = new QLabel("taskname:");
    layout->addWidget(label1);
    nameLineEdit = new QLineEdit(this);
    layout->addWidget(nameLineEdit);

    QLabel *label11 = new QLabel(" ");
    layout->addWidget(label11);

    QPushButton* submitButton = new QPushButton("Submit", this);
    connect(submitButton, &QPushButton::clicked, this, &Deltask::submit);
    layout->addWidget(submitButton);
    setLayout(layout);
}


Deltask::~Deltask() {}

void Deltask::submit()
{
    std::string name = nameLineEdit->text().toStdString();

    const char* nameGet;
    ll idGet;
    while(true) {
        nameGet = name.c_str();

        map<ll, task>::iterator iter;
        bool flag = true;
        for(iter = list1.begin(); iter != list1.end(); iter ++) {
            if(iter->second.taskName == nameGet) {
                cout << "\033[96m" << "yes" << "\033[0m" << endl;
                idGet = iter->first;
                flag = false;
                break;
            }
        }
        if(flag) {
            cout << "\033[96m" << "no" << "\033[0m" << endl;
            Test = new QWidget(this, Qt::Dialog);
            Test->resize(100, 100);
            Test->show();
            QLabel* ttest = new QLabel(Test);
            ttest->resize(Test->width(), Test->height());
            ttest->setText("CANNOT FIND");
            ttest->show();
            nameLineEdit->clear();
            return;
        }
        else {
            list1.erase(iter);
            cls[iter->second.pr].erase(idGet);
            cls[iter->second.tp].erase(idGet);
        }
        break;
    }
    Auxiliary aux;
    aux.write();
    Test = new QWidget(this, Qt::Dialog);
    Test->resize(100, 100);
    Test->show();
    QLabel* ttest = new QLabel(Test);
    ttest->resize(Test->width(), Test->height());
    ttest->setText("Success!");
    ttest->show();
    //    exit(0);
    nameLineEdit->clear();

}
