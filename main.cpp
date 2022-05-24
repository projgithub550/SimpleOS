#include "mainwindow.h"
#include <QApplication>

typedef struct Test {
    QString str;
    int a;
} test;

int main(int argc, char *argv[])
{
//    test* tst = new test;
//    tst->str = "1234";
//    tst->a = 257;
//    int len = sizeof(*tst);
//    qDebug()<<"len:"<<len;
//    FILE* disk1 = fopen("test.bin", "rb+");
//    fwrite((char*)tst, 64, 1, disk1);

//    test* tst2 = (test*)malloc(64);
//    fseek(disk1,0,0);
//    fread((char*)tst2, 64, 1, disk1);
//    fclose(disk1);

//    qDebug()<<tst2->str<<" "<<tst2->a;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
