#include <QApplication>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QString>
#include <QUrl>
#include <Qt>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/sniffles/main.qml"));

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
          QApplication::exit(-1);
        }
      },
      Qt::QueuedConnection);

  engine.load(url);

  return app.exec();
}
