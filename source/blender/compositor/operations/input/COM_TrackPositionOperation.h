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
 * Copyright 2012, Blender Foundation.
 */

#pragma once

#include <string.h>

#include "COM_NodeOperation.h"

#include "DNA_movieclip_types.h"

#include "BLI_string.h"

/**
 * Class with implementation of green screen gradient rasterization
 */
class TrackPositionOperation : public NodeOperation {
 protected:
  MovieClip *m_movieClip;
  int m_framenumber;
  char m_trackingObjectName[64];
  char m_trackName[64];
  int m_axis;
  int m_position;
  int m_relativeFrame;
  bool m_speed_output;

  float m_markerPos[2];
  float m_relativePos[2];

  float m_pix_result;

 public:
  TrackPositionOperation();

  void setMovieClip(MovieClip *clip)
  {
    this->m_movieClip = clip;
  }
  void setTrackingObject(char *object)
  {
    BLI_strncpy(this->m_trackingObjectName, object, sizeof(this->m_trackingObjectName));
  }
  void setTrackName(char *track)
  {
    BLI_strncpy(this->m_trackName, track, sizeof(this->m_trackName));
  }
  void setFramenumber(int framenumber)
  {
    this->m_framenumber = framenumber;
  }
  void setAxis(int value)
  {
    this->m_axis = value;
  }
  void setPosition(int value)
  {
    this->m_position = value;
  }
  void setRelativeFrame(int value)
  {
    this->m_relativeFrame = value;
  }
  void setSpeedOutput(bool speed_output)
  {
    this->m_speed_output = speed_output;
  }

  void initExecution();

  BufferType getBufferType() const override
  {
    return BufferType::NO_BUFFER_NO_WRITE;
  }
  bool isSingleElem() const override
  {
    return true;
  }
  float *getSingleElem() override
  {
    return &m_pix_result;
  }
};
