#include "common/geometry/Polygon.h"
#include "common/geometry/Point.h"
#include <gtest/gtest.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/noding/GeometryNoder.h>
#include <geos_c.h>

namespace bg = boost::geometry;

TEST(polygon,intersection_test){
    using namespace geos::geom;
    GeometryFactory::Ptr factory = GeometryFactory::create();
    CoordinateXY c;
    CoordinateSequence seq({CoordinateXY(0,0),
    CoordinateXY(10,0),
    CoordinateXY(10,3),
    CoordinateXY(5,-1),
    CoordinateXY(5,6),
    CoordinateXY(10,6),
    CoordinateXY(10,5),
    CoordinateXY(6,5),
    CoordinateXY(6,4),
    CoordinateXY(11,4),
    CoordinateXY(11,7),
    CoordinateXY(10,9),
    CoordinateXY(6,9),
    CoordinateXY(6,8),
    CoordinateXY(9,8),
    CoordinateXY(9,7),
    CoordinateXY(3,7),
    CoordinateXY(3,8),
    CoordinateXY(8,13),
    CoordinateXY(0,13),
    CoordinateXY(0,0)});
    Polygon::Ptr pg=factory->createPolygon(std::move(seq));
    LineString::Ptr line = factory->createLineString(CoordinateSequence(
        {CoordinateXY(1,-1),
        CoordinateXY(1,3),
        CoordinateXY(1,3),
        CoordinateXY(3,3),
        CoordinateXY(3,1),
        CoordinateXY(0,1)}));
    
}

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiLineString.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/noding/MCIndexNoder.h>
#include <geos/noding/IntersectionAdder.h>
#include <geos/noding/SegmentString.h>
#include <geos/noding/NodedSegmentString.h>
#include <geos/algorithm/LineIntersector.h>
#include <geos/algorithm/PointLocator.h>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/geom/Point.h>

#include <iostream>
#include <memory>
#include <vector>

using namespace geos::geom;
using namespace geos::io;
using namespace geos::noding;
using namespace geos::algorithm;
using namespace geos::operation::polygonize;

// Собираем все рёбра полигона в один MULTILINESTRING
std::unique_ptr<geos::geom::MultiLineString> collectPolygonEdges(const Polygon* poly,
                                              const GeometryFactory* gf) {
    std::vector<std::unique_ptr<Geometry>> rings;
    rings.push_back(poly->getExteriorRing()->clone());
    for (std::size_t i = 0; i < poly->getNumInteriorRing(); ++i) {
        rings.push_back(poly->getInteriorRingN(i)->clone());
    }
    return gf->createMultiLineString(std::move(rings));
}

// Извлекаем SegmentString из геометрии (аналог SegmentStringExtractor)
void extractSegmentStrings(const Geometry* geom,
                           std::vector<SegmentString*>& out) {
    // Обходим все LineString в геометрии
    for (std::size_t i = 0; i < geom->getNumGeometries(); ++i) {
        const Geometry* g = geom->getGeometryN(i);
        const LineString* ls = dynamic_cast<const LineString*>(g);
        if (ls) {
            // Клонируем координаты, так как NodedSegmentString забирает владение
            geos::geom::CoordinateSequence::Ptr coords = ls->getCoordinates()->clone();
            out.push_back(new NodedSegmentString(coords.get(), false,false,nullptr));
        } else {
            // Если внутри MultiLineString попался не LineString, рекурсивно обходим
            extractSegmentStrings(g, out);
        }
    }
}

