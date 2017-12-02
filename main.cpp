#include "trackingwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TrackingWindow w;
    w.show();

    return a.exec();
}
