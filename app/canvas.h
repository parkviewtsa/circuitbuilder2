#ifndef CB_CANVAS_H
#define CB_CANVAS_H

class CB_Canvas : public QWidget
{
public:
    CB_Canvas ();
    CB_Canvas* init (char*)
    QSize minimumSizeHint ();
    void resizeEvent ();
    void paintEvent ();

private:
    QPainter painter;
    QImage image;
    cbHeader* circuit_header;
};

#endif // CB_CANVAS_H
