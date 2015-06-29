#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>

#include "include/SDL2/SDL.h"
#include "toolbar.h"
#include "content_area.h"
#include "../renderer/circuit-render.h"
#include "../circuit/circuit.h"
#include "../error.h"
void setup_menus (QMainWindow*); // En lieu of a header file

int main (int argc, char **argv)
{
  crInit();
  QApplication app (argc, argv);

  QMainWindow main_window;
  main_window.resize(800, 500);
  main_window.setWindowTitle("CircuitBuilder 2");
  setup_menus(&main_window);

  CB_ContentArea content_area;
  main_window.setCentralWidget(&content_area);
  main_window.show();
  int status = app.exec();
  crQuit();
  return status;
}

int crash (int code, int isFatal)
{
  if (isFatal) {
    if (QMessageBox(QMessageBox::Critical, QMainWindow::tr("Fatal Error"),
      QMainWindow::tr(get_error_string(code)), QMessageBox::Abort | QMessageBox::Retry)
      .exec() == QMessageBox::Retry) return 1;
    else qApp->exit(code);
  } else {
    if (QMessageBox(QMessageBox::Warning, QMainWindow::tr("Error"),
      QMainWindow::tr(get_error_string(code)), QMessageBox::Ok | QMessageBox::Retry)
      .exec() == QMessageBox::Retry) return 1;
    else return 0;
  }
}
