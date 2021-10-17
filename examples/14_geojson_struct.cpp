//
// Created by 高常宇 on 2021/10/16.
//
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include "geojson_struct.h"

struct Layer {
  std::string name;
  unsigned int number;
  std::vector<Point> points;
  std::vector<double> v;
  GEOJS_PROPERTIES(name, number, v);
  GEOJS_GEOMETRY(points);
};

int main(int argc, char **argv) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1,100); // distribution in range [1, 6]

  std::vector<Layer> ss;
  Layer s;
  s.name = "Boundary";
  for (size_t i = 0U; i < 10; ++i)
  {
    s.number = dist(rng);
    for (size_t j = 0U; j < s.number; ++j) {
      s.points.emplace_back(dist(rng), dist(rng) , dist(rng));
    }
    for (size_t j = 0U; j < 3; ++j)
    {
      s.v.emplace_back(dist(rng));
    }
    ss.emplace_back(s);
  }

  auto geojson = magis::atlas::serializeFeatureCollection(ss, magis::atlas::GeoJSFeatureType::LINESTRING, "boundary");
  std::cout<<"geojson: "<<geojson<<std::endl;
  std::ofstream geojsonFile("/tmp/result.geojson");
  geojsonFile<<geojson;

  auto normaljson = JS::serializeStruct(ss, JS::SerializerOptions(JS::SerializerOptions::Compact));
  std::cout<<"normal json: "<<normaljson<<std::endl;
  std::ofstream jsonFile("/tmp/result.json");
  jsonFile<<normaljson;
}