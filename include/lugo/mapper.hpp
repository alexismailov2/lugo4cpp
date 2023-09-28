#pragma once

#include <lugo/specs.hpp>
#include <lugo/mapper.hpp>

#include <physics.pb.h>
#include <server.pb.h>

// MinCols Define the min number of cols allowed on the field division by the Mapper
auto const MinCols = 4;
// MinRows Define the min number of rows allowed on the field division by the Mapper
auto const MinRows = 2;
// MaxCols Define the max number of cols allowed on the field division by the Mapper
auto const MaxCols = 200;
// MaxRows Define the max number of rows allowed on the field division by the Mapper
auto const MaxRows = 100;

class Region;
struct Mapper
    : public std::enable_shared_from_this<Mapper>
{
  int _cols{}; ///
  int _rows{}; ///
  lugo::Team::Side _side; /// @private @type lugo::Team::Side
  int _regionWidth{}; ///
  int _regionHeight{}; ///

  /**
   *
   * @param cols {number}
   * @param rows {number}
   * @param side {lugo.Team.Side}
   */
  Mapper(int cols, int rows, lugo::Team::Side side);

  /**
   * @param col number
   * @param row number
   * @return Region
   */
  auto getRegion(int col, int row) -> Region;

  /**
   * @param physics::Point point
   * @return Region
   */
  auto getRegionFromPoint(lugo::Point point) -> Region;

  /**
   *
   * @param center
   * @return Point
   */
  auto mirrorCoordsToAway(lugo::Point center) -> lugo::Point;
};

class Region
{
private:
  int _col{}; ///
  int _row{}; ///
  lugo::Team_Side _side{}; ///
  lugo::Point _center{};  ///
  std::shared_ptr<Mapper> _positioner; ///

public:
  /**
   *
   * @param col int
   * @param row int
   * @param side lugo::Team_Side
   * @param center physics.Point
   * @param positioner Mapper
   */
  Region(int col,
         int row,
         lugo::Team_Side side,
         lugo::Point center,
         std::shared_ptr<Mapper> positioner);
  bool eq(Region region);
  auto getCol() -> int;
  auto getRow() -> int;
  auto getCenter() -> lugo::Point;
  auto toString() -> std::string;;
  auto front() -> Region;
  auto back() -> Region;
  auto left() -> Region;
  auto right() -> Region;
};