#pragma once

#include <lugo/geo.hpp>

#include <pb/physics.pb.h>

namespace ORIENTATION {

/**
 * TODO: Should be described.
 * @return
 */
auto EAST() -> lugo::Vector;

/**
 * TODO: Should be described.
 * @return
 */
auto WEST() -> lugo::Vector;

/**
 * TODO: Should be described.
 * @return
 */
auto SOUTH() -> lugo::Vector;

/**
 * TODO: Should be described.
 * @return
 */
auto NORTH() -> lugo::Vector;

/**
 * TODO: Should be described.
 * @return
 */
auto NORTH_EAST() -> lugo::Vector;

/**
 * TODO: Should be described.
 * @return
 */
auto NORTH_WEST() -> lugo::Vector;

/**
 * TODO: Should be described.
 * @return
 */
auto SOUTH_EAST() -> lugo::Vector;

/**
 * TODO: Should be described.
 * @return
 */
auto SOUTH_WEST() -> lugo::Vector;
}