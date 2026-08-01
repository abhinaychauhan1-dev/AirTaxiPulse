#include <QDir>
#include <QGuiApplication>
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

    return app.exec();
}
