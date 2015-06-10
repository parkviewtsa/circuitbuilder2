#include <QtWidgets>
#include "toolbar.h"

CB_Toolbar::CB_Toolbar ()
{
    QPushButton* select_b = new QPushButton("Select");
    QPushButton* wire_b = new QPushButton("Draw Wire");
    QPushButton* delete_b = new QPushButton("Delete");
    QPushButton* pan_b = new QPushButton("Pan");
    QPushButton* zoom_b = new QPushButton("Zoom");
    QPushButton* export_b = new QPushButton("Export");

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(select_b);
    layout->addWidget(wire_b);
    layout->addWidget(delete_b);
    layout->addWidget(pan_b);
    layout->addWidget(zoom_b);
    layout->addWidget(export_b);

    setLayout(layout);
}

