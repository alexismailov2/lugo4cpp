#pragma once

#include <physics.pb.h>
#include <lugo/geo.hpp>

namespace ORIENTATION {
inline auto EAST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(1);
  return normalize(vec);
}

inline auto WEST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(-1);
  return normalize(vec);
}

inline auto SOUTH() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_y(-1);
  return normalize(vec);
}

inline auto NORTH() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_y(1);
  return normalize(vec);
}

inline auto NORTH_EAST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(1);
  vec.set_y(1);
  return normalize(vec);
}

inline auto NORTH_WEST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(-1);
  vec.set_y(1);
  return normalize(vec);
}

inline auto SOUTH_EAST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(1);
  vec.set_y(-1);
  return normalize(vec);
}

inline auto SOUTH_WEST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(-1);
  vec.set_y(-1);
  return normalize(vec);
}
}