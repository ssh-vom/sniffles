#include "app/App.h"

#include "ui/MainWindow.h"

#include <QApplication>

namespace sniffles::app {
int App::Run(int argc, char *argv[]) {
  QApplication application(argc, argv);
  ui::MainWindow window;
  window.show();
  return application.exec();
}
} // namespace sniffles::app
