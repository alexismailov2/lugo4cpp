#include <lugo/configurator.hpp>

EnvVarLoader::EnvVarLoader()
{
  if (!std::getenv("BOT_TEAM")) {
    throw std::runtime_error("missing BOT_TEAM env value");
  }

  if (!std::getenv("BOT_NUMBER")) {
    throw std::runtime_error("missing BOT_NUMBER env value");
  }

  // the Lugo address
  auto botGrpcUrl = std::getenv("BOT_GRPC_URL");
  _grpcUrl = botGrpcUrl ? botGrpcUrl :  "localhost:5000";
  auto botGrpcInsecure = std::getenv("BOT_GRPC_INSECURE");
  _grpcInsecure = botGrpcInsecure && (std::string(botGrpcInsecure) == "true");

  // defining bot side
  auto botTeam = std::getenv("BOT_TEAM");
  if (!lugo::Team_Side_Parse(botTeam, &_botTeamSide)) {
    throw std::runtime_error("BOT_TEAM is not set");
  }
  auto bot_number = std::getenv("BOT_NUMBER");
  _botNumber = std::atoi(bot_number);
  if (_botNumber < 1 || _botNumber > SPECS::MAX_PLAYERS) {
    throw std::runtime_error(std::string("invalid bot number") + std::to_string(_botNumber) + ", must be between 1 and " + std::to_string(SPECS::MAX_PLAYERS));
  }

  // the token is mandatory in official matches, but you may ignore in local games
  auto bot_token = std::getenv("BOT_TOKEN");
  _botToken =  bot_token ? bot_token : "";
}
auto EnvVarLoader::getGrpcUrl() const -> std::string
{
  return _grpcUrl;
}
bool EnvVarLoader::getGrpcInsecure() const
{
  return _grpcInsecure;
}
auto EnvVarLoader::getBotTeamSide() const -> lugo::Team_Side
{
  return _botTeamSide;
}
auto EnvVarLoader::getBotNumber() const -> int
{
  return _botNumber;
}
auto EnvVarLoader::getBotToken() const -> std::string
{
  return _botToken;
}
