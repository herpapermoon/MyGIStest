#include "MyGIS.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MyGIS w;
    w.showMaximized(); // ��ʾ�������
    return a.exec();
}
