#include <QtWidgets>

#include "toolbar.h"
#include "content_area.h"

CB_ContentArea::CB_ContentArea()
{
    toolbar = new CB_Toolbar();
    //pallette = new CB_Pallette();
    //canvas = new CB_Canvas();

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(toolbar, 0, 0, 1, 2);
    //layout->addWidget(pallette, 1, 0);
    //layout->addWidget(canvas, 1, 1);

    setLayout(layout);
}

