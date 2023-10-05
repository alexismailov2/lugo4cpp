#pragma once

#include <pb/server.pb.h>

enum class PLAYER_STATE {
    SUPPORTING,
    HOLDING_THE_BALL,
    DEFENDING,
    DISPUTING_THE_BALL
};

/**
 * TODO: Should be described.
 * @param playerState
 * @return
 */
std::string to_string(PLAYER_STATE playerState);

/**
 * TODO: Should be described.
 */
class Bot
{
public:
  /**
   * Necessary if planned to derive from this class.
   */
  virtual ~Bot() = default;

  /**
   * onDisputing is called when no one has the ball possession
   * @param orderSet orderSet
   * @param snapshot snapshot
   * @return OrderSet or empty
   */
  virtual auto onDisputing(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet = 0;

  /**
   * onDefending is called when an opponent player has the ball possession
   * @param orderSet orderSet
   * @param snapshot snapshot
   * @return OrderSet or empty
   */
  virtual auto onDefending(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet = 0;

  /**
   * onHolding is called when this bot has the ball possession
   * @param orderSet orderSet
   * @param snapshot snapshot
   * @return OrderSet or empty
   */
  virtual auto onHolding(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet = 0;

  /**
   * onSupporting is called when TODO: should be finished
   * @param orderSet orderSet
   * @param snapshot snapshot
   * @return OrderSet or empty
   */
  virtual auto onSupporting(lugo::OrderSet orderSet, lugo::GameSnapshot snapshot) -> lugo::OrderSet = 0;

  /**
   * asGoalkeeper is only called when this bot is the goalkeeper (number 1). This method is called on every turn,
   * and the player state is passed at the last parameter.
   * @param orderSet orderSet
   * @param snapshot snapshot
   * @param PLAYER_STATE state
   * @returns OrderSet or empty
   */
  virtual auto asGoalkeeper(lugo::OrderSet orderSet,
                            lugo::GameSnapshot snapshot,
                            PLAYER_STATE state) -> lugo::OrderSet = 0;

  /**
   * gettingReady is called when the game is on Getting Ready state.
   *
   * @param snapshot
   */
  virtual void gettingReady(lugo::GameSnapshot snapshot) {};
};