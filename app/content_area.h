#ifndef CB_CONTENTAREA_H
#define CB_CONTENTAREA_H

#include "toolbar.h"
#include "canvas.h"
#include "pallette.h"

class CB_ContentArea : public QWidget
{
public:
    CB_Toolbar* toolbar;
    CB_Pallette* pallette;
    CB_Canvas* canvas;

    CB_ContentArea();
};

#endif // CB_CONTENTAREA_H
