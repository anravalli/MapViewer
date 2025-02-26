#include "mapviewer.h"
#include "./ui_mainwindow.h"

//#include "pugixml.hpp"
//#include <libxml++/libxml++.h>
#include <QXmlStreamReader>

#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <string>
#include <iomanip>

MapViewer::MapViewer(QWidget *_parent):
    QWidget(_parent)
{
    std::cout << "MapViewer created" << std::endl;
    this->setParent(_parent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_my_pos = QPointF(-1,-1);
    //setFixedSize(1000, 1000);

    //QSize w_size = this->size();
    // QSize p_size = _parent->size();
    // if(w_size != p_size)
    //     this->resize(p_size);
    m_center = this->rect().center();
    std::cout << "center is at:" << m_center.x() << ", " << m_center.y() << std::endl;
}

void MapViewer::loadMap(QString osmFile)
{
    m_streets.clear();
    extractStreets(osmFile.toStdString().c_str());

    if (!m_streets.empty()) {
        std::cout << "Extracted streets:" << std::endl;
        for (const auto& street : m_streets) {
            std::cout << "Name: " << street.name << ", Type: " << street.type << std::endl;
            //std::cout << "Coordinates: ";
            for (const auto& coord : street.points) {
                // std::cout << "(" << coord.x() << ", " << coord.y() << ") ";
                if(m_lat_min > coord.y())
                    m_lat_min = coord.y();
                if(m_lat_max < coord.y())
                    m_lat_max = coord.y();
                if(m_lon_min > coord.x())
                    m_lon_min = coord.x();
                if(m_lon_max < coord.x())
                    m_lon_max = coord.x();

            }
            // std::cout << std::endl;
        }
        std::cout << "m_lat_min:" << m_lat_min << " m_lon_min: " << m_lon_min << std::endl;
        std::cout << "m_lat_max:" << m_lat_max << " m_lon_max: " << m_lon_max << std::endl;
    }
    updateScale(this->size());
    m_my_pos = this->rect().center();
    m_center = this->rect().center();
    m_reference = QPointF(0,0);
    m_delta = QPointF(0,0);
    m_zoom = 1;
    update();
}

void MapViewer::extractStreets(const QString &osmFilePath)
{
    /*
     * pugixml implementation
    */
    /*
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(osmFilePath.c_str());

    if (result.status != pugi::status_ok) {
        std::cerr << "Error loading OSM file: " << result.description() << std::endl;
        return; // Return empty vector in case of error
    }

    // XPath query to find all way elements with a "highway" tag
    for (auto way : doc.select_nodes("//way[tag[@k='highway']]")) {
        Street street;

        // Extract street name (if available)
        pugi::xml_node nameTag = way.node().find_child(
                    [](pugi::xml_node n){
                        return not std::strcmp(n.name(), "tag") && not std::strcmp(n.attribute("k").value(),"name");
                    } );
        if (nameTag) {
            street.name = nameTag.attribute("v").value();
        }

        // Extract street type
        pugi::xml_node highwayTag = way.node().find_child( [](pugi::xml_node n){ return not std::strcmp(n.name(), "tag") && not std::strcmp(n.attribute("k").value(),"highway"); } );

        if (highwayTag) {
            street.type = highwayTag.attribute("v").value();
        }

         // Extract coordinates from nd nodes
        std::locale loc("en_US.utf8");
        for (auto nd : way.node().select_nodes("nd")) {
            std::string ref = nd.node().attribute("ref").value();
            // Find the corresponding node element to get coordinates
            for (auto node : doc.select_nodes(std::string("//node[@id='" + ref + "']").c_str())) {
                double lat = node.node().attribute("lat").as_double();
                double lon = node.node().attribute("lon").as_double();
                // std::cout << "lat as string: " << node.node().attribute("lat").as_string() << std::endl;
                // std::cout << "lat as string: " << node.node().attribute("lon").as_string() << std::endl;
                // std::string s_lat = node.node().attribute("lat").as_string();
                // std::string s_lon = node.node().attribute("lon").as_string();
                // double lon = std::stod(s_lon);
                // double lat = std::stod(s_lat);
                street.points.push_back(QPointF(lat, lon));
                break; // Exit inner loop once node is found
            }
        }
    */


    /*
     * libxm2/libxml++ implementation
     */
    /*
    try {
        xmlpp::DomParser parser;
        parser.parse_file(osmFilePath);

        if (parser) {
            xmlpp::Node* rootNode = parser.get_document()->get_root_node();
            if (rootNode) {
                std::map<long long, QPointF> nodes;

                // Extract nodes
                xmlpp::Node::NodeSet nodeSet = rootNode->find("//node");
                for (xmlpp::Node* node : nodeSet) {
                    xmlpp::Element* nodeElement = dynamic_cast<xmlpp::Element*>(node);
                    if (nodeElement) {
                        long long id = std::stoll(nodeElement->get_attribute_value("id"));
                        double lat = std::stod(nodeElement->get_attribute_value("lat"));
                        double lon = std::stod(nodeElement->get_attribute_value("lon"));
                        nodes[id] = QPointF(lat, lon);
                    }
                }

                // Extract ways
                xmlpp::Node::NodeSet waySet = rootNode->find("//way[tag[@k='highway']]");
                for (xmlpp::Node* way : waySet) {
                    xmlpp::Element* wayElement = dynamic_cast<xmlpp::Element*>(way);
                    if (wayElement) {
                        Street currentWay;

                        // Extract name
                        xmlpp::Node::NodeSet nameTagSet = wayElement->find("tag[@k='name']");
                        if (!nameTagSet.empty()) {
                            xmlpp::Element* nameTag = dynamic_cast<xmlpp::Element*>(nameTagSet.front());
                            if (nameTag) {
                                currentWay.name = nameTag->get_attribute_value("v");
                            }
                        }

                        // Extract highway
                        xmlpp::Node::NodeSet highwayTagSet = wayElement->find("tag[@k='highway']");
                        if (!highwayTagSet.empty()) {
                            xmlpp::Element* highwayTag = dynamic_cast<xmlpp::Element*>(highwayTagSet.front());
                            if (highwayTag) {
                                currentWay.type = highwayTag->get_attribute_value("v");
                            }
                        }

                        // Extract node refs
                        xmlpp::Node::NodeSet ndSet = wayElement->find("nd");
                        for (xmlpp::Node* nd : ndSet) {
                            xmlpp::Element* ndElement = dynamic_cast<xmlpp::Element*>(nd);
                            if (ndElement) {
                                long long ref = std::stoll(ndElement->get_attribute_value("ref"));
                                if (nodes.find(ref) != nodes.end()) {
                                    std::cout << "  Node: " << std::fixed << std::setprecision(8) << nodes[ref].x() << ", " << nodes[ref].y() << std::endl;
                                    currentWay.points.push_back(nodes[ref]);
                                }

                            }
                        }

                        m_streets.push_back(currentWay);
                    }
                }

            } else {
                std::cerr << "Error: Empty document." << std::endl;
            }
        } else {
            std::cerr << "Error: Could not parse file " << osmFilePath << std::endl;
        }
    } catch (const xmlpp::exception& ex) {
        std::cerr << "libxml++ exception: " << ex.what() << std::endl;
        return;
    }
    */

    QFile file(osmFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Could not open file " << osmFilePath.toStdString() << std::endl;
            return;
        }

        QXmlStreamReader xml(&file);
        std::map<long long, QPointF> nodes;
        // std::vector<Way> ways;

        while (!xml.atEnd() && !xml.hasError()) {
            QXmlStreamReader::TokenType token = xml.readNext();
            if (token == QXmlStreamReader::StartElement) {
                if (xml.name() == "node") {
                    long long id = xml.attributes().value("id").toLongLong();
                    double lon = xml.attributes().value("lon").toDouble();
                    double lat = xml.attributes().value("lat").toDouble();
                    nodes[id] = QPointF(lon, lat);
                } else if (xml.name() == "way") {
                    Street currentWay;

                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "way")) {
                        xml.readNext();
                        if (xml.tokenType() == QXmlStreamReader::StartElement) {
                            if (xml.name() == "tag") {
                                QString k = xml.attributes().value("k").toString();
                                QString v = xml.attributes().value("v").toString();
                                if (k == "name") {
                                    currentWay.name = v.toStdString();

                                } else if (k == "highway") {
                                    currentWay.type = v.toStdString();
                                }
                            } else if (xml.name() == "nd") {
                                long long ref = xml.attributes().value("ref").toLongLong();
                                if (nodes.find(ref) != nodes.end())
                                    currentWay.points.push_back(nodes[ref]);
                            }
                        }
                    }
                    if (isStreet(currentWay.type)) {
                      m_streets.push_back(currentWay);
                    }
                }
            }
        }

        if (xml.hasError()) {
            std::cerr << "Error: XML parsing error: " << xml.errorString().toStdString() << std::endl;
            return;
        }

        file.close();


}

