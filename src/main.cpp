#include <QGuiApplication>
#include <QTranslator>
#include <QLibraryInfo>

#include <QQmlApplicationEngine>
#include <QtQml/QQmlComponent>

#include "multiwriter.h"

#include "devicesmodel.h"

int main(int argc, char *argv[])
{
    qputenv("XDG_CURRENT_DESKTOP", "KDE");
    QCoreApplication::setOrganizationName("lemuri");
    QCoreApplication::setOrganizationDomain("lemuri.org");
    QCoreApplication::setApplicationName("lemuri-multi-writer");
    QCoreApplication::setApplicationVersion("1.0");

    QGuiApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QCoreApplication::installTranslator(&qtTranslator);

    qmlRegisterType<DevicesModel>("DisksManager", 1, 0, "DevicesModel");

    QQmlApplicationEngine engine(QUrl(QLatin1String("qrc:/qml/main.qml")));

    return app.exec();
}
