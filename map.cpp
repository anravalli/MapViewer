#include "map.h"

#include <iostream>
#include <iomanip>
#include <string>
//#include "pugixml.hpp"
//#include <libxml++/libxml++.h>
#include <QXmlStreamReader>

#include <QPointF>
#include <QLineF>

#include "Geodesy.h"

void testFunc()
{
    Geodesy::Position a = {Geodesy::degToRad(45.4355), Geodesy::degToRad(9.1772)};
    Geodesy::Position b = {Geodesy::degToRad(45.4353), Geodesy::degToRad(9.17552)};
    double bearing = Geodesy::initialBearing(a, b);
    std::cout << "bearing: " << Geodesy::radToDeg(bearing) << " (" << bearing << ")" << std::endl;
    auto relative_end = Geodesy::relativePosition(a, bearing, b);
    std::cout << "relative end - x: " << relative_end.x << ", y: " << relative_end.y << std::endl;
    relative_end = Geodesy::relativePosition(a, Geodesy::degToRad(-99.628), b);
    std::cout << "(99) relative end - x: " << relative_end.x << ", y: " << relative_end.y << std::endl;
}

Map::Map()
{
    m_rect = QRectF(0,0, 0.1,0.1);
    testFunc();
}

void Map::loadMap(QString osmFile)
{
    m_streets.clear();
    extractBounds(osmFile.toStdString().c_str());
    extractStreets(osmFile.toStdString().c_str());
    if (!m_streets.empty()) {
        printStreets();
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

void Map::extractBounds(const QString &osmFilePath)
{
    QFile file(osmFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Error: Could not open file " << osmFilePath.toStdString() << std::endl;
        return;
    }

    QXmlStreamReader xml(&file);

    //<bounds minlat="39.5291700" minlon="-122.3498900" maxlat="39.5467100" maxlon="-122.3172300"/>

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "bounds") {
                // std::cout << xml.text().toStdString() << std::endl;
                double min_lat = xml.attributes().value("minlat").toDouble();
                double max_lat = xml.attributes().value("maxlat").toDouble();
                double min_lon = xml.attributes().value("minlon").toDouble();
                double max_lon = xml.attributes().value("maxlon").toDouble();
                std::cout << "Map Bounds: min_lat:" << min_lat << ", min_lon: " <<min_lon << "max_lat:" <<
                             max_lat << " max_lon: " << max_lon << std::endl;
                m_rect = QRectF(QPointF(min_lon, min_lat), QPointF(max_lon, max_lat));
                break;
            }
        }
    }
}

bool Map::isStreet(std::string type)
{
    bool is_street = false;
    if(not type.empty()){
        if((type == "residential") or
                (type == "primary") or
                (type == "secondary") or
                (type == "tertiary") or
                (type == "tertiary_link") or
                (type == "unclassified") or
                (type == "service") or
                (type == "track") or
                (type == "raceway"))
            is_street = true;
    }
    return is_street;
}

Geodesy::Position pointToPosition (const QPointF &point)
{
    Geodesy::Position pos;
    pos.latitude = Geodesy::degToRad(point.y());
    pos.longitude = Geodesy::degToRad(point.x());
    return pos;
}

bool Map::mapToSegment(const QPointF &point, SegmentInfo &segment)
{
    bool matched = false;
    //Geodesy::Position has lat and lon swapped
    Geodesy::Position g_start = pointToPosition(segment.start);
    Geodesy::Position g_end = pointToPosition(segment.end); //{segment.end.y(), segment.end.x()};
    Geodesy::Position g_p = pointToPosition(point);
    double segment_bearing = Geodesy::initialBearing(g_start, g_end);
    //double segment_direction = std::atan2(dy,dx);
    Geodesy::Point relative_p = Geodesy::relativePosition(g_start, segment_bearing, g_p);
    Geodesy::Point relative_end = Geodesy::relativePosition(g_start, segment_bearing, g_end);
    if(relative_p.x >= 0 and relative_p.x <= relative_end.x
            and fabs(relative_p.y) < street_to_side_line)
    {
        matched = true;
        segment.lenght = relative_end.x;
        segment.relative_position = QPointF(relative_p.x, relative_p.y);
        std::cout << "matched: " << matched << std::endl;
    }
    return matched;
}

bool Map::matchToMap(QPointF p, SegmentInfo &segment)
{
    // bool matched = false;
    for(int i=0; i < m_streets.size(); i++)
    {
        auto street = m_streets[i];
        //double min_distance = 0xFFFFFFFF;
        for(int j=0; j<street.points.size()-1; j++)
        {
            segment.start = street.points[j];
            segment.end = street.points[j+1];
            // matched = ;
            if(mapToSegment(p, segment)){
                segment.start_idx = j;
                segment.street_idx = i;
                segment.street_name = street.name.c_str();
                std::cout << "street #" << segment.street_idx << ": " << segment.street_name.toStdString() << std::endl;
                return true; //assume only one match is possible
            }
        }
    }
    return false;
}

void Map::matchEgoToMap(QPointF p)
{
    std::cout << "Matching EGO to map" << std::endl;
    SegmentInfo segment;
    if (matchToMap(p, segment))
        emit matchedEgo(segment);
}

void Map::matchOtherToMap(QPointF p)
{
    std::cout << "Matching OTHER to map" << std::endl;
    SegmentInfo segment; // = {0};
    if (matchToMap(p, segment))
        emit matchedOther(segment);
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

void Map::printStreets(bool verbose)
{

    // Print extracted data
    for (const auto& street : m_streets) {
        std::cout << "street: " << street.name << " (" << street.type << ")" << std::endl;
        if(verbose){
            for (auto p : street.points) {
                std::cout << "(" << std::fixed << std::setprecision(8) << p.x() << ", " << p.y() << ")";
            }
        }
        std::cout << std::endl;
    }
}
