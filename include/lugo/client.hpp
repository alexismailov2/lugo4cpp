#pragma once

#include <lugo/configurator.hpp>
#include <lugo/index.hpp>
#include <lugo/stub.hpp>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/client_context.h>

#include <pb/physics.pb.h>
#include <pb/server.pb.h>
#include <pb/server.grpc.pb.h>

#include <functional>
#include <future>
using grpc::ClientContext;
auto const PROTOCOL_VERSION = "1.0.0";

using RawTurnProcessor = std::function<lugo::OrderSet(lugo::OrderSet, lugo::GameSnapshot)>;

/**
 * TODO: Should be described.
 */
class Client
{
private:
  std::string _serverAdd; /// TODO: Should be described.
  bool _grpc_insecure{}; /// TODO: Should be described.
  std::string _token; /// TODO: Should be described.
  lugo::Team_Side _teamSide; /// TODO: Should be described.
  int _number{}; /// TODO: Should be described.
  lugo::Point _init_position; /// TODO: Should be described.
  std::unique_ptr<lugo::Game::Stub> _client; /// TODO: Should be described.
  grpc::ClientContext _context; /// TODO: Should be described.
  std::function<void(lugo::GameSnapshot)> _gettingReadyHandler; /// TODO: Should be described.
  std::shared_ptr<grpc::Channel> _channel; /// TODO: Should be described.
  std::atomic_bool _play_finished;
  //std::shared_ptr<std::promise<bool>> _play_finished; /// TODO: Should be described.
  //std::thread _play_routine; /// TODO: Should be described.

public:
  /**
   * TODO: Should be described.
   * @param server_add {string}
   * @param grpc_insecure {boolean}
   * @param token {string}
   * @param teamSide {number}
   * @param number {number}
   * @param init_position {Point}
   * @return {Promise<void>}
   */
  Client(std::string server_add,
         bool grpc_insecure,
         std::string token,
         lugo::Team_Side teamSide,
         int number,
         lugo::Point init_position);

  /**
   * TODO: Should be described.
   * @return
   */
  auto get_name() -> std::string;

  /**
   * TODO: Should be described.
   * @param init_position
   */
  void set_initial_position(lugo::Point init_position);

  /**
   * TODO: Should be described.
   * @param snapshot
   */
  void getting_ready_handler(lugo::GameSnapshot snapshot);

  /**
   * TODO: Should be described.
   * @param handler
   */
  void set_getting_ready_handler(std::function<void(lugo::GameSnapshot)> handler);

  /**
   * TODO: Should be described.
   * @param raw_processor
   * @param onJoin
   */
  void play(RawTurnProcessor raw_processor, std::function<void()> onJoin = {});

  /**
   * TODO: Should be described.
   * @param bot
   * @param onJoin
   */
  void playAsBot(Bot& bot, std::function<void()> onJoin);

 /**
  * TODO: Should be described.
  * @param processor
  * @param onJoin callback
  * @return
  */
  void _bot_start(RawTurnProcessor processor, std::function<void()> onJoin);

  /**
   * TODO: Should be described.
   */
  void stop();

  /**
   * TODO: Should be described.
   */
  void wait();

  /**
   * TODO: Should be described.
   * @param reader
   * @param processor
   */
  void _response_watcher(std::unique_ptr<::grpc::ClientReader<::lugo::GameSnapshot>> reader,
                         RawTurnProcessor processor);
};

/**
 * TODO: Should be described.
 * @param {EnvVarLoader} config
 * @param {Point} initialPosition
 * @returns {Client}
 */
auto NewClientFromConfig(EnvVarLoader const& config, lugo::Point initialPosition) -> std::unique_ptr<Client>;