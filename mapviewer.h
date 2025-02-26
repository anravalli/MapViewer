#ifndef MAPVIEWER_H999
#define MAPVIEWER_H

#include <cstring>
#include <iostream>
#include <QString>
#include <vector>
#include <QWidget>
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>

struct Street {
    std::string name;
    std::string type; // e.g., "primary", "secondary", "residential"
    // Add other relevant attributes as needed (e.g., length, points)
    // For points, you might want to use a more robust data structure
    // like a std::vector of pairs (latitude, longitude)
    std::vector<QPointF> points;
};

class MapViewer: public QWidget
{
    Q_OBJECT
public:
    MapViewer(QWidget *_parent);

    void loadMap(QString osmFile);
    void extractStreets(const QString &osmFilePath);
    void printStreets();

public slots:
    void selectMap();
    // void onClick();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPointF toPointInScreen(QPointF coord);
    void updateScale(QSize new_size);
    bool isStreet(std::string type);

    QString m_map_file = "";

    bool m_pan_en = false;

    std::vector<Street> m_streets;
    double m_lat_min = 999;
    double m_lat_max = 0;
    double m_lon_min = 999;
    double m_lon_max = 0;

    double m_w_scale = 0;
    double m_h_scale = 0;
    double m_width = 0;
    double m_height = 0;
    // double m_h_offset = 0;

    QPointF m_my_pos;
    QPointF m_center;
    QPointF m_reference;
    QPointF m_delta;
    double m_zoom = 1;
};

#endif // MAPVIEWER_H
