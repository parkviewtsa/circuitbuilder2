#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

#include "toolbar.h"
#include "content_area.h"
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

void crash (int code, int isFatal)
{
    if (isFatal) {
        if (QMessageBox(QMessageBox::Critical, tr("Fatal Error"),
            tr(get_error_string(code)), QMessageBox::Abort | QMessageBox::Retry)
            .exec() == QMessageBox::Retry) return 1;
        else qApp->exit(code);
    } else {
        if (QMessageBox(QMessageBox::Warning, tr("Error"),
            tr(get_error_string(code)), QMessageBox::OK | QMessageBox::Retry)
            .exec() == QMessageBox::Retry) return 1;
        else return 0;
    }
}
