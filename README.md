# lugo4cpp

## Prerequisites
Clone this repo, go to the root folder of the repo project.
### Macos
```
brew install openssl grpc
```
### Linux
```
sudo apt-get install -y build-essential autoconf libtool pkg-config cmake clang libc++-dev
./build_grpc.sh
```

## Build
```
    cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./build/install
    cmake --build build --config Release --target install
```

## Run test
### Running docker container with server side
```
docker run -p 8080:8080 -p 5000:5000 lugobots/server:latest play
```
### Running simple examples(there is needed 12 HOME players and 12 AWAY)
```
cp ./example/simple/run_12_players.sh ./build/install/bin
cd ./build/install/bin
./run_12_players.sh
```