std::unique_ptr<Geometry> splitPolygonByLine(const Geometry* polygonGeom,
                                             const Geometry* lineGeom,
                                             const GeometryFactory* gf) {
    const Polygon* poly = dynamic_cast<const Polygon*>(polygonGeom);
    if (!poly) {
        std::cerr << "Входная геометрия не является полигоном." << std::endl;
        return nullptr;
    }

    // 1. Собираем рёбра полигона
    auto edges = collectPolygonEdges(poly, gf);
    assert(edges);
    std::cout<<"number points:"<<edges->getNumPoints()<<std::endl;
    std::cout<<"LineGeom number points:"<<lineGeom->getNumPoints()<<std::endl;
    // 2. Объединяем рёбра полигона и секущую линию
    std::vector<std::unique_ptr<Geometry>> linework;
    linework.push_back(std::move(edges));
    linework.push_back(lineGeom->clone());
    auto combined = gf->createGeometryCollection(std::move(linework));
    std::cout<<"combined number points:"<<combined->getNumPoints()<<std::endl;
    // 3. Извлекаем SegmentString из всех линий
    std::vector<SegmentString*> segStrings;
    extractSegmentStrings(combined.get(), segStrings);
    std::cout<<"segStrings size:"<< \
            segStrings.size()<<std::endl;
    auto& f = segStrings.front();
    auto& b = segStrings.back();
    // for(auto& seg:segStrings)
    //     std::cout<<"segStrings number points:"<< \
    //         "("<<seg->getCoordinates()->front()<<","<<seg->getCoordinates()->back()<<")"<<std::endl;

    // 4. Нодируем
    LineIntersector li;
    IntersectionAdder intAdder(li);
    MCIndexNoder noder;
    noder.setSegmentIntersector(&intAdder);
    noder.computeNodes(&segStrings);

    geos::noding::SegmentString::NonConstVect* nodedSubstrings = noder.getNodedSubstrings();
    std::vector<std::unique_ptr<Geometry>> nodedGeometries;
    for (SegmentString* ss : *nodedSubstrings) {
        // Приводим к NodedSegmentString, чтобы получить координаты с узлами
        auto* nss = dynamic_cast<NodedSegmentString*>(ss);
        if (!nss) continue;

        // getNodedCoordinates() возвращает unique_ptr<CoordinateSequence>
        auto coords = nss->getNodedCoordinates();
        if (coords) {
            // Создаём LineString из координат
            auto line = gf->createLineString(std::move(coords));
            nodedGeometries.push_back(std::move(line));
        }
        // Сам SegmentString больше не нужен — удаляем
        delete ss;
    }
    // 5. Полигонизируем
    auto nodedLinework = gf->createMultiLineString(std::move(nodedGeometries));
    Polygonizer polygonizer;
    polygonizer.add(nodedLinework.get());
    auto polygons = polygonizer.getPolygons();

    // Освобождаем SegmentString
    for (auto* ss : segStrings) delete ss;

    // 6. Фильтруем: оставляем только полигоны внутри исходного
    std::vector<std::unique_ptr<Geometry>> validParts;
    PointLocator locator;

    for (auto& polyPtr : polygons) {
        auto interiorPoint = polyPtr->getInteriorPoint();
        if (interiorPoint &&
            locator.intersects(*interiorPoint->getCoordinate(), poly)) {
            validParts.push_back(std::move(polyPtr));
        }
    }

    return gf->createGeometryCollection(std::move(validParts));
}

int main() {
    auto gf = GeometryFactory::create();

    WKTReader reader(gf.get());
    auto polygon = reader.read("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    auto line = reader.read("LINESTRING(5 -1, 5 11)");

    auto result = splitPolygonByLine(polygon.get(), line.get(), gf.get());

    if (result) {
        WKTWriter writer;
        std::cout << "Результат: " << writer.write(result.get()) << std::endl;
    } else {
        std::cerr << "Не удалось разрезать полигон." << std::endl;
    }

    return 0;
}

TEST(polygon,self_intersection_test){
    
}

TEST(polygon,cut_test){
    
}

TEST(polygon,dissolve_test){

}

// int main(int argc,char* argv[]){
//     testing::InitGoogleTest(&argc,argv);
//     return RUN_ALL_TESTS();
// }
