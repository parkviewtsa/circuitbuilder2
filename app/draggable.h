#ifndef DRAGGABLE_H
#define DRAGGABLE_H

class CB_Draggable : public QWidget
{
public:
  int type;
protected:
  void mousePressEvent (QMouseEvent*);
  void mouseMoveEvent (QMouseEvent*);
private:
  QPoint dragStartPosition;
};

#endif // DRAGGABLE_H
