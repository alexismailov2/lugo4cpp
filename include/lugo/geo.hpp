#pragma once

#include <pb/physics.pb.h>

/**
 * TODO: Should be described.
 * @param Vector v
 */
auto isInValidateVector(lugo::Vector const& v) -> bool;

/**
 * TODO: Should be described.
 * @param Point from
 * @param Point to
 * @returns Vector
 */
auto NewVector(lugo::Point const& from, lugo::Point const& to) -> lugo::Vector;

/**
 * TODO: Should be described.
 * @param Vector v
 * @returns number
 */
auto getLength(lugo::Vector const& v) -> float;

/**
 * TODO: Should be described.
 * @param v
 * @param scale
 * @return
 */
auto getScaledVector(lugo::Vector const& v, float scale) -> lugo::Vector;

/**
 * TODO: Should be described.
 * @param v
 * @return Vector a new vector with same direction but normalized to 0-100
 */
auto normalize(lugo::Vector const& v) -> lugo::Vector;

/**
 * TODO: Should be described.
 * @param Vector originalV
 * @param Vector subV
 * @returns Vector
 */
auto subVector(lugo::Vector const& originalV, lugo::Vector const& subV) -> lugo::Vector;

/**
 * TODO: Should be described.
 * @param a
 * @param b
 * @return
 */
auto distanceBetweenPoints(lugo::Point const& a, lugo::Point const& b) -> float;