#pragma once

#include <pb/physics.pb.h>
#include <pb/server.pb.h>

/**
 * TODO: Should be described.
 */
class Goal
{
private:
  lugo::Team_Side _place; /// TODO: Should be described.
  lugo::Point _center; /// TODO: Should be described.
  lugo::Point _topPole; /// TODO: Should be described.
  lugo::Point _bottomPole; /// TODO: Should be described.

public:
  /**
   * TODO: Should be described.
   * @param place
   * @param center
   * @param topPole
   * @param bottomPole
   */
  Goal(lugo::Team_Side place,
       lugo::Point center,
       lugo::Point topPole,
       lugo::Point bottomPole);

  /**
   * TODO: Should be described.
   * @return
   */
  auto getCenter() -> lugo::Point;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getPlace() -> lugo::Team_Side;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getTopPole() -> lugo::Point;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getBottomPole() -> lugo::Point;
};