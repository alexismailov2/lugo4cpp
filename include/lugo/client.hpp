#pragma once

#include <lugo/configurator.hpp>
#include <lugo/index.hpp>
#include <lugo/stub.hpp>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

#include <physics.pb.h>
#include <server.pb.h>
#include <server.grpc.pb.h>

#include <functional>
#include <future>

auto const PROTOCOL_VERSION = "1.0.0";

using RawTurnProcessor = std::function<lugo::OrderSet(lugo::OrderSet, lugo::GameSnapshot)>;

class Client
{
private:
  std::string _serverAdd;
  bool _grpc_insecure{};
  std::string _token;
  lugo::Team_Side _teamSide;
  int _number{};
  lugo::Point _init_position;
  std::unique_ptr<lugo::Game::Stub> _client;
  grpc::ClientContext _context;
  std::function<void(lugo::GameSnapshot)> _gettingReadyHandler;
  std::shared_ptr<grpc::Channel> _channel;
  std::shared_ptr<std::promise<bool>> _play_finished;
  std::thread _play_routine;

public:
  /**
   *
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
         lugo::Point init_position)
      : _serverAdd{std::move(server_add)}
      , _grpc_insecure{grpc_insecure}
      , _token{std::move(token)}
      , _teamSide{teamSide}
      , _number{number}
      , _init_position{std::move(init_position)}
  {
  }

//  void set_client(server_grpc::GameStub client)
//  {
//      _client = client;
//  }

  auto get_name() -> std::string
  {
    return std::string(!_teamSide ? "HOME" : "AWAY") + "-" + std::to_string(_number);
  }

  void set_initial_position(lugo::Point init_position)
  {
    _init_position = init_position;
  }

  void getting_ready_handler(lugo::GameSnapshot snapshot)
  {
    std::cout << "Default getting ready handler called for ";
  }

  void set_getting_ready_handler(std::function<void(lugo::GameSnapshot)> handler)
  {
    _gettingReadyHandler = std::move(handler);
  }

  void play(RawTurnProcessor raw_processor, std::function<void()> onJoin = {})
  {
    std::cout << get_name() << "Starting to play" << std::endl;
    _bot_start(std::move(raw_processor), std::move(onJoin));
  }

  //  void play(callback: Callable[[lugo.GameSnapshot], lugo.OrderSet],
  //            on_join: Callable[[], None]) -> std::future<bool>
  //  {
  //    callback = callback;
  //    std::cout << get_name() << "Starting to play" << std::endl;
  //    return _bot_start(callback, on_join);
  //  }

  void playAsBot(Bot& bot, std::function<void()> onJoin)
  {
    set_getting_ready_handler([&](lugo::GameSnapshot s) { bot.gettingReady(s); });
    std::cout << get_name() << "Playing as bot" << std::endl;

    auto processor = [&](lugo::OrderSet orders, lugo::GameSnapshot snapshot) -> lugo::OrderSet {
      auto playerState = defineState(snapshot, _number, _teamSide);
      if (_number == 1)
      {
        orders = bot.asGoalkeeper(orders, snapshot, playerState);
      }
      else
      {
        switch (playerState) {
          case PLAYER_STATE::DISPUTING_THE_BALL:
            orders = bot.onDisputing(orders, snapshot);
            break;
          case PLAYER_STATE::DEFENDING:
            orders = bot.onDefending(orders, snapshot);
            break;
          case PLAYER_STATE::SUPPORTING:
            orders = bot.onSupporting(orders, snapshot);
            break;
          case PLAYER_STATE::HOLDING_THE_BALL:
            orders = bot.onHolding(orders, snapshot);
            break;
        }
      }
      return orders;
    };
    _bot_start(processor, std::move(onJoin));
  }

 /**
  *
  * @param processor
  * @param onJoin callback
  * @return
  */
  void _bot_start(RawTurnProcessor processor, std::function<void()> onJoin)
  {
    std::cout << get_name() << " Starting bot " << _teamSide << "-" << _number << std::endl;
    _channel = _grpc_insecure
                   ? grpc::CreateChannel(_serverAdd, grpc::InsecureChannelCredentials())
                   : grpc::CreateChannel(_serverAdd, grpc::InsecureChannelCredentials());
    // TODO: Temporary secure credentials not implemented
                   //: grpc::CreateChannel(serverAdd, grpc::SecureChannelCredentials())

    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);
    _context.set_deadline(deadline);
    auto stateConnection = _channel->GetState(true);
    switch(stateConnection)
    {
        case GRPC_CHANNEL_IDLE:
          std::cout << "channel is idle" << std::endl;
          break;
        case GRPC_CHANNEL_CONNECTING:
          std::cout << "channel is connecting" << std::endl;
          break;
        case GRPC_CHANNEL_READY:
          std::cout << "channel is ready for work" << std::endl;
          break;
        case GRPC_CHANNEL_TRANSIENT_FAILURE:
          std::cout << "channel has seen a failure but expects to recover" << std::endl;
          break;
        case GRPC_CHANNEL_SHUTDOWN:
          std::cout << "channel has seen a failure that it cannot recover from" << std::endl;
          break;
    }
    _client = lugo::Game::NewStub(_channel);

//    if (err)
//    {
//      reject(Error("failed to connect to the Game Server at " + serverAdd + " : " + err));
//    }
    std::cout << (std::string("connect to the gRPC server ") + (_teamSide == lugo::Team_Side_HOME ? "HOME" : "AWAY") + " - " + std::to_string(_number)) << std::endl;

