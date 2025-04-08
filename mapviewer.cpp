#include "mapviewer.h"
#include "./ui_mainwindow.h"

#include <QMqttClient>
#include <QJsonDocument>
#include <QJsonObject>

#include "map.h"
#include "ego.h"

#include <QDebug>

MapViewer::MapViewer(QWidget *_parent, Map *map, Ego *ego, Ego* other):
    QWidget(_parent), m_map(map), m_ego(ego), m_other(other)
{
    std::cout << "MapViewer created" << std::endl;
    this->setParent(_parent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_center = this->rect().center();
    std::cout << "center is at:" << m_center.x() << ", " << m_center.y() << std::endl;
}

void MapViewer::reset()
{
    updateScale(this->size());
    m_center = this->rect().center();
    m_reference = QPointF(0,0);
    m_delta = QPointF(0,0);
    m_zoom = 1;
    update();
}

void MapViewer::centerView()
{
    centerTo(m_map->rect().center());
    update();
}

void MapViewer::centerToEgo()
{
    if(m_ego->isValid()){
        centerTo(m_ego->pos());
        update();
    }
}

void MapViewer::centerToOther()
{
    if(m_other->isValid()){
        centerTo(m_other->pos());
        update();
    }
}

void MapViewer::centerTo(const QPointF p)
{
    std::cout << "centering to point in map: " << p.x() << ", " << p.y() << std::endl;
    auto ps = toPointInScreen(p);
    std::cout << "--- converted point to screen: " << ps.x() << ", " << ps.y() << std::endl;
    m_delta = m_center-ps;
    std::cout << "--- new delta to apply: " << m_delta.x() << ", " << m_delta.y() << std::endl;
}

void MapViewer::paintMarker(QPainter &painter, QPointF pos,
                             MarkerType type, QColor pen, QColor brush)
{
    if(m_marker_en){
        painter.setPen(pen);
        painter.setBrush(brush);
        switch(type){
        case first:
        case second:
            painter.drawRect(QRectF(pos.x()-5,pos.y()-5,10,10));
            break;
        case lastBut:
            // painter.drawEllipse(pos, 5, 5);
            // break;
        case last:
        {
            QPointF tri[3] = {
                QPointF(pos.x(), pos.y()+5),
                QPointF(pos.x()+5, pos.y()-5),
                QPointF(pos.x()-5, pos.y()-5)
            };
            painter.drawPolygon(tri, 3);
        }
            break;
        default:
            painter.drawEllipse(pos, 3, 3);
            break;
        }
    }
    if(m_marker_label_en){
        painter.setPen(pen);
        painter.setBrush(brush);
        switch(type){
        case first:
            painter.drawText(QPointF(pos.x(),pos.y()-15), "S");
            break;
        case last:
            painter.drawText(QPointF(pos.x(),pos.y()-10), "E");
            break;
        }
    }
    return;
}

void MapViewer::setMarker_label_en(bool newMarker_label_en)
{
    m_marker_label_en = newMarker_label_en;
    update();
}

void MapViewer::setMarker_en(bool newMarker_en)
{
    m_marker_en = newMarker_en;
    update();
}



QPointF MapViewer::toPointInScreen(QPointF coord)
{
    QPointF screen_point;
    QPointF min = m_map->top_left();

    double x = (coord.x()-min.x())*m_w_scale*m_zoom;
    double y = (m_map->height() - (coord.y() - min.y()))*m_h_scale*m_zoom;

    screen_point.setX(x);
    screen_point.setY(y);
    return screen_point;
}


QPointF MapViewer::toPointInMap(QPointF sp)
{
    QPointF map_point;
    QPointF min = m_map->top_left();

    double lon = min.x() + sp.x()/(m_w_scale*m_zoom);
    double lat = m_map->height() - (sp.y()/(m_h_scale*m_zoom) - min.y());

    map_point.setX(lon);
    map_point.setY(lat);
    return map_point;
}

void MapViewer::updateScale(QSize new_size)
{
    m_w_scale = new_size.width()/m_map->width();
    m_h_scale = new_size.height()/m_map->height();
    std::cout << "New w_scale: " << m_w_scale << ", new h_scale: " << m_h_scale << std::endl;
}


void MapViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // std::cout << "center is at:" << m_center.x() << ", " << m_center.y() << std::endl;
    painter.translate(m_center+m_delta);
    // painter.translate(m_center - this->rect().center());
    // painter.translate(m_delta);

    enum Qt::GlobalColor color_array[13] = {Qt::white,
                                           Qt::red,
                                           Qt::green,
                                           Qt::blue,
                                           Qt::cyan,
                                           Qt::magenta,
                                           Qt::yellow,
                                           Qt::darkRed,
                                           Qt::darkGreen,
                                           Qt::darkBlue,
                                           Qt::darkCyan,
                                           Qt::darkMagenta,
                                           Qt::darkYellow};

    QPointF ps;
    int i = 0;

    painter.setFont(QFont("Arial", 14));

    for(auto street: m_map->streets()){
        QVector<QPointF> street_pline;

        for(int j=0; j<street.points.size(); j++ ){
            auto p = street.points[j];
            ps = toPointInScreen(p)-m_center;
            street_pline.append(ps);
            if(j==0){
                paintMarker(painter, ps, MarkerType::first,
                             color_array[i], color_array[i]);
            }
            if(j==1){
                paintMarker(painter, ps, MarkerType::second,
                             Qt::green, color_array[i]);
            }
            else if(j==street.points.size()-2){
                paintMarker(painter, ps, MarkerType::lastBut,
                             Qt::green, color_array[i]);
            }
            else if(j==street.points.size()-1){
                paintMarker(painter, ps, MarkerType::last,
                             color_array[i], color_array[i]);
            }
            else {
                paintMarker(painter, ps, MarkerType::other,
                             Qt::blue, Qt::white);
            }

        }
        painter.setPen(color_array[i]);
        painter.setBrush(color_array[i]);
        painter.drawPolyline(street_pline);
        i++;
        if(i>12)
            i = 0;
    }


    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    //QPointF p((m_my_pos*m_zoom-m_center));
    QPointF p = toPointInScreen(m_ego->pos());
    QPointF pp = p-m_center;
    painter.drawEllipse(pp, 5, 5);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(QPoint(pp.x(), pp.y()-5), m_ego->map_info().street_name);
    // std::cout << "paint p x: " << p.x() << ", y: " << p.y() << std::endl;
    // std::cout << "paint pp: x" << pp.x() << ", y:" << pp.y() << std::endl;

    painter.setPen(Qt::darkRed);
    painter.setBrush(Qt::darkRed);
    p = toPointInScreen(m_other->pos());
    pp = p-m_center;
    painter.drawEllipse(pp, 5, 5);
    painter.drawText(QPoint(pp.x(), pp.y()-5), m_other->map_info().street_name);

}

