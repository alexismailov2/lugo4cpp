#include <lugo/mapper.hpp>

Region::Region(int col,
               int row,
               lugo::Team_Side side,
               lugo::Point center,
               std::shared_ptr<Mapper> positioner)
    : _col{col},
      _row{row},
      _side{side},
      _center{std::move(center)},
      _positioner{std::move(positioner)}
{
}

bool Region::eq(Region const& region) const
{
  return region.getCol() == _col &&
         region._side == _side &&
         region.getRow() == _row;
}

auto Region::getCol() const -> int
{
  return _col;
}

auto Region::getRow() const -> int
{
  return _row;
}

auto Region::getCenter() const -> lugo::Point const&
{
  return _center;
}

auto Region::toString() const -> std::string
{
  return std::to_string(_col) + ", " + std::to_string(_row);
}

auto Region::front() -> Region
{
  return _positioner->getRegion(std::max(_col + 1, 0), _row);
}

auto Region::back() -> Region
{
  return _positioner->getRegion(std::max(_col - 1, 0), _row);
}

auto Region::left() -> Region
{
  return _positioner->getRegion(_col, std::max(_row + 1, 0));
}

auto Region::right() -> Region
{
  return _positioner->getRegion(_col, std::max(_row - 1, 0));
}

Mapper::Mapper(int cols, int rows, lugo::Team::Side side)
  : _cols{cols},
    _rows{rows},
    _side{side},
    _regionWidth{SPECS::MAX_X_COORDINATE / cols},
    _regionHeight{SPECS::MAX_Y_COORDINATE / rows}
{
  if (cols < MinCols) {
    throw std::runtime_error("number of cols lower the minimum");
  }
  if (cols > MaxCols) {
    throw std::runtime_error("number of cols higher the maximum");
  }
  if (rows < MinRows) {
    throw std::runtime_error("number of rows lower the minimum");
  }
  if (rows > MaxRows) {
    throw std::runtime_error("number of rows higher the maximum");
  }
}

auto Mapper::getRegion(int col, int row) -> Region
{
  col = std::max(0, col);
  col = std::min(_cols - 1, col);
  row = std::max(0, row);
  row = std::min(_rows - 1, row);
  auto center = lugo::Point();
  center.set_x(std::round((col * _regionWidth) + (_regionWidth / 2)));
  center.set_y(std::round((row * _regionHeight) + (_regionHeight / 2)));
  if (_side == lugo::Team_Side_AWAY) {
    center = mirrorCoordsToAway(center);
  }
  return Region(col, row, _side, center, shared_from_this());
}

auto Mapper::getRegionFromPoint(lugo::Point point) -> Region
{
  if (_side == lugo::Team_Side_AWAY) {
    point = mirrorCoordsToAway(point);
  }
  auto const cx = std::floor(point.x() / _regionWidth);
  auto const cy = std::floor(point.y() / _regionHeight);
  auto const col = std::min((int)cx, _cols - 1);
  auto const row = std::min((int)cy, _rows - 1);
  return getRegion(col, row);
}

auto Mapper::mirrorCoordsToAway(lugo::Point const& center) -> lugo::Point
{
  auto mirrored = lugo::Point();
  mirrored.set_x(SPECS::MAX_X_COORDINATE - center.x());
  mirrored.set_y(SPECS::MAX_Y_COORDINATE - center.y());
  return mirrored;
}
