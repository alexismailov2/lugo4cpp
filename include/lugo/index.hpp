#pragma once

//#include <lugo/client.hpp>
#include <lugo/configurator.hpp>
#include <lugo/geo.hpp>
#include <lugo/goal.hpp>
#include <lugo/mapper.hpp>
#include <lugo/orientation.hpp>
#include <lugo/proto_exported.hpp>
#include <lugo/specs.hpp>
#include <lugo/stub.hpp>
//  import * as rl from "./rl/index"
// imports actually used in this file

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

std::string to_string(DIRECTION direction) {
  switch(direction) {
    case DIRECTION::FORWARD:
      return "DIRECTION::FORWARD";
    case DIRECTION::BACKWARD:
      return "DIRECTION::BACKWARD";
    case DIRECTION::LEFT:
      return "DIRECTION::LEFT";
    case DIRECTION::RIGHT:
      return "DIRECTION::RIGHT";
    case DIRECTION::BACKWARD_LEFT:
      return "DIRECTION::BACKWARD_LEFT";
    case DIRECTION::BACKWARD_RIGHT:
      return "DIRECTION::BACKWARD_RIGHT";
    case DIRECTION::FORWARD_LEFT:
      return "DIRECTION::FORWARD_LEFT";
    case DIRECTION::FORWARD_RIGHT:
      return "DIRECTION::FORWARD_RIGHT";
  }
}

auto const homeGoalCenter = []() -> lugo::Point {
  auto homeGoalCenter = lugo::Point();
  homeGoalCenter.set_x(0);
  homeGoalCenter.set_y(SPECS::MAX_Y_COORDINATE / 2);
  return homeGoalCenter;
};

auto const homeGoalTopPole = []() -> lugo::Point {
  auto homeGoalTopPole = lugo::Point();
  homeGoalTopPole.set_x(0);
  homeGoalTopPole.set_y(SPECS::GOAL_MAX_Y);
  return homeGoalTopPole;
};

auto const homeGoalBottomPole = []() -> lugo::Point {
  auto homeGoalBottomPole = lugo::Point();
  homeGoalBottomPole.set_x(0);
  homeGoalBottomPole.set_y(SPECS::GOAL_MIN_Y);
  return homeGoalBottomPole;
};

auto const awayGoalCenter = []() -> lugo::Point {
  auto awayGoalCenter = lugo::Point();
  awayGoalCenter.set_x(SPECS::MAX_X_COORDINATE);
  awayGoalCenter.set_y(SPECS::MAX_Y_COORDINATE / 2);
  return awayGoalCenter;
};

auto const awayGoalTopPole = []() -> lugo::Point {
  auto awayGoalTopPole = lugo::Point();
  awayGoalTopPole.set_x(SPECS::MAX_X_COORDINATE);
  awayGoalTopPole.set_y(SPECS::GOAL_MAX_Y);
  return awayGoalTopPole;
};

auto const awayGoalBottomPole = []() -> lugo::Point {
  auto awayGoalBottomPole = lugo::Point();
  awayGoalBottomPole.set_x(SPECS::MAX_X_COORDINATE);
  awayGoalBottomPole.set_y(SPECS::GOAL_MIN_Y);
  return awayGoalBottomPole;
};

auto const awayGoal = Goal(lugo::Team_Side_AWAY, awayGoalCenter(), awayGoalTopPole(), awayGoalBottomPole());
auto const homeGoal = Goal(lugo::Team_Side_HOME, homeGoalCenter(), homeGoalTopPole(), homeGoalBottomPole());

class GameSnapshotReader
{
public:
  lugo::Team_Side mySide;     ///
  lugo::GameSnapshot snapshot;///

  GameSnapshotReader(lugo::GameSnapshot snapshot,
                     lugo::Team_Side mySide)
      : snapshot{std::move(snapshot)}
      , mySide{std::move(mySide)}
  {}

  /**
   * Returns the bot team
   * @returns {Lugo.Team}
   */
  auto getMyTeam() -> lugo::Team { return getTeam(mySide); }

