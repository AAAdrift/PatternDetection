#ifndef RM_THREAD_H
#define RM_THREAD_H
#include<QThread>
#include<QObject>


class Rm_thread:public QThread
{
    Q_OBJECT
public:
    explicit Rm_thread(QObject *parent = nullptr);
protected:
    void run();//对父类的run函数进行重写
private:
    //扫描是否有需要提醒的任务并保存到全局变量rm_list中
    void check_remind();
signals:
    //发送“存在需提醒任务”的信号函数
    void sgn();
};

#endif // RM_THREAD_H
