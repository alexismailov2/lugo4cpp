#pragma once

auto const BASE_UNIT = 100;

constexpr auto const max_x_coordinate = 200 * BASE_UNIT;
constexpr auto const max_y_coordinate = 100 * BASE_UNIT;
constexpr auto const goal_width = 30 * BASE_UNIT;
constexpr auto const player_max_speed = 100.0;

struct SPECS
{
  // PLAYER_SIZE is the size of each player
  static auto const PLAYER_SIZE = 4 * BASE_UNIT;

  // PLAYER_RECONNECTION_WAIT_TIME is a penalty time imposed to the player that needs to reconnect during the match.
  // this interval ensure players won't drop connection in purpose to be reallocated to their initial position.
  static auto const PLAYER_RECONNECTION_WAIT_TIME = 20;

  // max number of players in a team by mach
  static auto const MAX_PLAYERS = 11;

  // min number of players in a team by mach, if a team gets to have less to this number, the team loses by W.O.
  static auto const MIN_PLAYERS = 6;

  // PLAYER_MAX_SPEED is the max speed that a play may move  by frame
  constexpr static auto const PLAYER_MAX_SPEED = player_max_speed;

  // MAX_X_COORDINATE help us to remember that we should not use the field width to check the field boundaries!
  // The field dimensions counts the total number of coordinates, but the coordinates are zero-indexed.
  // e.g. If the field width is 10, so the max coordinate will be 9
  static auto const MAX_X_COORDINATE = max_x_coordinate;

  // MAX_Y_COORDINATE help us to remember that we should not use the field width to check the field boundaries!
  // The field dimensions counts the total number of coordinates, but the coordinates are zero-indexed.
  // e.g. If the field height is 10, so the max coordinate will be 9
  static auto const MAX_Y_COORDINATE = max_y_coordinate;

  // FIELD_WIDTH is the width of the field (horizontal view)
  // This value must be an odd number because the central coordinate must be neutral, and we want to have the same number
  // of coordinates on both sides.
  // e.g. If the field width is 10, and the coordinates go from 0 to 9, there is no precise middle.
  // Thus, the field width would have to be 11, so the coordinate 5 is at the precise center
  static auto const FIELD_WIDTH = max_x_coordinate + 1;

  // FIELD_HEIGHT is the height of the field (horizontal view)
  // This value must be an odd number because we cant to a coordinate at the perfect middle of the field
  // e.g. If the field height is 10, and the coordinates go from 0 to 9, there is no precise middle.
  // Thus, the field height would have to be 11, so the coordinate 5 is at the precise center
  static auto const FIELD_HEIGHT = max_y_coordinate + 1;

  // FIELD_NEUTRAL_CENTER is the radius of the neutral circle on the center of the field
  static auto const FIELD_NEUTRAL_CENTER = 100;

  // BALL_SIZE size of the element ball
  static auto const BALL_SIZE = 2 * BASE_UNIT;

  // BALL_DECELERATION is the deceleration rate of the ball speed  by frame
  constexpr static auto const BALL_DECELERATION = 10.0;

  // BALL_MAX_SPEED is the max speed of the ball by frame
  constexpr static auto const BALL_MAX_SPEED = 4.0 * BASE_UNIT;

  // BALL_MIN_SPEED is the minimal speed of the ball  by frame. When the ball was at this speed or slower, it will be considered stopped.
  static auto const BALL_MIN_SPEED = 2;

  // BALL_TIME_IN_GOAL_ZONE is the max number of turns that the ball may be in a goal zone. After that, the ball will be auto kicked
  // towards the center of the field.
  static auto const BALL_TIME_IN_GOAL_ZONE = 40; // 40 / 20 fps : 2 seconds,

  // GOAL_WIDTH is the goal width
  static auto const GOAL_WIDTH = goal_width;

  // GOAL_MIN_Y is the coordinate Y of the lower pole of the goals
  static auto const GOAL_MIN_Y = (max_y_coordinate - goal_width) / 2;

  // GOAL_MAX_Y is the coordinate Y of the upper pole of the goals
  static auto const GOAL_MAX_Y = ((max_y_coordinate - goal_width) / 2) + goal_width;

  // GOAL_ZONE_RANGE is the minimal distance that a player can stay from the opponent goal
  static auto const GOAL_ZONE_RANGE = 14 * BASE_UNIT;

  // GOAL_KEEPER_JUMP_DURATION is the number of turns that the jump takes. A jump cannot be interrupted after has been requested
  static auto const GOAL_KEEPER_JUMP_DURATION = 3;

  // GOAL_KEEPER_JUMP_SPEED is the max speed of the goalkeeper during the jump
  constexpr static auto const GOAL_KEEPER_JUMP_SPEED = 2 * player_max_speed;

  // GOALKEEPER_NUMBER defines the goalkeeper number
  static auto const GOALKEEPER_NUMBER = 1;

  // Number of turns each teams has on attack before losing the ball possession.
  static auto const SHOT_CLOCK_TIME = 300;
};