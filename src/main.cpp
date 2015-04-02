#include <QApplication>

#include "multiwriter.h"

int main(int argc, char *argv[])
{
    qputenv("XDG_CURRENT_DESKTOP", "KDE");
    QCoreApplication::setOrganizationName("lemuri");
    QCoreApplication::setOrganizationDomain("lemuri.org");
    QCoreApplication::setApplicationName("lemuri-multi-writer");
    QCoreApplication::setApplicationVersion("1.0");

    // TODO use libudisks-qt
    QCoreApplication::setSetuidAllowed(true);

    QApplication app(argc, argv);

    MultiWriter dialog;

    dialog.show();

    return app.exec();
}
