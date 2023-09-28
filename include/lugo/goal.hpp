#pragma once

#include <physics.pb.h>
#include <server.pb.h>

class Goal
{
private:
  lugo::Point _center;
  lugo::Team_Side _place;
  lugo::Point _topPole;
  lugo::Point _bottomPole;

public:
  /**
   *
   * @param place
   * @param center
   * @param topPole
   * @param bottomPole
   */
  Goal(lugo::Team_Side place,
       lugo::Point center,
       lugo::Point topPole,
       lugo::Point bottomPole)
    : _center{center}
    , _place{place}
    , _topPole{topPole}
    , _bottomPole{bottomPole}
  {
  }

  auto getCenter() -> lugo::Point { return _center; }
  auto getPlace() -> lugo::Team_Side { return _place; }
  auto getTopPole() -> lugo::Point { return _topPole; }
  auto getBottomPole() -> lugo::Point { return _bottomPole; }
};