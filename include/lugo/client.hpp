#pragma once

#include <lugo/configurator.hpp>
#include <lugo/index.hpp>
#include <lugo/stub.hpp>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

#include <physics.pb.h>
#include <server.pb.h>
#include <server.grpc.pb.h>


//#include "./physics.pb.h"
//#include "./server.pb.h"
//#include "./server.grpc.pb.h"


#include <functional>
#include <future>

auto const PROTOCOL_VERSION = "1.0.0";

using RawTurnProcessor = std::function<lugo::OrderSet(lugo::OrderSet, lugo::GameSnapshot)>;

class Client
{
private:
  std::string serverAdd; ///
  bool grpc_insecure{}; ///
  std::string token; ///
  lugo::Team_Side teamSide; ///
  int number{}; ///
  lugo::Point init_position; ///
  std::unique_ptr<lugo::Game::Stub> _client; ///
  grpc::ClientContext context;
  std::function<void(lugo::GameSnapshot)> gettingReadyHandler; ///
  std::shared_ptr<grpc::Channel> _channel;

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
      : serverAdd{std::move(server_add)}
      , grpc_insecure{grpc_insecure}
      , token{std::move(token)}
      , teamSide{teamSide}
      , number{number}
      , init_position{std::move(init_position)}
  {
  }

  /**
   *
   * @param {function(GameSnapshot)} handler
   *
   * @returns {Client}
   */
  Client& setGettingReadyHandler(std::function<void(lugo::GameSnapshot)> handler)
  {
    gettingReadyHandler = std::move(handler);
    return *this;
  }

  auto get_name() -> std::string
  {
    return std::string(!teamSide ? "HOME" : "AWAY") + "-" + std::to_string(number);
  }

  /**
   *
   * @param {Bot} bot
   * @param {function()} onJoin
   * @returns {Promise<void>}
   */
  auto playAsBot(Bot& bot, std::function<void()> onJoin) -> lugo::OrderSet
  {
    std::cout << get_name() << "Playing as bot" << std::endl;
    setGettingReadyHandler([&](lugo::GameSnapshot s) { bot.gettingReady(s); });

    auto processor = [&](lugo::OrderSet orders, lugo::GameSnapshot snapshot) -> lugo::OrderSet {
      auto playerState = defineState(snapshot, number, teamSide);
      if (number == 1)
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
    return _bot_start(processor, onJoin);
  }

  /**
   *
   * @param {function} raw_processor
   * @param {function()} onJoin
   */
  auto play(RawTurnProcessor raw_processor, std::function<void()> onJoin = {}) -> lugo::OrderSet
  {
    return _bot_start(std::move(raw_processor), std::move(onJoin));
  }

 /**
  *
  * @param processor
  * @param onJoin callback
  * @return
  */
  auto _bot_start(RawTurnProcessor processor, std::function<void()> onJoin) -> lugo::OrderSet
  {
    std::cout << get_name() << " Starting bot to " << serverAdd << " === " << teamSide << "-" << number << std::endl;
    auto channel = grpc_insecure
                       ? grpc::CreateChannel(serverAdd, grpc::InsecureChannelCredentials())
                       : grpc::CreateChannel(serverAdd, grpc::InsecureChannelCredentials());
    // TODO: Temporary secure credentials not implemented
                       //: grpc::CreateChannel(serverAdd, grpc::SecureChannelCredentials())
//    try
//    {
//      grpc.channel_ready_future(channel).result(timeout=5);
//    }
//    catch(grpc::FutureTimeoutError)
//    {
//        throw std::runtime_error("timed out waiting to connect to the game server " + serverAdd);
//    }

    _channel = channel;
    _client = lugo::Game::NewStub(channel);

    gpr_timespec out;
    out.tv_sec = 10;
    out.tv_nsec = 0;
    out.clock_type = GPR_TIMESPAN;
    channel->WaitForConnected(out);
//    channel->WaitForStateChange(GRPC_CHANNEL_IDLE,out );
//    if (err)
//    {
//      reject(Error("failed to connect to the Game Server at " + serverAdd + " : " + err));
//    }

     std::cout << "POS ORIGINAL, X: " << init_position.x() << ", Y: " << init_position.y() << std::endl;
    std::cout << (std::string("connect to the gRPC server ") + (teamSide == lugo::Team_Side_HOME ? "HOME" : "AWAY") + " - " + std::to_string(number)) << std::endl;

//    auto iniPos = new lugo::Point();
//
//     iniPos->set_x(2000 + (500 * number));
//     iniPos->set_y(600 );
    auto req = lugo::JoinRequest();
    req.set_token(token);
    req.set_protocol_version(PROTOCOL_VERSION);
    req.set_team_side(teamSide);
    req.set_number(number);
    req.set_allocated_init_position(&init_position);
  //  *req.mutable_init_position() = iniPos;

    auto reader = _client->JoinATeam(&context, req);
//    reader->WaitForInitialMetadata();

//    lugo::GameSnapshot snapshot;
//     std::cout << " Lets read this: X: " << req.init_position().x() << ", Y: " << req.init_position().y() << std::endl;
//     while (reader->Read(&snapshot)) {
//         std::cout << snapshot.state() << " Ainda aqui" << std::endl;
//
//     }

    onJoin();
    return _response_watcher(std::move(reader), processor);
    // TODO: for multithreading should be returned std::future
  }

  auto _response_watcher(std::unique_ptr<::grpc::ClientReader<::lugo::GameSnapshot>> reader,
                         RawTurnProcessor processor) -> lugo::OrderSet
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
//          else if(_play_finished.is_set())
//          {
//            break;
//          }
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
                std::cout << get_name() << "bot processor error: " << e.what() << std::endl;
            }
            if (orders_.IsInitialized())
            {
              //await this.orderSetSender(orderSet);
              auto status = _client->SendOrders(&context, orders_, nullptr);
              if (!status.ok())
              {
                std::cout << get_name() << " bot processor errorCode: " << status.error_code() << ", errorMessage: " << status.error_message() << std::endl;
              }
            }
            else
            {
              std::cout << get_name() << " [turn #" << std::to_string(snapshot.turn()) << "] bot " << std::to_string(teamSide) << "-" << std::to_string(number) << " did not return orders" << std::endl;
            }
          }
          else if (snapshot.state() == lugo::GameSnapshot_State_GET_READY)
          {
            gettingReadyHandler(snapshot);
          }
          //std::cout << "Client: received value " << reader.turn() << std::endl;
        }
        //_play_finished.set();
        auto status = reader->Finish();

        if (status.ok())
        {
          std::cout << "Client: tracing succeeded" << std::endl;
          //return true;
        }
        else
        {
            // IMPORTANT TO HELP DEBUGGING
          std::cout << status.error_details() << " -> " << status.error_message() << " CODE: " << status.error_code() << "Client: tracing failed" << std::endl;
          //return false;
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
    return {};
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