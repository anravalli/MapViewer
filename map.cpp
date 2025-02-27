#include "map.h"

#include <iostream>
#include <iomanip>
#include <string>
//#include "pugixml.hpp"
//#include <libxml++/libxml++.h>
#include <QXmlStreamReader>
#include <QFile>
#include <QPointF>

Map::Map()
{
    m_rect = QRectF(0,0, 0.1,0.1);
}

void Map::loadMap(QString osmFile)
{
    m_streets.clear();
    extractStreets(osmFile.toStdString().c_str());

    if (!m_streets.empty()) {
        QPointF top_left(999,999);
        QPointF bottom_right(0,0);
        std::cout << "Extracted streets:" << std::endl;
        for (const auto& street : m_streets) {
            std::cout << "Name: " << street.name << ", Type: " << street.type << std::endl;
            //std::cout << "Coordinates: ";
            for (const auto& coord : street.points) {
                // std::cout << "(" << coord.x() << ", " << coord.y() << ") ";
                if(top_left.y() > coord.y())
                    top_left.setY(coord.y());
                if(top_left.x() > coord.x())
                    top_left.setX(coord.x());

                if(bottom_right.y() < coord.y())
                    bottom_right.setY(coord.y());
                if(bottom_right.x() < coord.x())
                    bottom_right.setX(coord.x());

            }
            // std::cout << std::endl;
        }
        m_rect = QRectF(top_left, bottom_right);
        std::cout << "m_lat_min:" << top_left.y() << " m_lon_min: " <<top_left.x() << std::endl;
        std::cout << "m_lat_max:" << bottom_right.y() << " m_lon_max: " << bottom_right.x() << std::endl;
    }
}

void Map::extractStreets(const QString &osmFilePath)
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

bool Map::isStreet(std::string type)
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

QRectF Map::rect() const
{
    return m_rect;
}

double Map::width() const
{
    return m_rect.width();
}

double Map::height() const
{
    return m_rect.height();
}

QPointF Map::bottom_right() const
{
    return m_rect.bottomRight();
}

QPointF Map::top_left() const
{
    return m_rect.topLeft();
}

std::vector<Street> Map::streets() const
{
    return m_streets;
}

void Map::printStreets()
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
