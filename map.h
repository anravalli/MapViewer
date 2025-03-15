#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QRectF>
#include <vector>
#include <QFile>

constexpr int street_to_side_line = 3; //meters

struct Street {
    std::string name;
    std::string type; // e.g., "primary", "secondary", "residential"
    // Add other relevant attributes as needed (e.g., length, points)
    // For points, you might want to use a more robust data structure
    // like a std::vector of pairs (latitude, longitude)
    std::vector<QPointF> points;
};

struct SegmentInfo {
    QPointF relative_position;
    double lenght;
    double bearing;
    QString street_name;
    int street_idx;
    int start_idx;
    QPointF start;
    QPointF end;
};

class Map: public QObject
{
    Q_OBJECT
public:
    Map();
    void loadMap(QString osmFile);
    void printStreets(bool verbose = false);

    std::vector<Street> streets() const;

    QPointF top_left() const;

    QPointF bottom_right() const;

    double width() const;
    double height() const;

    QRectF rect() const;

public slots:
    void matchEgoToMap(QPointF p);
    void matchOtherToMap(QPointF p);

signals:
    void matchedEgo(SegmentInfo segment);
    void matchedOther(SegmentInfo segment);

private:
    bool matchToMap(QPointF p, SegmentInfo &segment);
    void extractStreets(const QString &osmFilePath);
    void extractBounds(const QString &osmFilePath);
    bool isStreet(std::string type);
    bool mapToSegment(const QPointF& point, SegmentInfo &segment);

    QString m_map_file = "";

    std::vector<Street> m_streets;
    QRectF m_rect;
    // QPointF m_top_left;
    // QPointF m_bottom_right;
    // double m_width;
    // double m_height;
};

#endif // MAP_H
