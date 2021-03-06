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

#include "COM_MultilayerImageOperation.h"

#include "COM_BufferUtil.h"
#include "COM_PixelsUtil.h"
#include "DNA_image_types.h"
#include "IMB_imbuf.h"
#include "IMB_imbuf_types.h"

#include "COM_kernel_cpu.h"

MultilayerBaseOperation::MultilayerBaseOperation(int passindex, int view) : BaseImageOperation()
{
  this->m_passId = passindex;
  this->m_view = view;
  m_renderlayer = nullptr;
}

void MultilayerBaseOperation::requestImBuf()
{
  if (m_imbuf == nullptr) {
    /* temporarily changes the view to get the right ImBuf */
    int view = this->m_imageUser->view;

    this->m_imageUser->view = this->m_view;
    this->m_imageUser->pass = this->m_passId;

    if (BKE_image_multilayer_index(this->m_image->rr, this->m_imageUser)) {
      BaseImageOperation::requestImBuf();
      this->m_imageUser->view = view;
    }

    this->m_imageUser->view = view;
  }
}

void MultilayerBaseOperation::hashParams()
{
  BaseImageOperation::hashParams();
  hashParam(this->m_view);
  hashParam(this->m_passId);
  hashParam((size_t)&m_renderlayer);
}

void MultilayerColorOperation::execPixels(ExecutionManager &man)
{
  auto cpuWrite = [&](PixelsRect &dst, const WriteRectContext & /*ctx*/) {
    PixelsUtil::copyImBufRect(dst, m_imbuf, COM_NUM_CHANNELS_COLOR, COM_NUM_CHANNELS_COLOR);
  };
  return cpuWriteSeek(man, cpuWrite);
}

void MultilayerValueOperation::execPixels(ExecutionManager &man)
{
  auto cpuWrite = [&](PixelsRect &dst, const WriteRectContext &) {
    PixelsUtil::copyImBufRectNChannels(
        dst, m_imbuf, COM_NUM_CHANNELS_VALUE, COM_NUM_CHANNELS_VALUE);
  };
  return cpuWriteSeek(man, cpuWrite);
}

void MultilayerVectorOperation::execPixels(ExecutionManager &man)
{
  auto cpuWrite = [&](PixelsRect &dst, const WriteRectContext & /*ctx*/) {
    PixelsUtil::copyImBufRectNChannels(
        dst, m_imbuf, COM_NUM_CHANNELS_VECTOR, COM_NUM_CHANNELS_VECTOR);
  };
  return cpuWriteSeek(man, cpuWrite);
}
