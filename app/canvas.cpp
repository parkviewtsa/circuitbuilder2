#include <QtWidgets>

#include "canvas.h"
#include "../renderer/circuit-render.h"

CB_Canvas::CB_Canvas()
{
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

