#include "bittle.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Bittle b;
    b.show();
    
    return a.exec();
}
