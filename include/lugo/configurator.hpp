#pragma once

#include <lugo/specs.hpp>

#include <pb/server.pb.h>

/**
 * TODO: Should be described.
 */
class EnvVarLoader
{
private:
  std::string _grpcUrl; /// TODO: Should be described.
  bool _grpcInsecure{}; /// TODO: Provide default value.
  lugo::Team::Side _botTeamSide{}; /// TODO: Provide default value
  int _botNumber{}; /// TODO: Provide default value.
  std::string _botToken; /// TODO: Should be described.

public:
  /**
   * TODO: Should be described.
   */
  EnvVarLoader();

  /**
   * TODO: Should be described.
   * @return
   */
  auto getGrpcUrl() const -> std::string;

  /**
   * TODO: Should be described.
   * @return
   */
  bool getGrpcInsecure() const;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getBotTeamSide() const -> lugo::Team_Side;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getBotNumber() const -> int;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getBotToken() const -> std::string;
};
