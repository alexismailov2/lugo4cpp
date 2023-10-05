#pragma once

#include <lugo/specs.hpp>
#include <lugo/mapper.hpp>

#include <pb/physics.pb.h>
#include <pb/server.pb.h>

// MinCols Define the min number of cols allowed on the field division by the Mapper
auto const MinCols = 4;
// MinRows Define the min number of rows allowed on the field division by the Mapper
auto const MinRows = 2;
// MaxCols Define the max number of cols allowed on the field division by the Mapper
auto const MaxCols = 200;
// MaxRows Define the max number of rows allowed on the field division by the Mapper
auto const MaxRows = 100;

class Region;

/**
 * TODO: Should be described.
 */
struct Mapper
    : public std::enable_shared_from_this<Mapper>
{
  // TODO: Should be private
  int _cols{}; /// TODO: Should be described.
  int _rows{}; /// TODO: Should be described.
  lugo::Team::Side _side; /// TODO: Should be described.
  int _regionWidth{}; /// TODO: Should be described.
  int _regionHeight{}; /// TODO: Should be described.

  /**
   * TODO: Should be described.
   * @param cols
   * @param rows
   * @param side
   */
  Mapper(int cols, int rows, lugo::Team::Side side);

  /**
   * TODO: Should be described.
   * @param col
   * @param row
   * @return
   */
  auto getRegion(int col, int row) -> Region;

  /**
   * TODO: Should be described.
   * @param point
   * @return
   */
  auto getRegionFromPoint(lugo::Point point) -> Region;

  /**
   * TODO: Should be described.
   * @param center
   * @return Point
   */
  auto mirrorCoordsToAway(lugo::Point const& center) -> lugo::Point;
};

/**
 * TODO: Should be described.
 */
class Region
{
private:
  int _col{}; /// TODO: Should be described.
  int _row{}; /// TODO: Should be described.
  lugo::Team_Side _side{}; /// TODO: Should be described.
  lugo::Point _center{};  /// TODO: Should be described.
  std::shared_ptr<Mapper> _positioner; /// TODO: Should be described.

public:
  /**
   * TODO: Should be described.
   * @param col
   * @param row
   * @param side
   * @param center
   * @param positioner
   */
  Region(int col,
         int row,
         lugo::Team_Side side,
         lugo::Point center,
         std::shared_ptr<Mapper> positioner);

  /**
   * TODO: Should be described.
   * @param region
   * @return
   */
  bool eq(Region const& region) const;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getCol() const -> int;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getRow() const -> int;

  /**
   * TODO: Should be described.
   * @return
   */
  auto getCenter() const -> lugo::Point const&;

  /**
   * TODO: Should be described.
   * @return
   */
  auto toString() const -> std::string;

  /**
   * TODO: Should be described.
   * @return
   */
  auto front() -> Region;

  /**
   * TODO: Should be described.
   * @return
   */
  auto back() -> Region;

  /**
   * TODO: Should be described.
   * @return
   */
  auto left() -> Region;

  /**
   * TODO: Should be described.
   * @return
   */
  auto right() -> Region;
};