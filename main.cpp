#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("SOS");
    QCoreApplication::setApplicationName("Xeoma Detectors");
    QCoreApplication::setOrganizationDomain("sos-dn.com");
    QCoreApplication::setApplicationVersion("0.1");
    MainWindow w;
    w.show();
    return a.exec();
}
