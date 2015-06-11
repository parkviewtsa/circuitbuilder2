#include <QtWidgets>

#include "canvas.h"
#include "../renderer/circuit-render.h"
#include "../error.h"

CB_Canvas::CB_Canvas()
{
    try_init:
    if (crInit()) { // true == error
        if (QMessageBox(QMessageBox::Critical, tr("Fatal Error"),
            tr("The application cannot load because SDL failed to initalize."),
            QMessageBox::Abort | QMessageBox::Retry).exec() == QMessageBox::Retry)
        {
            goto try_init;
        } else {
            qApp.exit(CB_ERR_SDL);
        }
    }

    QSize s = size();
    unsigned int w = s.width();
    unsigned int h = s.height();

    crResize(w, h);
    crDraw();
    image = QImage(crGetImgBuf(), w, h, w*3, QImage::Format_RGB888);
}

QSize CB_Canvas::minimumSizeHint ()
{
    return QSize(500, 300);
}

void CB_Canvas::resizeEvent (void)
{
    QSize s = size();
    unsigned int w = s.width();
    unsigned int h = s.height();

    crResize(w, h);
    crDraw();
    image = QImage(crGetImgBuf(), w, h, w*3, QImage::Format_RGB888);
}

void CB_Canvas::paintEvent (void)
{
    crDraw();
    painter.drawImage(rect(), image);
}

