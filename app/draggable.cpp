#include "draggable.h"

void CB_Draggable::mousePressEvent (QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    dragStartPosition = event->pos();
  }
}

void CB_Draggable::mouseMoveEvent (QMouseEvent* event)
{
  if ((event->buttons() == Qt::LeftButton)) {
    if ((event->pos() - dragStartPosition).manhattanLength() >= QApplication::startDragDistance()) {

      QDrag *drag = new QDrag(this);
      QMimeData *mimeData = new QMimeData("application/octet-stream", QByteArray(1, (char)type));

      drag->setMimeData(mimeData);

      Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
    }
  }
}
