#include <lugo/client.hpp>

Client::Client(std::string server_add, bool grpc_insecure, std::string token, lugo::Team_Side teamSide, int number, lugo::Point init_position)
  : _serverAdd{std::move(server_add)}
  , _grpc_insecure{grpc_insecure}
  , _token{std::move(token)}
  , _teamSide{teamSide}
  , _number{number}
  , _init_position{std::move(init_position)}
{
}

auto Client::get_name() -> std::string
{
  return std::string(!_teamSide ? "HOME" : "AWAY") + "-" + std::to_string(_number);
}

void Client::set_initial_position(lugo::Point init_position)
{
  _init_position = std::move(init_position);
}

void Client::getting_ready_handler(lugo::GameSnapshot snapshot)
{
  std::cout << "Default getting ready handler called for ";
}

void Client::set_getting_ready_handler(std::function<void(lugo::GameSnapshot)> handler)
{
  _gettingReadyHandler = std::move(handler);
}

void Client::play(RawTurnProcessor raw_processor, std::function<void()> onJoin)
{
  std::cout << get_name() << "Starting to play" << std::endl;
  _bot_start(std::move(raw_processor), std::move(onJoin));
}

void Client::playAsBot(Bot &bot, std::function<void()> onJoin)
{
  set_getting_ready_handler([&](lugo::GameSnapshot s) { bot.gettingReady(s); });
  std::cout << get_name() << " Playing as bot" << std::endl;

  auto processor = [&](lugo::OrderSet orders, lugo::GameSnapshot snapshot) -> std::optional<lugo::OrderSet> {
    auto playerState = defineState(snapshot, _number, _teamSide);
    if (_number == 1)
    {
      return bot.asGoalkeeper(orders, snapshot, playerState);
    }
    switch (playerState) {
      case PLAYER_STATE::DISPUTING_THE_BALL:
        return bot.onDisputing(orders, snapshot);
      case PLAYER_STATE::DEFENDING:
        return bot.onDefending(orders, snapshot);
      case PLAYER_STATE::SUPPORTING:
        return bot.onSupporting(orders, snapshot);
      case PLAYER_STATE::HOLDING_THE_BALL:
        return bot.onHolding(orders, snapshot);
    }
    return {};
  };
  _bot_start(processor, std::move(onJoin));
}

void Client::_bot_start(RawTurnProcessor processor, std::function<void()> onJoin)
{
  std::cout << get_name() << " Starting bot " << _teamSide << "-" << _number << std::endl;
  _channel = _grpc_insecure
             ? grpc::CreateChannel(_serverAdd, grpc::InsecureChannelCredentials())
             : grpc::CreateChannel(_serverAdd, grpc::InsecureChannelCredentials());
  // TODO: Temporary secure credentials not implemented
  //: grpc::CreateChannel(serverAdd, grpc::SecureChannelCredentials())

  gpr_timespec out;
  out.tv_sec = 10;
  out.tv_nsec = 0;
  out.clock_type = GPR_TIMESPAN;
  _channel->WaitForConnected(out);

  //std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);
  //_context.set_deadline(deadline);
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
  joinRequest.set_protocol_version(PROTOCOL_VERSION); //TODO: python not set it
  joinRequest.set_team_side(_teamSide);
  joinRequest.set_number(_number);
  joinRequest.set_allocated_init_position(new lugo::Point(_init_position));

  //_play_finished = std::make_shared<std::promise<bool>>();
  std::unique_ptr<grpc::ClientReader<lugo::GameSnapshot>> reader = _client->JoinATeam(&_context, joinRequest);
  onJoin();
  //_play_routine = std::thread([rdr = std::move(reader), p = std::move(processor), this]() mutable {
  //  _response_watcher(std::move(rdr), std::move(p));
  //});
  std::cout << "++++++ 1 ++++++" << std::endl;
  _play_finished = false;
  _response_watcher(std::move(reader), std::move(processor));
  std::cout << "++++++ 2 ++++++" << std::endl;
}

void Client::stop()
{
    _play_finished = true;
//  std::cout << get_name() << "stopping bot - you may need to kill the process if there is no messages coming from the server" << std::endl;
//  _play_finished->set_value(true);
//  if (_play_routine.joinable())
//  {
//    _play_routine.join();
//  }
}

void Client::wait()
{
  // TODO: Do nothing for now
//  _play_finished->get_future().get();
//  if (_play_routine.joinable())
//  {
//    _play_routine.join();
//  }
}

void Client::_response_watcher(std::unique_ptr<::grpc::ClientReader<::lugo::GameSnapshot>> reader, RawTurnProcessor processor)
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
      //else if(_play_finished->get_future().wait_for(std::chrono::seconds(0)) == std::future_status::ready)
      else if (_play_finished)
      {
        break;
      }
      else if (snapshot.state() == lugo::GameSnapshot_State_LISTENING)
      {
        auto orders_ = std::optional<lugo::OrderSet>();
        orders_ = lugo::OrderSet();
        orders_.value().set_turn(snapshot.turn());
        try
        {
          orders_ = processor(orders_.value(), snapshot);
        }
        catch(std::runtime_error& e)
        {
          std::cout << get_name() << " bot processor error: " << e.what() << std::endl;
        }
        if (orders_.has_value())
        {
          auto orderResponse = lugo::OrderResponse();
//          std::cout << "++++++ LISTENING 7 ++++++" << std::endl;
          grpc::ClientContext context;
          auto status = _client->SendOrders(&context, orders_.value(), &orderResponse);
//          std::cout << "++++++ LISTENING 6 ++++++" << std::endl;
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
    //_play_finished->set_value(true);
    _play_finished = true;
    auto status = reader->Finish();
    if (status.ok())
    {
      std::cout << "Client: tracing succeeded" << std::endl;
    }
    else
    {
      std::cout << status.error_details() << " -> " << status.error_message() << " CODE: " << status.error_code() << ", Client: tracing failed" << std::endl;
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

auto NewClientFromConfig(const EnvVarLoader &config, lugo::Point initialPosition) -> std::unique_ptr<Client>
{
  return std::make_unique<Client>(config.getGrpcUrl(),
                                  config.getGrpcInsecure(),
                                  config.getBotToken(),
                                  config.getBotTeamSide(),
                                  config.getBotNumber(),
                                  std::move(initialPosition));
}
