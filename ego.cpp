#include "ego.h"
#include <iostream>
#include <iomanip>

static constexpr double PI = 3.14159265358979323846;
const constexpr double todeg(double r){return 80.0 / PI * r;}

Ego::Ego()
{

}

void Ego::update(QPointF pos)
{
    std::cout << "New position (lat,lon): " << std::fixed << std::setprecision(8) << pos.y() << "," << pos.x() << std::endl;
    m_pos = pos;
    m_map_info.street_name = "";
    m_is_valid = true;
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
    m_heading = info.bearing;
    std::cout << "\tlat,lon,heading: " << std::fixed << std::setprecision(8)
              << m_pos.y() << "," << m_pos.x() << "," << todeg(m_heading) <<  std::endl;
}

double Ego::speed() const
{
    return m_speed;
}

double Ego::heading() const
{
    return m_heading;
}

SegmentInfo Ego::map_info() const
{
    return m_map_info;
}

Ego &Ego::operator =(const Ego &other)
{
    this->m_pos = other.pos();
    this->m_heading = other.heading();
    this->m_speed = other.speed();
    emit updated(this->pos());
    return *this;
}
