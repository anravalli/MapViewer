#ifndef EGO_H
#define EGO_H

#include <QPointF>

class Ego
{
public:
    Ego();
    void update(QPointF pos);

    QPointF pos() const;

private:
    QPointF m_pos;
};

#endif // EGO_H
