#include "mapviewer.h"
#include "./ui_mainwindow.h"

#include "map.h"
#include "ego.h"

#include <QDebug>

MapViewer::MapViewer(QWidget *_parent, Map *map, Ego *ego):
    QWidget(_parent), m_map(map), m_ego(ego)
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

    // x = (lon-min.x())*m_w_scale*m_zoom --> x/(m_w_scale*m_zoom) = lon-min.x()
    //      --> lon = min.x() + x/(m_w_scale*m_zoom)
    double lon = min.x() + sp.x()/(m_w_scale*m_zoom);
    // y = ( m_map->height() - ( lat - min.y() ) )*m_h_scale*m_zoom;
    // y/(m_h_scale*m_zoom) = m_map->height() - ( lat - min.y() )
    // y/(m_h_scale*m_zoom) - m_map->height() = - ( lat - min.y() )
    // lat - min.y() = - (y/(m_h_scale*m_zoom) - m_map->height()) = -y/(m_h_scale*m_zoom) + m_map->height()
    // lat - min.y() = m_map->height() - y/(m_h_scale*m_zoom)
    //      --> lat = m_map->height() + min.y() - y/(m_h_scale*m_zoom)
    //      --> lat = m_map->height() - (y/(m_h_scale*m_zoom) - min.y())
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
    painter.translate(this->rect().center());
    // painter.translate(m_center - this->rect().center());
    painter.translate(m_delta);
    // painter.setFont(QFont("Arial", 20));
    // painter.drawText(QPoint(100, 100), "Hello, Canvas!");
    enum Qt::GlobalColor color_array[4] = {Qt::white, Qt::yellow, Qt::darkBlue, Qt::red};

    QPointF ps;
    int i = 0;
    for(auto street: m_map->streets()){
        QVector<QPointF> street_pline;
        //style for points
        painter.setPen(Qt::blue);
        painter.setBrush(Qt::white);
        for(auto p: street.points ){
            ps = toPointInScreen(p)-m_center;
            painter.drawEllipse(ps, 2, 2);
            street_pline.append(ps);
        }
        //style for lines
        //std::cout << "(color: " << color_array[i] << ") - " << "street: " << street.name << std::endl;
        painter.setPen(color_array[i]);
        painter.setBrush(color_array[i]);
        painter.drawPolyline(street_pline);
        i++;
        if(i>3)
            i = 0;
    }


    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    //QPointF p((m_my_pos*m_zoom-m_center));
    QPointF p = toPointInScreen(m_ego->pos());
    std::cout << "paint p x: " << p.x() << ", y: " << p.y() << std::endl;
    QPointF pp = p-m_center;
    std::cout << "paint pp: x" << pp.x() << ", y:" << pp.y() << std::endl;
    painter.drawEllipse(pp, 5, 5);

}

void MapViewer::mousePressEvent(QMouseEvent *event)
{
    switch(event->button()){
    case Qt::LeftButton:
        break;
    case Qt::MiddleButton:
        m_reference = event->pos();
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        m_pan_en = true;
        break;
    default:
        break;
    }
}

void MapViewer::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF sp;
    switch(event->button()){
    case Qt::LeftButton:
        std::cout << "Event x: " << event->x() << ", y: " << event->y() << std::endl;
        sp = (event->pos()-m_delta);
        std::cout << "event sp x: " << sp.x() << ", y: " << sp.y() << std::endl;
        m_ego->update(toPointInMap(sp));

        update();
        break;
    case Qt::MiddleButton:
        qApp->restoreOverrideCursor();
        m_pan_en = false;
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
    // Ottieni la direzione della rotazione


    // Ottieni i modificatori (Ctrl, Shift, Alt)
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if (modifiers & Qt::ControlModifier) {
        qDebug() << "Ctrl premuto";
        // Accetta l'evento per evitare che venga propagato ad altri widget
        int delta = event->angleDelta().y();

        if (delta > 0) {
            qDebug() << "Rotellina su";
            m_zoom = m_zoom * 2;
        } else if (delta < 0) {
            qDebug() << "Rotellina giù";
            m_zoom = m_zoom / 2;
        }

        //m_delta = this->rect().center()-event->position();
        // m_center = event->position();
        event->accept();
        qDebug() << "new zoom factor: " << m_zoom;
        // qDebug() << "new center is: " << m_center;

        update();
    }
}