void MapViewer::mousePressEvent(QMouseEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    switch(event->button()){
    case Qt::LeftButton:
        if (modifiers & Qt::ControlModifier) {
            m_reference = event->pos();
            qApp->setOverrideCursor(Qt::ClosedHandCursor);
            m_pan_en = true;
        }
        break;
    default:
        break;
    }
}

void MapViewer::mouseReleaseEvent(QMouseEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    QPointF sp;
    switch(event->button()){
    case Qt::LeftButton:
        if (modifiers & Qt::ControlModifier) {
            qApp->restoreOverrideCursor();
            m_pan_en = false;
            update();
        }
        else {
            std::cout << "L - Event x: " << event->position().x() << ", y: " << event->position().y() << std::endl;
            sp = (event->pos()-m_delta);
            std::cout << "event sp x: " << sp.x() << ", y: " << sp.y() << std::endl;
            m_ego->update(toPointInMap(sp));
            update();
        }
        break;
    case Qt::RightButton:
        std::cout << "R - Event x: " << event->position().x() << ", y: " << event->position().y() << std::endl;
        sp = (event->pos()-m_delta);
        std::cout << "event sp x: " << sp.x() << ", y: " << sp.y() << std::endl;
        m_other->update(toPointInMap(sp));

        update();
        break;
    default:
        break;
    }
}

void MapViewer::mouseMoveEvent(QMouseEvent *event) {
    if(m_pan_en){
        m_delta += (event->pos() - m_reference);// * 1.0/m_zoom;
        m_reference = event->pos();
        update();
    }
}

void MapViewer::resizeEvent(QResizeEvent *event)
{
    qDebug() << "Nuove dimensioni widget figlio:" << this->size();
    QSize new_size = event->size();
    std::cout << "New width: " << new_size.width() << ", new height: " << new_size.height() << std::endl;
    m_center = this->rect().center();
    std::cout << "center is at:" << m_center.x() << ", " << m_center.y() << std::endl;
    updateScale(new_size);

    QWidget::resizeEvent(event);
}

void MapViewer::wheelEvent(QWheelEvent *event)
{
    qDebug() << "Ctrl Down";

    QPointF tmp = event->position()-m_delta;
    //convert the event position to a point-in-map with old zoom factor
    tmp = toPointInMap(tmp);

    int delta = event->angleDelta().y();

    if (delta > 0) {
        qDebug() << "Wheel Up";
        m_zoom = m_zoom * 2;
    } else if (delta < 0) {
        qDebug() << "Wheel Down";
        m_zoom = m_zoom / 2;
    }

    event->accept();
    qDebug() << "new zoom factor: " << m_zoom;

    //center (convert to point-in-screen) with new zoom factor
    centerTo(tmp);
    update();

}
