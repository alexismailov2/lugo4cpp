#include <iostream>

//#include <lugo/mapper.hpp>
//#include <lugo/orientation.hpp>
//#include <lugo/geo.hpp>
//#include <lugo/proto_exported.hpp>
//#include <lugo/stub.hpp>
//#include <lugo/specs.hpp>
//#include <lugo/goal.hpp>
//#include <lugo/configurator.hpp>
//#include <lugo/index.hpp>
#include <lugo/client.hpp>

#include "my_bot.hpp"

int main()
{
  struct ColRow {
    int Col;
    int Row;
  };
  auto PLAYER_POSITIONS = std::vector<ColRow> {
    {0, 0},
    {1, 1},
    {2, 2},
    {2, 3},
    {1, 4},
    {3, 1},
    {3, 2},
    {3, 3},
    {3, 4},
    {4, 3},
    {4, 2}
  };

  // we must load the env vars following the standard defined by the game specs because all bots will receive the
  // arguments in the same format (env vars)
  auto config = EnvVarLoader();

  // the map will help us to see the field in quadrants (called regions) instead of working with coordinates
  auto map = Mapper(10, 6, config. getBotTeamSide());

  // our bot strategy defines our bot initial position based on its number
  auto initialRegion = map.getRegion(PLAYER_POSITIONS[config.getBotNumber()].Col,
                                     PLAYER_POSITIONS[config.getBotNumber()].Row);

  // now we can create the bot. We will use a shortcut to create the client from the config, but we could use the
  // client constructor as well
  auto lugoClient = NewClientFromConfig(config, initialRegion.getCenter());
  auto myBot = MyBot(config.getBotTeamSide(),
                     config.getBotNumber(),
                     initialRegion.getCenter(),
                     map);

  auto orderSet = lugoClient.playAsBot(myBot, [](){ std::cout << "onJoin" << std::endl; });
  std::cout << "We are playing!" << std::endl;
  return 0;
}
