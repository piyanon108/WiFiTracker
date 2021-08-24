#include <QCoreApplication>
#include <MainWindow.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MainWindow exec;
    return a.exec();
}
