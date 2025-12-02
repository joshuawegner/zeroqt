#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFont>
#include <QFontDatabase>

#include "macrocontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("MacroPad");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("MacroPad");
    app.setOrganizationDomain("macropad.local");

    // Enable Material theme for touchscreen-friendly UI
    QQuickStyle::setStyle("Material");

    // Create the macro controller
    MacroController controller;
    
    // Initialize the controller (Bluetooth HID + config)
    if (!controller.initialize()) {
        qWarning() << "Failed to initialize MacroController - running in demo mode";
    }

    QQmlApplicationEngine engine;
    
    // Expose the controller to QML
    engine.rootContext()->setContextProperty("macroController", &controller);

    // Load main QML file
    const QUrl url(QStringLiteral("qrc:/MacroPad/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    
    engine.load(url);

    return app.exec();
}

