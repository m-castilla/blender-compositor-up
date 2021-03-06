/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2020, Blender Foundation.
 */

#pragma once

#include <chrono>

namespace TimeUtil {

uint64_t getNowMsTime();
uint64_t getNowNsTime();
uint64_t durationToMs(std::chrono::steady_clock::duration duration);
uint64_t durationToNs(std::chrono::steady_clock::duration duration);

std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> nsEpochToTimePoint(
    uint64_t epoch_time_nanoseconds);

};  // namespace TimeUtil
