#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

int main (int argc, char **argv)
{
  QApplication app (argc, argv);

  QWidget main_window;
  main_window.resize(250, 150);
  main_window.setWindowTitle("CircuitBuilder 2");
  main_window.show();

  return app.exec();
}
