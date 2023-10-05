#pragma once

#include <lugo/configurator.hpp>
#include <lugo/geo.hpp>
#include <lugo/goal.hpp>
#include <lugo/mapper.hpp>
#include <lugo/orientation.hpp>
#include <lugo/specs.hpp>
#include <lugo/stub.hpp>

/**
 * TODO: Should be described.
 */
enum class DIRECTION {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  BACKWARD_LEFT,
  BACKWARD_RIGHT,
  FORWARD_LEFT,
  FORWARD_RIGHT
};

/**
 * TODO: Should be described.
 * @param direction
 * @return
 */
std::string to_string(DIRECTION direction);

/**
 * TODO: Should be described.
 * @return
 */
auto homeGoalCenter() -> lugo::Point;

/**
 * TODO: Should be described.
 * @return
 */
auto homeGoalTopPole() -> lugo::Point;

/**
 * TODO: Should be described.
 * @return
 */
auto homeGoalBottomPole() -> lugo::Point;

/**
 * TODO: Should be described.
 * @return
 */
auto awayGoalCenter() -> lugo::Point;

/**
 * TODO: Should be described.
 * @return
 */
auto awayGoalTopPole() -> lugo::Point;

/**
 * TODO: Should be described.
 * @return
 */
auto awayGoalBottomPole() -> lugo::Point;

/**
 * TODO: Should be described.
 * @return
 */
auto awayGoal() -> Goal;

/**
 * TODO: Should be described.
 * @return
 */
auto homeGoal() -> Goal;

/**
 * TODO: Should be described.
 */
class GameSnapshotReader
{
public:
  //TODO: Should be private IMHO.
  lugo::Team_Side mySide;     ///
  lugo::GameSnapshot snapshot;///

  /**
   *  TODO: Should be described.
   * @param snapshot
   * @param mySide
   */
  GameSnapshotReader(lugo::GameSnapshot snapshot,
                     lugo::Team_Side mySide);

  /**
   *  TODO: Should be described.
   * @return
   */
  auto getMyTeam() -> lugo::Team;

   /**
    *  TODO: Should be described.
    * @return
    */
  auto getOpponentTeam() -> lugo::Team;

  /**
   *  TODO: Should be described.
   * @param side
   * @return
   */
  auto getTeam(lugo::Team_Side side) const -> lugo::Team;

  /**
   *  TODO: Should be described.
   * @param player
   * @return
   */
  auto isBallHolder(lugo::Player const& player) -> bool;

  /**
   *  TODO: Should be described.
   * @return
   */
  auto getOpponentSide() const -> lugo::Team_Side;

 /**
  *  TODO: Should be described.
  * @return
  */
  auto getMyGoal() const -> Goal;

 /**
  *  TODO: Should be described.
  * @return
  */
  auto getBall() const -> lugo::Ball;

  /**
   *  TODO: Should be described.
   * @return
   */
  auto getOpponentGoal() const -> Goal;

  /**
   *  TODO: Should be described.
   * @param side
   * @param number
   * @return
   */
  auto getPlayer(lugo::Team::Side side,
                 int number) const -> lugo::Player;

  /**
   *  TODO: Should be described.
   * @param origin
   * @param target
   * @return
   */
  auto makeOrderMoveMaxSpeed(lugo::Point origin,
                             lugo::Point target) -> lugo::Order;

  /**
   *  TODO: Should be described.
   * @param origin
   * @param target
   * @param speed
   * @return
   */
  auto makeOrderMove(lugo::Point origin,
                     lugo::Point target,
                     int speed) -> lugo::Order;

  /**
   *  TODO: Should be described.
   * @param direction
   * @param speed
   * @return
   */
  static auto makeOrderMoveFromVector(lugo::Vector direction,
                                      int speed) -> lugo::Order;

  /**
   *  TODO: Should be described.
   * @param direction
   * @return
   */
  auto makeOrderMoveByDirection(DIRECTION direction) -> lugo::Order;

  /**
   *  TODO: Should be described.
   * @param origin
   * @param target
   * @param speed
   * @return
   */
  auto makeOrderJump(lugo::Point const& origin,
                     lugo::Point const& target,
                     int speed) -> lugo::Order;

  /**
   *  TODO: Should be described.
   * @param ball
   * @param target
   * @param speed
   * @return
   */
  static auto makeOrderKick(lugo::Ball const& ball,
                            lugo::Point target,
                            int speed) -> lugo::Order;

  /**
   *  TODO: Should be described.
   * @param ball
   * @param target
   * @return
   */
  static auto makeOrderKickMaxSpeed(lugo::Ball ball,
                                    lugo::Point target) -> lugo::Order;

  /**
   *  TODO: Should be described.
   * @return
   */
  static auto makeOrderCatch() -> lugo::Order;
};

/**
 * TODO: Should be described.
 * @param snapshot
 * @param playerNumber
 * @param side
 * @return
 */
auto defineState(lugo::GameSnapshot const& snapshot,
                 int playerNumber,
                 lugo::Team_Side side) -> PLAYER_STATE;