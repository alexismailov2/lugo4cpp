#include <lugo/orientation.hpp>

auto ORIENTATION::EAST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(1);
  return normalize(vec);
}
auto ORIENTATION::WEST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(-1);
  return normalize(vec);
}
auto ORIENTATION::SOUTH() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_y(-1);
  return normalize(vec);
}
auto ORIENTATION::NORTH() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_y(1);
  return normalize(vec);
}
auto ORIENTATION::NORTH_EAST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(1);
  vec.set_y(1);
  return normalize(vec);
}
auto ORIENTATION::NORTH_WEST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(-1);
  vec.set_y(1);
  return normalize(vec);
}
auto ORIENTATION::SOUTH_EAST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(1);
  vec.set_y(-1);
  return normalize(vec);
}
auto ORIENTATION::SOUTH_WEST() -> lugo::Vector
{
  auto vec = lugo::Vector();
  vec.set_x(-1);
  vec.set_y(-1);
  return normalize(vec);
}