  /**
     * Returns the opponent team
     * @returns lugo::Team
     */
  auto getOpponentTeam() -> lugo::Team { return getTeam(getOpponentSide()); }

  /**
   * @param lugo::Team_Side side
   * @returns lugo::Team
   */
  auto getTeam(lugo::Team_Side side) const -> lugo::Team
  {
      return (side == lugo::Team_Side_HOME) ? snapshot.home_team() : snapshot.away_team();
  }


  /**
   *
   * @param Player player
   * @returns bool
   */
  auto isBallHolder(lugo::Player player) -> bool
  {
    auto ball = snapshot.ball();
    return ball.has_holder() &&
           ball.holder().team_side() == player.team_side() &&
           ball.holder().number() == player.number();
  }

  /**
   *
   * @returns lugo::Team_Side
   */
  auto getOpponentSide() -> lugo::Team_Side
  {
      return (mySide == lugo::Team_Side_HOME) ? lugo::Team_Side_AWAY : lugo::Team_Side_HOME;
  }

  /**
   *
   * @returns Goal
   */
  auto getMyGoal() -> Goal
  {
    return (mySide == lugo::Team_Side_HOME) ? homeGoal : awayGoal;
  }

  /**
   *
   * @returns lugo::Ball
   */
  auto getBall() -> lugo::Ball
  {
    return snapshot.ball();
  }

  /**
     *
     * @returns {Goal}
     */
  auto getOpponentGoal() -> Goal
  {
    return (mySide == lugo::Team_Side_HOME) ? awayGoal : homeGoal;
  }

  /**
   *
   * @param {.Lugo.Team.Side} side
   * @param {number} number
   * @returns {.Player}
   */
  auto getPlayer(lugo::Team::Side side, int number) const -> lugo::Player
  {
    auto const team = getTeam(side);
    if (!team.IsInitialized())
    {
      return {};
    }
    for (auto const& player : team.players())
    {
      if (player.number() == number)
      {
        return player;
      }
    }
    return {};
  }

  /**
   *
   * @param {Point} origin
   * @param {Point} target
   * @return {Order}
   */
  auto makeOrderMoveMaxSpeed(lugo::Point origin, lugo::Point target) -> lugo::Order
  {
    return makeOrderMove(std::move(origin), std::move(target), SPECS::PLAYER_MAX_SPEED);
  }

  /**
   *
   * @param {Point} origin
   * @param {Point} target
   * @param speed
   * @returns {Order}
   */
  auto makeOrderMove(lugo::Point origin, lugo::Point target, int speed) -> lugo::Order
  {
    if (origin.x() == target.x() &&
        origin.y() == target.y())
    {
      // a vector cannot have zeroed direction. In this case, the player will just be stopped
      return makeOrderMoveFromVector(ORIENTATION::NORTH(), 0);
    }

    auto direction = NewVector(std::move(origin), std::move(target));
    direction = normalize(direction);
    return makeOrderMoveFromVector(direction, speed);
  }

  /**
     *
     * @param {Vector} direction
     * @param {number} speed
     * @returns {Order}
     * @private
     */
  auto makeOrderMoveFromVector(lugo::Vector direction, int speed) -> lugo::Order
  {
    auto velocity = lugo::Velocity();
    *velocity.mutable_direction() = std::move(direction);
    velocity.set_speed(speed);

    auto moveOrder = lugo::Move();
    *moveOrder.mutable_velocity() = velocity;
    auto order = lugo::Order();
    *order.mutable_move() = moveOrder;
    return order;
  }

