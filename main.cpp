#include "widget.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
int main(int argc, char *argv[])
{

    QCoreApplication::setOrganizationName("StudioYa");
    QCoreApplication::setOrganizationDomain("ya.self-promo.ru");
    QCoreApplication::setApplicationName("Password generator");

    QApplication a(argc, argv);

    QTranslator translator;
#if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0)
    if(translator.load("qt_" + QLocale::system().name(), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        a.installTranslator(&translator);
#else
    if(translator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        a.installTranslator(&translator);
#endif


    Widget w;
    w.show();
    return a.exec();
}
