#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

void setup_menus (QMainWindow*, QApplication*); // En lieu of a header file

int main (int argc, char **argv)
{
  QApplication app (argc, argv);

  QMainWindow main_window;
  main_window.resize(800, 500);
  main_window.setWindowTitle("CircuitBuilder 2");
  setup_menus(&main_window, &app);
  
  main_window.show();
  return app.exec();
}

void quit ()
{
    exit(0);
}
