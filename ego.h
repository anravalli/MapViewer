#ifndef EGO_H
#define EGO_H

#include <QObject>
#include <QPointF>
#include "map.h"

class Ego: public QObject
{
    Q_OBJECT
public:
    Ego();
    void update(QPointF pos);

    QPointF pos() const;

    SegmentInfo map_info() const;

    Ego &operator = (const Ego& other);

    double heading() const;
    double speed() const;

    bool isValid(){
        return m_is_valid;
    }

public slots:
    void updateMapInfo(SegmentInfo info);

signals:
    void updated(QPointF pos);

private:
    bool m_is_valid = false;
    QPointF m_pos;
    SegmentInfo m_map_info;
    double m_heading;
    double m_speed;
};

#endif // EGO_H
