#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QRectF>
#include <vector>

struct Street {
    std::string name;
    std::string type; // e.g., "primary", "secondary", "residential"
    // Add other relevant attributes as needed (e.g., length, points)
    // For points, you might want to use a more robust data structure
    // like a std::vector of pairs (latitude, longitude)
    std::vector<QPointF> points;
};

class Map: public QObject
{
    Q_OBJECT
public:
    Map();
    void loadMap(QString osmFile);
    void printStreets();

    std::vector<Street> streets() const;

    QPointF top_left() const;

    QPointF bottom_right() const;

    double width() const;
    double height() const;

    QRectF rect() const;

private:
    void extractStreets(const QString &osmFilePath);
    bool isStreet(std::string type);

    QString m_map_file = "";

    std::vector<Street> m_streets;
    QRectF m_rect;
    // QPointF m_top_left;
    // QPointF m_bottom_right;
    // double m_width;
    // double m_height;
};

#endif // MAP_H
