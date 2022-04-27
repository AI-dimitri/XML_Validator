#include <QtWidgets>

#include "MyValdator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyValidator* wgt = new MyValidator;
    wgt->show();

    return a.exec();
}
