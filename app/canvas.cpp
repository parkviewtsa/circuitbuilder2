#include <QtWidgets>

#include "canvas.h"
#include "../renderer/circuit-render.h"
#include "../error.h"
#include "../circuit/circuit.h"

CB_Canvas* CB_Canvas::init (char* filename = "")
{
  setAcceptDrops(true);

  circuit_header = malloc(sizeof(cbHeader));
  circuit_init(circuit_header, filename);

  QSize s = size();
  crIndex w = s.width();
  crIndex h = s.height();

  void* img_buf = 0;
  crResize(w, h);
  crDraw();
  get_buf:
  crGetImg(&img_buf, &actual_width, &actual_height);
  if (img_buf) {
    image = QImage((uchar*)img_buf, (int)w, (int)h, (int)w*3, QImage::Format_RGB888);
    return this;
  } else if crash(CR_ERR_NO_BUF, FATAL) {
    goto get_buf;
  } else {
    delete this;
    return NULL;
  }
}

CB_Canvas::~CB_Canvas ()
{
  cb_Close(circuit_header);
  crDropAll();
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

void CB_Canvas::dropEvent (QDropEvent* drop)
{
  int component_type = (int) drop->mimeData()->data("application/octet-stream").at(0);
  QPoint loc = drop->pos();
  cbComponent* comp = cb_CreateComponent(component_type);
  crScalar x = (crScalar) loc.x() / size().width();
  crScalar y = (crScalar) loc.y() / size().height();
  crScreenToWorld(&x, &y);
  comp->draw_item->posx = x;
  comp->draw_item->posy = y;
}
