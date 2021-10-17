#ifndef GEOJSON_STRUCT_H
#define GEOJSON_STRUCT_H
#include "json_struct.h"

#define GEOJS_PROPERTIES JS_OBJ
#define GEOJS_PROPERTIES_EXT JS_OBJ_EXT

#define GEOJS_GEOMETRY(geometry) GEOJS_GEOMETRY_INTERNAL_IMPL(geometry)

#define GEOJS_GEOMETRY_INTERNAL_IMPL(geometry) \
  using GEOMETRY_TYPE = decltype(geometry);                                             \
  const GEOMETRY_TYPE & geojs_get_geometry() const \
  {                                                                                                          \
    return geometry;                                                                                              \
  }                                            \


struct Point {
  double x;
  double y;
  double z;
  Point(double x, double y, double z) : x(x), y(y), z(z) {}
};

template <>
struct JS::TypeHandler<const Point &>
{
public:
  /*
  static inline Error to(std::vector<Point> &to_type, ParseContext &context)
  {
    if (context.token.value_type != JS::Type::ArrayStart)
      return Error::ExpectedArrayStart;
    Error error = context.nextToken();
    if (error != JS::Error::NoError)
      return error;
    to_type.clear();
    to_type.reserve(10);
    while (context.token.value_type != JS::Type::ArrayEnd)
    {

      bool toBool;
      error = TypeHandler<bool>::to(toBool, context);
      to_type.push_back(toBool);
      if (error != JS::Error::NoError)
        break;
      error = context.nextToken();
      if (error != JS::Error::NoError)
        break;
    }

    return error;
  }
   */

  static inline void from(const Point &p, Token &token, Serializer &serializer)
  {
    token.value_type = JS::Type::ArrayStart;
    token.value = JS::DataRef("[");
    serializer.write(token);
    token.name.size = 0;
    TypeHandler<double>::from(p.x, token, serializer);
    TypeHandler<double>::from(p.y, token, serializer);
    TypeHandler<double>::from(p.z, token, serializer);
    token.value_type = JS::Type::ArrayEnd;
    token.value = JS::DataRef("]");
    serializer.write(token);
    token.name.size = 0;
  }
};

