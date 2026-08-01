#include <QDir>
#include <QCursor>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "../viewmodel/AirTaxiModuleRegistry.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    AirTaxiModuleRegistry modules;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("airTaxiModules", &modules);
    engine.rootContext()->setContextProperty("flightSimulation", modules.primaryFlight());

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    const QUrl url = QUrl::fromLocalFile(dir.filePath("src/view/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    if (!engine.rootObjects().isEmpty()) {
        QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().first());
        if (window) {
            QScreen *targetScreen = QGuiApplication::screenAt(QCursor::pos());
            if (!targetScreen) {
                targetScreen = QGuiApplication::primaryScreen();
            }

            if (targetScreen) {
                const QRect available = targetScreen->availableGeometry();
                const int targetWidth = qMax(1024, qRound(available.width() * 0.90));
                const int targetHeight = qMax(700, qRound(available.height() * 0.88));
                const int x = available.x() + (available.width() - targetWidth) / 2;
                const int y = available.y() + (available.height() - targetHeight) / 2;

                window->setWidth(targetWidth);
                window->setHeight(targetHeight);
                window->setX(x);
                window->setY(y);
            }
        }
    }

    return app.exec();
}
