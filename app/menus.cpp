#include <QtWidgets>

void setup_menus (QMainWindow* window, QApplication* app)
{
    // File menu
    QAction *new_file = new QAction("&New", window);
    QAction *open = new QAction("&Open", window);
    QAction *quit = new QAction("&Quit", window);
    new_file->setShortcut(window->tr("CTRL+N"));
    open->setShortcut(window->tr("CTRL+O"));
    quit->setShortcut(window->tr("ALT+F4"));
    QMenu* file;
    file = window->menuBar()->addMenu("&File");
    file->addAction(new_file);
    file->addAction(open);
    file->addSeparator();
    file->addAction(quit);
    // TODO: fix this
    //connect(quit, SIGNAL(triggered()), app, SLOT(quit()));
}