void MapViewer::printStreets()
{

    // Print extracted data
    for (const auto& street : m_streets) {
        std::cout << "street: " << street.name << " (" << street.type << ")" << std::endl;
        for (auto p : street.points) {
            std::cout << "(" << std::fixed << std::setprecision(8) << p.x() << ", " << p.y() << ")";
        }
        std::cout << std::endl;
    }
}


void MapViewer::selectMap()
{
    std::cout << "select map " << std::endl;
    m_map_file = QFileDialog::getOpenFileName(this, "Open Map File", "", "OSM (*.osm)");

    // Check if the user selected a file
    if (!m_map_file.isEmpty()) {
        qDebug() << "Selected file:" << m_map_file;
        loadMap(m_map_file);
    } else {
        qDebug() << "No file selected.";
    }
}

QPointF MapViewer::toPointInScreen(QPointF coord)
{
    QPointF screen_point;
    // QSizeF curr_screen_size = this->size();
    double x = (coord.x()-m_lon_min)*m_w_scale*m_zoom;
    //traslate & flip  m_lat_max - y;
    double y = (m_lat_max - m_lat_min) - (coord.y() - m_lat_min);
    y = y*m_h_scale*m_zoom;
    //std::cout << "screen point x: " << x << ", y: " << y << std::endl;
    screen_point.setX(x); // - this->rect().center().x());
    screen_point.setY(y); // - this->rect().center().y());
    return screen_point;
}