namespace magis {
namespace atlas{

enum class GeoJSFeatureType {
  POLYGON,
  LINESTRING,
  POINT
};

std::vector<std::string> geoJSFeatureTypeString{"Polygon", "LineString", "Point"};

template <typename T>
void serializeGeometry(const T &from_type, GeoJSFeatureType type, JS::Token &token, JS::Serializer &serializer)
{
  token.value_type = JS::Type::ObjectStart;
  token.value = JS::DataRef("{");
  serializer.write(token);

  token.name_type = JS::Type::String;
  token.name = JS::DataRef("coordinates");
  if (type == GeoJSFeatureType::POLYGON) {
    token.value_type = JS::Type::ArrayStart;
    token.value = JS::DataRef("[");
    serializer.write(token);
    token.name.size = 0;
  }
  if (type == GeoJSFeatureType::POLYGON || type == GeoJSFeatureType::LINESTRING) {
    token.value_type = JS::Type::ArrayStart;
    token.value = JS::DataRef("[");
    serializer.write(token);
    token.name.size = 0;
  }
  for (const auto &point:from_type.geojs_get_geometry())
  {
    JS::TypeHandler<decltype(point)>::from(point, token, serializer);
  }
  if (type == GeoJSFeatureType::POLYGON || type == GeoJSFeatureType::LINESTRING) {
    token.value_type = JS::Type::ArrayEnd;
    token.value = JS::DataRef("]");
    serializer.write(token);
    token.name.size = 0;
  }
  if (type == GeoJSFeatureType::POLYGON) {
    token.value_type = JS::Type::ArrayEnd;
    token.value = JS::DataRef("]");
    serializer.write(token);
    token.name.size = 0;
  }


  token.name = JS::DataRef("type");
  token.name_type = JS::Type::String;
  token.value = JS::DataRef(geoJSFeatureTypeString[static_cast<size_t>(type)]);
  token.value_type = JS::Type::String;
  serializer.write(token);

  token.name.size = 0;
  token.value_type = JS::Type::ObjectEnd;
  token.value = JS::DataRef("}");
  serializer.write(token);
}

template <typename T>
void serializeFeature(const T &from_type, GeoJSFeatureType type, JS::Token &token, JS::Serializer &serializer)
{

  token.value_type = JS::Type::ObjectStart;
  token.value = JS::DataRef("{");
  serializer.write(token);

  token.name = JS::DataRef("geometry");
  token.name_type = JS::Type::String;

  serializeGeometry(from_type, type, token, serializer);

  token.name = JS::DataRef("properties");
  token.name_type = JS::Type::String;

  JS::TypeHandler<T>::from(from_type, token, serializer);

  token.name = JS::DataRef("type");
  token.name_type = JS::Type::String;
  token.value = JS::DataRef("Feature");
  token.value_type = JS::Type::String;
  serializer.write(token);

  token.name.size = 0;
  token.value_type = JS::Type::ObjectEnd;
  token.value = JS::DataRef("}");
  serializer.write(token);
}

template <typename T>
std::string serializeFeatureCollection (const std::vector<T> &from_type,
                                       GeoJSFeatureType type,
                                       const std::string &name = "",
                                       const std::string &crs = "urn:ogc:def:crs:EPSG::3857",
                                       const JS::SerializerOptions &options
                                        = JS::SerializerOptions(JS::SerializerOptions::Compact))
{

  std::string ret_string;
  JS::SerializerContext serializeContext(ret_string);
  serializeContext.serializer.setOptions(options);
  JS::Token token;
  token.value_type = JS::Type::ObjectStart;
  token.value = JS::DataRef("{");
  serializeContext.serializer.write(token);

  token.name = JS::DataRef("name");
  token.name_type = JS::Type::String;
  token.value = JS::DataRef(name);
  token.value_type = JS::Type::String;
  serializeContext.serializer.write(token);

  token.name = JS::DataRef("crs");
  token.name_type = JS::Type::String;
  token.value_type = JS::Type::ObjectStart;
  token.value = JS::DataRef("{");
  serializeContext.serializer.write(token);

  token.name = JS::DataRef("type");
  token.name_type = JS::Type::String;
  token.value = JS::DataRef("name");
  token.value_type = JS::Type::String;
  serializeContext.serializer.write(token);

  token.name = JS::DataRef("properties");
  token.name_type = JS::Type::String;
  token.value_type = JS::Type::ObjectStart;
  token.value = JS::DataRef("{");
  serializeContext.serializer.write(token);

  token.name = JS::DataRef("name");
  token.name_type = JS::Type::String;
  token.value = JS::DataRef(crs);
  token.value_type = JS::Type::String;
  serializeContext.serializer.write(token);

  token.name.size = 0;
  token.value_type = JS::Type::ObjectEnd;
  token.value = JS::DataRef("}");
  serializeContext.serializer.write(token);
  token.name.size = 0;
  token.value_type = JS::Type::ObjectEnd;
  token.value = JS::DataRef("}");
  serializeContext.serializer.write(token);

  token.name = JS::DataRef("features");
  token.name_type = JS::Type::String;
  token.value_type = JS::Type::ArrayStart;
  token.value = JS::DataRef("[");
  serializeContext.serializer.write(token);
  token.name.size = 0;
  for (const auto &feature : from_type)
  {
    serializeFeature(feature, type, token, serializeContext.serializer);
  }
  token.name.size = 0;
  token.value_type = JS::Type::ArrayEnd;
  token.value = JS::DataRef("]");
  serializeContext.serializer.write(token);

  token.name = JS::DataRef("type");
  token.name_type = JS::Type::String;
  token.value = JS::DataRef("FeatureCollection");
  token.value_type = JS::Type::String;
  serializeContext.serializer.write(token);

  token.name.size = 0;
  token.value_type = JS::Type::ObjectEnd;
  token.value = JS::DataRef("}");
  serializeContext.serializer.write(token);
  serializeContext.flush();
  return ret_string;
}

}
}

#endif