    auto joinRequest = lugo::JoinRequest();
    joinRequest.set_token(_token);
    //joinRequest.set_protocol_version(PROTOCOL_VERSION); //TODO: python not set it
    joinRequest.set_team_side(_teamSide);
    joinRequest.set_number(_number);
    *joinRequest.mutable_init_position() = _init_position;

    _play_finished = std::make_shared<std::promise<bool>>();
    auto reader = _client->JoinATeam(&_context, joinRequest);
    onJoin();
    _play_routine = std::thread([rdr = std::move(reader), p = std::move(processor), this]() mutable {
      _response_watcher(std::move(rdr), std::move(p));
    });
    //return _play_finished;
  }

  void stop()
  {
    std::cout << get_name() << "stopping bot - you may need to kill the process if there is no messages coming from the server" << std::endl;
    _play_finished->set_value(true);
    if (_play_routine.joinable())
    {
      _play_routine.join();
    }
  }

  void wait()
  {
    _play_finished->get_future().get();
    if (_play_routine.joinable())
    {
      _play_routine.join();
    }
  }

  void _response_watcher(std::unique_ptr<::grpc::ClientReader<::lugo::GameSnapshot>> reader,
                         RawTurnProcessor processor)
  {
    try
    {
        lugo::GameSnapshot snapshot;
        while (reader->Read(&snapshot))
        {
          if (snapshot.state() == lugo::GameSnapshot_State_OVER)
          {
            std::cout << get_name() << " All done! lugo::GameSnapshot_State_OVER" << std::endl;
            break;
          }
          else if(_play_finished->get_future().wait_for(std::chrono::seconds(0)) == std::future_status::ready)
          {
            break;
          }
          else if (snapshot.state() == lugo::GameSnapshot_State_LISTENING)
          {
            auto orders_ = lugo::OrderSet();
            orders_.set_turn(snapshot.turn());
            try
            {
              orders_ = processor(orders_, snapshot);
            }
            catch(std::runtime_error& e)
            {
                std::cout << get_name() << " bot processor error: " << e.what() << std::endl;
            }
            if (orders_.IsInitialized())
            {
              auto status = _client->SendOrders(&_context, orders_, nullptr);
              if (!status.ok())
              {
                std::cout << get_name() << " bot processor errorCode: " << status.error_code() << ", errorMessage: " << status.error_message() << std::endl;
              }
            }
            else
            {
              std::cout << get_name() << " [turn #" << std::to_string(snapshot.turn()) << "] bot " << std::to_string(_teamSide) << "-" << std::to_string(_number) << " did not return orders" << std::endl;
            }
          }
          else if (snapshot.state() == lugo::GameSnapshot_State_GET_READY)
          {
            getting_ready_handler(snapshot);
          }
        }
        _play_finished->set_value(true);
        auto status = reader->Finish();
        if (status.ok())
        {
          std::cout << "Client: tracing succeeded" << std::endl;
        }
        else
        {
          std::cout << "Client: tracing failed" << std::endl;
        }
    }
//    catch(grpc::rpcError)
//    {
//
//    }
    catch(std::runtime_error& e)
    {
      std::cout << get_name() << " internal error processing turn: " << e.what() << std::endl;
    }
  }

  /**
   *
   * @param {OrderSet} orderSet
   * @param {game_service.GameClient} connection
   */
//  void orderSetSender(lugo::OrderSet orderSet)
//  {
//    /** @type {module:grpc.ClientUnaryCall} response */
//    auto const response = _client->SendOrders(&context, orderSet, [](){});
//    // console.log(response.getPeer())
//  }
};

/**
 *
 * @param {EnvVarLoader} config
 * @param {Point} initialPosition
 * @returns {Client}
 */
auto NewClientFromConfig(EnvVarLoader config, lugo::Point initialPosition) -> Client
{
  return Client(config.getGrpcUrl(),
    config.getGrpcInsecure(),
    config.getBotToken(),
    config.getBotTeamSide(),
    config.getBotNumber(),
    std::move(initialPosition));
}