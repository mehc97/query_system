#include "equipment_system.h"
#include "stdafx.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    equipment_system w;
    w.show();
    return a.exec();
}
