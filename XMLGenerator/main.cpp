#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QFile File("C:\\Users\\praveense\\Desktop\\darkorange.stylesheet.txt");
//    File.open(QFile::ReadOnly);
//    QString StyleSheet = QLatin1String(File.readAll());

//    a.setStyleSheet(StyleSheet);
    QApplication::setStyle(QStyleFactory::create("fusion"));
    MainWindow w;
    w.show();

    return a.exec();
    return 0;
}
