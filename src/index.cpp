#include <lugo/index.hpp>

auto homeGoalCenter() -> lugo::Point {
  auto homeGoalCenter = lugo::Point();
  homeGoalCenter.set_x(0);
  homeGoalCenter.set_y(SPECS::MAX_Y_COORDINATE / 2);
  return homeGoalCenter;
};

auto homeGoalTopPole() -> lugo::Point {
  auto homeGoalTopPole = lugo::Point();
  homeGoalTopPole.set_x(0);
  homeGoalTopPole.set_y(SPECS::GOAL_MAX_Y);
  return homeGoalTopPole;
};

auto homeGoalBottomPole() -> lugo::Point {
  auto homeGoalBottomPole = lugo::Point();
  homeGoalBottomPole.set_x(0);
  homeGoalBottomPole.set_y(SPECS::GOAL_MIN_Y);
  return homeGoalBottomPole;
};

auto awayGoalCenter() -> lugo::Point {
  auto awayGoalCenter = lugo::Point();
  awayGoalCenter.set_x(SPECS::MAX_X_COORDINATE);
  awayGoalCenter.set_y(SPECS::MAX_Y_COORDINATE / 2);
  return awayGoalCenter;
};

auto awayGoalTopPole() -> lugo::Point {
  auto awayGoalTopPole = lugo::Point();
  awayGoalTopPole.set_x(SPECS::MAX_X_COORDINATE);
  awayGoalTopPole.set_y(SPECS::GOAL_MAX_Y);
  return awayGoalTopPole;
};

auto awayGoalBottomPole() -> lugo::Point {
  auto awayGoalBottomPole = lugo::Point();
  awayGoalBottomPole.set_x(SPECS::MAX_X_COORDINATE);
  awayGoalBottomPole.set_y(SPECS::GOAL_MIN_Y);
  return awayGoalBottomPole;
};

auto awayGoal() -> Goal
{
  return Goal(lugo::Team_Side_AWAY, awayGoalCenter(), awayGoalTopPole(), awayGoalBottomPole());
}

auto homeGoal() -> Goal
{
  return Goal(lugo::Team_Side_HOME, homeGoalCenter(), homeGoalTopPole(), homeGoalBottomPole());
}

std::string to_string(DIRECTION direction)
{
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

auto defineState(const lugo::GameSnapshot &snapshot, int playerNumber, lugo::Team_Side side) -> PLAYER_STATE
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

  if (!snapshot.ball().has_holder())
  {
    return PLAYER_STATE::DISPUTING_THE_BALL;
  }
  auto const ballHolder = snapshot.ball().holder();
  if (ballHolder.team_side() == side)
  {
    if (ballHolder.number() == playerNumber)
    {
      return PLAYER_STATE::HOLDING_THE_BALL;
    }
    return PLAYER_STATE::SUPPORTING;
  }
  return PLAYER_STATE::DEFENDING;
}

GameSnapshotReader::GameSnapshotReader(lugo::GameSnapshot snapshot, lugo::Team_Side mySide)
  : snapshot{std::move(snapshot)}
  , mySide{mySide}
{
}

auto GameSnapshotReader::getMyTeam() -> lugo::Team
{
  return getTeam(mySide);
}

auto GameSnapshotReader::getOpponentTeam() -> lugo::Team
{
  return getTeam(getOpponentSide());
}

auto GameSnapshotReader::getTeam(lugo::Team_Side side) const -> lugo::Team
{
  return (side == lugo::Team_Side_HOME) ? snapshot.home_team() : snapshot.away_team();
}

auto GameSnapshotReader::isBallHolder(const lugo::Player &player) -> bool
{
  auto ball = snapshot.ball();
  return ball.has_holder() &&
         ball.holder().team_side() == player.team_side() &&
         ball.holder().number() == player.number();
}

auto GameSnapshotReader::getOpponentSide() const -> lugo::Team_Side
{
  return (mySide == lugo::Team_Side_HOME) ? lugo::Team_Side_AWAY : lugo::Team_Side_HOME;
}

