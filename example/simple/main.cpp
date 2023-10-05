#include <lugo/client.hpp>

#include "my_bot.hpp"

#include <iostream>

Client* lugoClientPtr = nullptr;
auto signalHandler(int signum)
{
  std::cout << "Stop requested\n" << std::endl;
  if (lugoClientPtr)
  {
    lugoClientPtr->stop();
  }
  exit(signum);
}

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
  auto map = std::make_shared<Mapper>(10, 6, config. getBotTeamSide());

  // our bot strategy defines our bot initial position based on its number
  auto initialRegion = map->getRegion(PLAYER_POSITIONS[config.getBotNumber() - 1].Col,
                                      PLAYER_POSITIONS[config.getBotNumber() - 1].Row);

  // now we can create the bot. We will use a shortcut to create the client from the config, but we could use the
  // client constructor as well
  auto lugoClient = NewClientFromConfig(config, initialRegion.getCenter());
  auto myBot = MyBot(config.getBotTeamSide(),
                     config.getBotNumber(),
                     initialRegion.getCenter(),
                     map);

  lugoClient->playAsBot(myBot, [](){
    std::cout << "I may run it when the bot is connected to the server" << std::endl;
  });
  std::cout << "We are playing!" << std::endl;

  signal(SIGINT, signalHandler);
  lugoClient->wait();
  std::cout << "bye!" << std::endl;
  return 0;
}
