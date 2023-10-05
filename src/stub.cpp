#include <lugo/stub.hpp>

std::string to_string(PLAYER_STATE playerState)
{
  switch(playerState)
  {
    case PLAYER_STATE::SUPPORTING:
      return "supporting";
    case PLAYER_STATE::HOLDING_THE_BALL:
      return "holding";
    case PLAYER_STATE::DEFENDING:
      return "defending";
    case PLAYER_STATE::DISPUTING_THE_BALL:
    default:
      return "disputing";
  }
}
