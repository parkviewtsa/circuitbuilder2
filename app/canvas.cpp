#include <QtWidgets>

#include "canvas.h"
#include "../renderer/circuit-render.h"
#include "../error.h"

CB_Canvas::CB_Canvas()
{
    try_init:
    // crInit returns 1 if something went wrong; crash returns 1 if the user
    // chose to retry and exits otherwise
    if (crInit()) if (crash(CB_ERR_SDL_INIT, FATAL)) goto try_init;

    QSize s = size();
    crIndex w = s.width();
    crIndex h = s.height();
    delete &s;

    void* img_buf = 0;
    crResize(w, h);
    crDraw();
    crGetImg(&img_buf, &actual_width, &actual_height);
    if (img_buf)
        image = QImage((uchar*)img_buf, (int)w, (int)h, (int)w*3, QImage::Format_RGB888);
    else
}

QSize CB_Canvas::minimumSizeHint ()
{
    return QSize(500, 300);
}

void CB_Canvas::resizeEvent (void)
{
    QSize s = size();
    crIndex w = s.width();
    crIndex h = s.height();
    delete &s;

    void* img_buf = 0;
    crResize(w, h);
    crDraw();
    crGetImg(&img_buf, &w, &h);
    if (img_buf)
        image = QImage((uchar*)img_buf, (int)w, (int)h, (int)w*3, QImage::Format_RGB888);
}

void CB_Canvas::paintEvent (void)
{
    crDraw();
    painter.drawImage(rect(), image);
}
