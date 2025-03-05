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

public slots:
    void updateMapInfo(SegmentInfo info);

signals:
    void updated(QPointF pos);

private:
    QPointF m_pos;
    SegmentInfo m_map_info;
};

#endif // EGO_H
