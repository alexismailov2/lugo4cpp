#pragma once

#include <physics.pb.h>

/**
 *
 * @param Vector v
 */
auto isInValidateVector(lugo::Vector const& v) -> bool
{
  return v.x() == 0 && v.y() == 0;
}

/**
 *
 * @param Point from
 * @param Point to
 * @returns Vector
 */
auto NewVector(lugo::Point from, lugo::Point to) -> lugo::Vector
{
  auto v = lugo::Vector();
  v.set_x(to.x() - from.x());
  v.set_y(to.y() - from.y());
  if (isInValidateVector(v)) {
    throw std::runtime_error("an vector cannot have zero length");
  }
  return v;
}

/**
 *
 * @param Vector v
 * @returns number
 */
auto getLength(lugo::Vector v) -> int
{
  return std::hypot(v.x(), v.y());
}

/**
 *
 * @param Vector v
 * @param number scale
 * @returns Vector
 */
auto getScaledVector(lugo::Vector v, int scale) -> lugo::Vector
{
  if (scale <= 0) {
    throw std::runtime_error("vector can not have zero length");
  }
  auto v2 = lugo::Vector();
  v2.set_x(v.x() * scale);
  v2.set_y(v.y() * scale);
  return v2;
}

/**
 *
 * @param Vector v
 * @returns Vector a new vector with same direction but normalized to 0-100
 */
auto normalize(lugo::Vector v) -> lugo::Vector
{
  auto const length = getLength(v);
  return getScaledVector(v, 100 / length);
}

/**
 *
 * @param Vector originalV
 * @param Vector subV
 * @returns Vector
 */
auto subVector(lugo::Vector originalV, lugo::Vector subV) -> lugo::Vector
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

auto distanceBetweenPoints(lugo::Point a, lugo::Point b) -> int
{
  return std::hypot(a.x() - b.x(), a.y() - b.y());
}