#ifndef MAPVIEWER_H999
#define MAPVIEWER_H

#include <cstring>
#include <iostream>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>

class Map;
class Ego;

class MapViewer: public QWidget
{
    Q_OBJECT
public:
    MapViewer(QWidget *_parent, Map *map, Ego *ego, Ego *other);
    void reset();
    void centerView();
    void centerToEgo();
    void centerToOther();

    void setMarker_en(bool newMarker_en);
    void setMarker_label_en(bool newMarker_label_en);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPointF toPointInScreen(QPointF coord);
    QPointF toPointInMap(QPointF sp);
    void updateScale(QSize new_size);
    void centerTo(const QPointF p);

    enum MarkerType {
        first,
        second,
        lastBut,
        last,
        other
    };

    void paintMarker(QPainter &painter, QPointF pos,
                      MarkerType type, QColor pen, QColor brush);

    bool m_pan_en = false;

    bool m_marker_en = true;
    bool m_marker_label_en = false;

    double m_w_scale = 0;
    double m_h_scale = 0;

    QPointF m_center;
    QPointF m_reference;
    QPointF m_delta;
    double m_zoom = 1;

    Map *m_map;
    Ego *m_ego;
    Ego *m_other;
};

#endif // MAPVIEWER_H
