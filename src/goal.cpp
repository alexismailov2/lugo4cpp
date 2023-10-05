#include <lugo/goal.hpp>

Goal::Goal(lugo::Team_Side place, lugo::Point center, lugo::Point topPole, lugo::Point bottomPole)
  : _place{place}
  , _center{std::move(center)}
  , _topPole{std::move(topPole)}
  , _bottomPole{std::move(bottomPole)}
{
}

auto Goal::getCenter() -> lugo::Point
{
  return _center;
}

auto Goal::getPlace() -> lugo::Team_Side
{
  return _place;
}

auto Goal::getTopPole() -> lugo::Point
{
  return _topPole;
}

auto Goal::getBottomPole() -> lugo::Point
{
  return _bottomPole;
}