auto GameSnapshotReader::getMyGoal() const -> Goal
{
  return (mySide == lugo::Team_Side_HOME) ? homeGoal() : awayGoal();
}

auto GameSnapshotReader::getBall() const -> lugo::Ball
{
  return snapshot.ball();
}

auto GameSnapshotReader::getOpponentGoal() const -> Goal
{
  return (mySide == lugo::Team_Side_HOME) ? awayGoal() : homeGoal();
}

auto GameSnapshotReader::getPlayer(lugo::Team::Side side, int number) const -> lugo::Player
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

auto GameSnapshotReader::makeOrderMoveMaxSpeed(lugo::Point origin, lugo::Point target) -> lugo::Order
{
  return makeOrderMove(std::move(origin), std::move(target), SPECS::PLAYER_MAX_SPEED);
}

auto GameSnapshotReader::makeOrderMove(lugo::Point origin, lugo::Point target, int speed) -> lugo::Order
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

auto GameSnapshotReader::makeOrderMoveFromVector(lugo::Vector direction, int speed) -> lugo::Order
{
  auto velocity = lugo::Velocity();
  velocity.set_allocated_direction(new lugo::Vector(direction));
  //*velocity.mutable_direction() = std::move(direction);
  velocity.set_speed(speed);

  auto moveOrder = lugo::Move();
  moveOrder.set_allocated_velocity(new lugo::Velocity(velocity));
  //*moveOrder.mutable_velocity() = velocity;
  auto order = lugo::Order();
  order.set_allocated_move(new lugo::Move(moveOrder));
  //*order.mutable_move() = moveOrder;
  return order;
}

auto GameSnapshotReader::makeOrderMoveByDirection(DIRECTION direction) -> lugo::Order
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

auto GameSnapshotReader::makeOrderJump(const lugo::Point &origin, const lugo::Point &target, int speed) -> lugo::Order
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
  velocity.set_allocated_direction(new lugo::Vector(direction));
  //*velocity.mutable_direction() = direction;
  velocity.set_speed(speed);

  auto jump = lugo::Jump();
  jump.set_allocated_velocity(new lugo::Velocity(velocity));
  //*jump.mutable_velocity() = velocity;

  auto order = lugo::Order();
  order.set_allocated_jump(new lugo::Jump(jump));
  //*order.mutable_jump() = jump;
  return order;
}

auto GameSnapshotReader::makeOrderKick(const lugo::Ball &ball, lugo::Point target, int speed) -> lugo::Order
{
  auto const ballExpectedDirection = NewVector(ball.position(), std::move(target));

  // the ball velocity is summed to the kick velocity, so we have to consider the current ball direction
  auto const diffVector = subVector(ballExpectedDirection, ball.velocity().direction());

  auto newVelocity = lugo::Velocity();
  newVelocity.set_speed(speed);
  newVelocity.set_allocated_direction(new lugo::Vector(normalize(diffVector)));
  //*newVelocity.mutable_direction() = normalize(diffVector);

  auto kick = lugo::Kick();
  kick.set_allocated_velocity(new lugo::Velocity(newVelocity));
  //*kick.mutable_velocity() = newVelocity;

  auto order = lugo::Order();
  order.set_allocated_kick(new lugo::Kick(kick));
  //*order.mutable_kick() = kick;
  return order;
}

auto GameSnapshotReader::makeOrderKickMaxSpeed(lugo::Ball ball, lugo::Point target) -> lugo::Order
{
  return makeOrderKick(std::move(ball), std::move(target), SPECS::BALL_MAX_SPEED);
}

auto GameSnapshotReader::makeOrderCatch() -> lugo::Order
{
  auto order = lugo::Order();
  order.set_allocated_catch_(new lugo::Catch());
  //*order.mutable_catch_() = lugo::Catch();
  return order;
}

