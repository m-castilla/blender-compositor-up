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

#include "BLI_map.hh"
#include "COM_ComputeDevice.h"
#include "COM_ComputePlatform.h"
#include "clew.h"

namespace blender::compositor {
class OpenCLManager;
class OpenCLPlatform : public ComputePlatform {
 private:
  cl_context m_context;
  cl_program m_program;
  OpenCLManager &m_man;
  blender::Map<std::pair<int, int>, cl_sampler> m_samplers;

 public:
  OpenCLPlatform(OpenCLManager &man, cl_context context, cl_program program);
  ~OpenCLPlatform();

  cl_context getContext()
  {
    return m_context;
  }
  cl_program getProgram()
  {
    return m_program;
  }

  const cl_image_format *getImageFormat() const;
  int getMemoryAccessFlag(ComputeAccess mem_access) const;
  cl_sampler getSampler(ComputeInterpolation interp, ComputeExtend extend);

 protected:
  ComputeKernel *createKernel(const blender::StringRef kernel_name,
                              ComputeDevice *device) override;
  cl_sampler createSampler(ComputeInterpolation interp, ComputeExtend extend);

#ifdef WITH_CXX_GUARDEDALLOC
  MEM_CXX_CLASS_ALLOC_FUNCS("COM:OpenCLPlatform")
#endif
};

}  // namespace blender::compositor