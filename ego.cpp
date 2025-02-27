#include "ego.h"
#include <iostream>

Ego::Ego()
{

}

void Ego::update(QPointF pos)
{
    // QPointF min = m_map->top_left();
    // double x = (coord.x()-min.x())*m_w_scale*m_zoom;
    // double y = m_map->height() - (coord.y() - min.y());
    // y = y*m_h_scale*m_zoom;

    std::cout << "New ego x: " << pos.x() << ", y: " << pos.y() << std::endl;
    m_pos = pos;
}

QPointF Ego::pos() const
{
    return m_pos;
}