void MapViewer::updateScale(QSize new_size)
{
    m_w_scale = new_size.width()/(m_lon_max-m_lon_min);
    m_h_scale = new_size.height()/(m_lat_max-m_lat_min);
    std::cout << "New w_scale: " << m_w_scale << ", new h_scale: " << m_h_scale << std::endl;
    m_width = new_size.width();
    m_height = new_size.height();
}

bool MapViewer::isStreet(std::string type)
{
    bool is_street = false;
    if(not type.empty()){
        if((type == "residential") or
                (type == "primary") or
                (type == "secondary"))
            is_street = true;
    }
    return is_street;
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
    for(auto street: m_streets){
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

    if(m_my_pos != QPointF(-1,-1)){
        painter.setPen(Qt::green);
        painter.setBrush(Qt::green);
        QPointF p((m_my_pos*m_zoom-m_center));
        //std::cout << "my pos: (" << p.x() << "," << p.y() << ") - my pos zoomed (" << (p*m_zoom).x() << "," << (p*m_zoom).y() << ")" << std::endl;
        painter.drawEllipse(p, 5, 5);
    }
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
    switch(event->button()){
    case Qt::LeftButton:
        std::cout << "Event x: " << event->x() << ", y: " << event->y() << std::endl;
        m_my_pos = (event->pos()-m_delta)/m_zoom;
        std::cout << "m_my_pos x: " << m_my_pos.x() << ", y: " << m_my_pos.y() << std::endl;
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
        m_delta += (event->pos() - m_reference) * 1.0/m_zoom;
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
