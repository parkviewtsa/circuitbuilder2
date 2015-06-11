#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

#include "toolbar.h"
#include "content_area.h"
void setup_menus (QMainWindow*); // En lieu of a header file

int main (int argc, char **argv)
{
  QApplication app (argc, argv);

  QMainWindow main_window;
  main_window.resize(800, 500);
  main_window.setWindowTitle("CircuitBuilder 2");
  setup_menus(&main_window);

  CB_ContentArea content_area;
  main_window.setCentralWidget(&content_area);
  
  main_window.show();
  return app.exec();
}
