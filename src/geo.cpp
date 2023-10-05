#include <lugo/geo.hpp>

auto isInValidateVector(const lugo::Vector &v) -> bool
{
  return v.x() == 0 && v.y() == 0;
}

auto NewVector(const lugo::Point &from, const lugo::Point &to) -> lugo::Vector
{
  auto v = lugo::Vector();
  v.set_x(to.x() - from.x());
  v.set_y(to.y() - from.y());
  if (isInValidateVector(v)) {
    throw std::runtime_error("an vector cannot have zero length");
  }
  return v;
}

auto getLength(const lugo::Vector &v) -> float
{
  return std::hypot(v.x(), v.y());
}

auto getScaledVector(const lugo::Vector &v, float scale) -> lugo::Vector
{
  if (scale <= 0) {
    throw std::runtime_error("vector can not have zero length");
  }
  auto v2 = lugo::Vector();
  v2.set_x(v.x() * scale);
  v2.set_y(v.y() * scale);
  return v2;
}

auto normalize(const lugo::Vector &v) -> lugo::Vector
{
  auto const length = getLength(v);
  return getScaledVector(v, 100 / length);
}

auto subVector(const lugo::Vector &originalV, const lugo::Vector &subV) -> lugo::Vector
{
  auto const newX = originalV.x() - subV.x();
  auto const newY = originalV.y() - subV.y();
  auto newVector = lugo::Vector();
  newVector.set_x(newX);
  newVector.set_y(newY);
  if (isInValidateVector(newVector)) {
    throw std::runtime_error("could not subtract vectors an vector cannot have zero length");
  }
  return newVector;
}

auto distanceBetweenPoints(const lugo::Point &a, const lugo::Point &b) -> float
{
  return std::hypot(a.x() - b.x(), a.y() - b.y());
}
