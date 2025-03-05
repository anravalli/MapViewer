#include "ego.h"
#include <iostream>

Ego::Ego()
{

}

void Ego::update(QPointF pos)
{
    std::cout << "New ego x: " << pos.x() << ", y: " << pos.y() << std::endl;
    m_pos = pos;
    m_map_info.street_name = "pippo";
    emit(updated(pos));
}

QPointF Ego::pos() const
{
    return m_pos;
}

void Ego::updateMapInfo(SegmentInfo info)
{
    m_map_info = info;
}

SegmentInfo Ego::map_info() const
{
    return m_map_info;
}