  auto makeOrderMoveByDirection(DIRECTION direction) -> lugo::Order
  {
    lugo::Vector directionTarget;
    switch (direction) {
      case DIRECTION::FORWARD:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::WEST() : ORIENTATION::EAST();
        break;
      case DIRECTION::BACKWARD:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::EAST() : ORIENTATION::WEST();
        break;
      case DIRECTION::LEFT:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::SOUTH() : ORIENTATION::NORTH();
        break;
      case DIRECTION::RIGHT:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::NORTH() : ORIENTATION::SOUTH();
        break;
      case DIRECTION::BACKWARD_LEFT:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::SOUTH_EAST() : ORIENTATION::NORTH_WEST();
        break;
      case DIRECTION::BACKWARD_RIGHT:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::NORTH_EAST() : ORIENTATION::SOUTH_WEST();
        break;
      case DIRECTION::FORWARD_LEFT:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::SOUTH_WEST() : ORIENTATION::NORTH_EAST();
        break;
      case DIRECTION::FORWARD_RIGHT:
        directionTarget = (mySide == lugo::Team_Side_AWAY) ? ORIENTATION::NORTH_WEST() : ORIENTATION::SOUTH_EAST();
        break;
      default:
        throw std::runtime_error(std::string("unknown direction ") + to_string(direction));
    }
    return makeOrderMoveFromVector(directionTarget, SPECS::PLAYER_MAX_SPEED);
  }


  auto makeOrderJump(lugo::Point const& origin, lugo::Point const& target, int speed) -> lugo::Order
  {
    auto direction = ORIENTATION::EAST();
    if (origin.x() != target.x() ||
        origin.y() != target.y())
    {
      // a vector cannot have zeroed direction. In this case, the player will just be stopped
      direction = NewVector(origin, target);
      direction = normalize(direction);
    }
    auto velocity = lugo::Velocity();
    *velocity.mutable_direction() = direction;
    velocity.set_speed(speed);

    auto jump = lugo::Jump();
    *jump.mutable_velocity() = velocity;

    auto order = lugo::Order();
    *order.mutable_jump() = jump;
    return order;
  }

  /**
     *
     * @param {Ball} ball
     * @param {Point} target
     * @param {number} speed
     * @returns {Order}
     */
  auto makeOrderKick(lugo::Ball const& ball, lugo::Point target, int speed) -> lugo::Order
  {
    auto const ballExpectedDirection = NewVector(ball.position(), std::move(target));

    // the ball velocity is summed to the kick velocity, so we have to consider the current ball direction
    auto const diffVector = subVector(ballExpectedDirection, ball.velocity().direction());

    auto newVelocity = lugo::Velocity();
    newVelocity.set_speed(speed);
    *newVelocity.mutable_direction() = normalize(diffVector);

    auto kick = lugo::Kick();
    *kick.mutable_velocity() = newVelocity;

    auto order = lugo::Order();
    *order.mutable_kick() = kick;
    return order;
  }

  /**
   *
   * @param {Ball} ball
   * @param {Point} target
   * @returns {Order}
   */
  auto makeOrderKickMaxSpeed(lugo::Ball ball, lugo::Point target) -> lugo::Order
  {
    return makeOrderKick(std::move(ball), std::move(target), SPECS::BALL_MAX_SPEED);
  }

  /**
   *
   * @returns {!Order}
   */
  auto makeOrderCatch() -> lugo::Order
  {
    auto order = lugo::Order();
    *order.mutable_catch_() = lugo::Catch();
    return order;
  }
};

/**
 *
 * @param {GameSnapshot}  snapshot
 * @param playerNumber
 * @param side
 * @returns {PLAYER_STATE}
 */
inline auto defineState(lugo::GameSnapshot const& snapshot,
                        int playerNumber,
                        lugo::Team_Side side) -> PLAYER_STATE
{
  if (!snapshot.IsInitialized() || !snapshot.has_ball())
  {
    throw std::runtime_error("invalid snapshot state - cannot define player state");
  }

  auto const reader = GameSnapshotReader(snapshot, side);
  auto const me = reader.getPlayer(side, playerNumber);
  if (!me.IsInitialized())
  {
    throw std::runtime_error("could not find the bot in the snapshot - cannot define player state");
  }

  auto const ballHolder = snapshot.ball().holder();
  if (!ballHolder.IsInitialized())
  {
    return PLAYER_STATE::DISPUTING_THE_BALL;
  }
  else if (ballHolder.team_side() == side)
  {
    if (ballHolder.number() == playerNumber)
    {
      return PLAYER_STATE::HOLDING_THE_BALL;
    }
    return PLAYER_STATE::SUPPORTING;
  }
  return PLAYER_STATE::DEFENDING;
}