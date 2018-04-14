//QT4:#include <QtGui/QApplication>
#include <QtWidgets/QApplication>
#include "mainusergui.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainUserGUI w;
    w.show();
    
    return a.exec();
}
