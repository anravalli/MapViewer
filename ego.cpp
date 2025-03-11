#include "ego.h"
#include <iostream>
#include <iomanip>

Ego::Ego()
{

}

void Ego::update(QPointF pos)
{
    std::cout << "New position (lat,lon): " << std::fixed << std::setprecision(8) << pos.y() << "," << pos.x() << std::endl;
    m_pos = pos;
    m_map_info.street_name = "";
    emit(updated(pos));
}

QPointF Ego::pos() const
{
    return m_pos;
}

void Ego::updateMapInfo(SegmentInfo info)
{
    std::cout << "New map info received - street name is: " << info.street_name.toStdString() << std::endl;
    m_map_info = info;
}

SegmentInfo Ego::map_info() const
{
    return m_map_info;
}
