#ifndef CB_CANVAS_H
#define CB_CANVAS_H

class CB_Canvas : public QWidget
{
public:
    CB_Canvas();
    QSize minimumSizeHint ();
    void resizeEvent ();
    void paintEvent ();

private:
    QPainter painter;
    QImage image;
};

#endif // CB_CANVAS_H
