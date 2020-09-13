#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <clocale>
#include <QTextCodec>
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Wydatki");
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if(!QDir().mkpath(path)) {
        cerr << "Failed to create data directory in " << path.toStdString() << endl;
        return 0;
    }
    setlocale(LC_ALL, "C");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("latin2"));
    MainWindow w;
    w.show();

    return a.exec();
}
