#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

int main (int argc, char **argv)
{
  QApplication app (argc, argv);

  QMainWindow main_window;
  main_window.resize(800, 500);
  main_window.setWindowTitle("CircuitBuilder 2");
  
  main_window.show();
  return app.exec();
}
