#include "MyGIS.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MyGIS w;
    w.showMaximized(); // 显示窗口最大化
    return a.exec();
}
