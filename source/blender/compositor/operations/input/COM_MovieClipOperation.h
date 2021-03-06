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
 * Copyright 2011, Blender Foundation.
 */

#pragma once

#include "BLI_listbase.h"
#include "DNA_movieclip_types.h"
#include "IMB_imbuf_types.h"
#include <mutex>

#include "COM_NodeOperation.h"

/**
 * Base class for movie clip
 */
class MovieClipBaseOperation : public NodeOperation {
 protected:
  MovieClip *m_movieClip;
  MovieClipUser *m_movieClipUser;
  ImBuf *m_clip_imbuf;
  int m_framenumber;
  bool m_cacheFrame;

  virtual ResolutionType determineResolution(int resolution[2],
                                             int preferredResolution[2],
                                             bool setResolution) override;

 public:
  MovieClipBaseOperation();

  void initExecution();
  void deinitExecution();
  void setMovieClip(MovieClip *image)
  {
    this->m_movieClip = image;
  }
  void setMovieClipUser(MovieClipUser *imageuser)
  {
    this->m_movieClipUser = imageuser;
  }
  void setCacheFrame(bool value)
  {
    this->m_cacheFrame = value;
  }

  void setFramenumber(int framenumber)
  {
    this->m_framenumber = framenumber;
  }

 protected:
  virtual void hashParams() override;
};

class MovieClipOperation : public MovieClipBaseOperation {
 public:
  MovieClipOperation();
  std::mutex m_mutex;
  int m_n_written_rects;

 protected:
  void execPixels(ExecutionManager &man) override;
  bool canCompute() const override
  {
    return false;
  }
};

class MovieClipAlphaOperation : public MovieClipBaseOperation {
 public:
  MovieClipAlphaOperation();

 protected:
  void execPixels(ExecutionManager &man) override;
  bool canCompute() const override
  {
    return false;
  }
};
