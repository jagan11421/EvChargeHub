
#include <QApplication>
#include <QMessageBox>

#include "ui/MainWindow.h"
#include "database/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("EVChargeHub");
    app.setApplicationName("EVChargeHub");

    QString error;

    if (!DatabaseManager::initialize(&error))
    {
        QMessageBox::critical(
            nullptr,
            "Database Error",
            "Could not initialize database:\n" + error
        );

        return 1;
    }

    MainWindow window;
    window.show();

    return app.exec();
}
