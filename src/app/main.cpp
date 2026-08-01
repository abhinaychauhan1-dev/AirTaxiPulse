#include <QDir>
#include <QCursor>
#include <QDebug>
#include <QGuiApplication>
#include <QObject>
#include <QScopedPointer>
#include <QScreen>
#include <QWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#ifdef AIRTAXI_HAS_QT_MQTT
#include "../services/MqttFlightTelemetryService.h"
#endif
#include "../viewmodel/AirTaxiModuleRegistry.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    const bool useMqttAsPrimary = qEnvironmentVariableIntValue("AIRTAXI_USE_MQTT") == 1;
    QObject *mqttTelemetryContext = nullptr;
    QScopedPointer<AirTaxiModuleRegistry> modules;

#ifdef AIRTAXI_HAS_QT_MQTT
    MqttFlightTelemetryService mqttTelemetry;
    const int envMaxParsers = qEnvironmentVariableIntValue("AIRTAXI_MQTT_MAX_CONCURRENT_PARSERS");
    const int envMaxPending = qEnvironmentVariableIntValue("AIRTAXI_MQTT_MAX_PENDING_MESSAGES");
    const bool envAsyncPayloadLog = qEnvironmentVariableIntValue("AIRTAXI_MQTT_ASYNC_LOG") == 1;

    mqttTelemetry.setBrokerHost(QStringLiteral("127.0.0.1"));
    mqttTelemetry.setBrokerPort(1883);
    mqttTelemetry.setTopicFilter(QStringLiteral("airtaxi/telemetry/#"));
    if (envMaxParsers > 0) {
        mqttTelemetry.setMaxConcurrentParsers(envMaxParsers);
    }
    if (envMaxPending > 0) {
        mqttTelemetry.setMaxPendingMessages(envMaxPending);
    }
    mqttTelemetry.setAsyncPayloadLoggingEnabled(envAsyncPayloadLog);
    mqttTelemetry.connectToBroker();
    mqttTelemetryContext = &mqttTelemetry;

    if (useMqttAsPrimary) {
        modules.reset(new AirTaxiModuleRegistry(mqttTelemetry, mqttTelemetry));
    } else {
        modules.reset(new AirTaxiModuleRegistry());
    }
#else
    if (useMqttAsPrimary) {
        qWarning() << "AIRTAXI_USE_MQTT=1 requested, but Qt MQTT is unavailable in this Qt kit.";
    }
    modules.reset(new AirTaxiModuleRegistry());
#endif

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("airTaxiModules", modules.data());
    engine.rootContext()->setContextProperty("flightSimulation", modules->primaryFlight());
    engine.rootContext()->setContextProperty("mqttTelemetry", mqttTelemetryContext);
    engine.rootContext()->setContextProperty("mqttTelemetryAvailable", mqttTelemetryContext != nullptr);

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
                const int targetHeight = qMax(680, qRound(available.height() * 0.87));
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
