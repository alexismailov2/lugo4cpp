#pragma once

#include <lugo/index.hpp>

#include <lugo/stub.hpp>
#include <server.pb.h>

class MyBot
    : public Bot
{
public:
  lugo::Team_Side side;    ///
  int number{};            ///
  lugo::Point initPosition;///
  Mapper mapper;           ///

  /**
   *
   * @param {Lugo.Team.Side} side
   * @param {number} number
   * @param {physics.Point} initPosition
   * @param {Mapper} mapper
   */
  MyBot(lugo::Team_Side side,
        int number,
        lugo::Point initPosition,
        Mapper mapper)
      : side{side}
      , number{number}
      , initPosition{std::move(initPosition)}
      , mapper{std::move(mapper)}
  {
  }

  ~MyBot() override = default;

  /**
   *
   * @param {GameSnapshot} snapshot
   * @private
   * @return {GameSnapshotReader}
   */
  auto _makeReader(lugo::GameSnapshot snapshot) -> std::pair<GameSnapshotReader, lugo::Player>
  {
    auto reader = GameSnapshotReader(std::move(snapshot), side);
    auto me = reader.getPlayer(side, number);
    if (!me.IsInitialized())
    {
      throw std::runtime_error("did not find myself in the game");
    }
    return {reader, me};
  }

  /**
   *
   * @param orderSet
   * @param snapshot
   * @return
   */
  auto onDisputing(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet override
  {
    try
    {
      auto [reader, me] = _makeReader(snapshot);
      auto ballPosition = reader.getBall().position();
      auto ballRegion = mapper.getRegionFromPoint(ballPosition);
      auto myRegion = mapper.getRegionFromPoint(initPosition);

      auto moveDest = initPosition;
      if (std::abs(myRegion.getRow() - ballRegion.getRow()) <= 2 &&
          std::abs(myRegion.getCol() - ballRegion.getCol()) <= 2)
      {
        moveDest = ballPosition;
      }
      auto moveOrder = reader.makeOrderMoveMaxSpeed(me.position(), moveDest);
      // const catchOrder = reader.
      orderSet.set_turn(snapshot.turn());
      orderSet.set_debug_message("mi mi mi");
      std::vector<lugo::Order> list{moveOrder};
      orderSet.mutable_orders()->Add(list.begin(), list.end());
      return orderSet;
    }
    catch (std::runtime_error& e)
    {
      std::cout << "did not play this turn" << e.what() << std::endl;
    }
    return {};
  }

  auto onDefending(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet override
  {
    try
    {
      auto [reader, me] = _makeReader(snapshot);
      auto ballPosition = snapshot.ball().position();
      auto ballRegion = mapper.getRegionFromPoint(ballPosition);
      auto myRegion = mapper.getRegionFromPoint(initPosition);

      auto moveDest = initPosition;
      if (std::abs(myRegion.getRow() - ballRegion.getRow()) <= 2 &&
          std::abs(myRegion.getCol() - ballRegion.getCol()) <= 2) {
        moveDest = ballPosition;
      }
      auto moveOrder = reader.makeOrderMoveMaxSpeed(me.position(), moveDest);
      auto catchOrder = reader.makeOrderCatch();

      orderSet.set_turn(snapshot.turn());
      orderSet.set_debug_message("trying to catch the ball");
      std::vector<lugo::Order> list{moveOrder, catchOrder};
      orderSet.mutable_orders()->Add(list.begin(), list.end());
      return orderSet;
    }
    catch (std::runtime_error& e)
    {
      std::cout << "did not play this turn" << e.what() << std::endl;
    }
    return {};
  }

  auto onHolding(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet override
  {
    try {
      auto [reader, me] = _makeReader(snapshot);

      auto myGoalCenter = mapper.getRegionFromPoint(reader.getOpponentGoal().getCenter());
      auto currentRegion = mapper.getRegionFromPoint(me.position());

      lugo::Order myOrder;
      if (std::abs(currentRegion.getRow() - myGoalCenter.getRow()) <= 1 &&
          std::abs(currentRegion.getCol() - myGoalCenter.getCol()) <= 1)
      {
        myOrder = reader.makeOrderKickMaxSpeed(snapshot.ball(), reader.getOpponentGoal().getCenter());
      }
      else
      {
        myOrder = reader.makeOrderMoveMaxSpeed(me.position(), reader.getOpponentGoal().getCenter());
      }

      orderSet.set_turn(snapshot.turn());
      orderSet.set_debug_message("attack!");
      std::vector<lugo::Order> list{myOrder};
      orderSet.mutable_orders()->Add(list.begin(), list.end());
      return orderSet;
    }
    catch (std::runtime_error &e)
    {
      std::cout << "did not play this turn" << e.what() << std::endl;
    }
    return {};
  }

  auto onSupporting(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet override
  {
    try
    {
      auto [reader, me] = _makeReader(snapshot);
      auto ballHolderPosition = snapshot.ball().position();
      auto myOrder = reader.makeOrderMoveMaxSpeed(me.position(), ballHolderPosition);

      orderSet.set_turn(snapshot.turn());
      orderSet.set_debug_message("supporting");
      std::vector<lugo::Order> list;
      orderSet.mutable_orders()->Add(list.begin(), list.end());
      return orderSet;
    }
    catch (std::runtime_error &e)
    {
      std::cout << "did not play this turn " << e.what() << std::endl;
    }
    return {};
  }

  /**
   *
   * @param orderSet
   * @param snapshot
   * @param state
   * @return {OrderSet}
   */
  auto asGoalkeeper(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot, PLAYER_STATE state) -> lugo::OrderSet override
  {
    try
    {
      auto [reader, me] = _makeReader(snapshot);
      auto position = snapshot.ball().position();
      if (state != PLAYER_STATE::DISPUTING_THE_BALL)
      {
        position = reader.getMyGoal().getCenter();
      }
      auto myOrder = reader.makeOrderMoveMaxSpeed(me.position(), position);
      orderSet = lugo::OrderSet();
      orderSet.set_turn(snapshot.turn());
      orderSet.set_debug_message("supporting");
      std::vector<lugo::Order> list{myOrder, reader.makeOrderCatch()};
      orderSet.mutable_orders()->Add(list.begin(), list.end());
      return orderSet;
    }
    catch (std::runtime_error& e)
    {
      std::cout << "did not play this turn " << e.what() << std::endl;
    }
    return {};
  }
};