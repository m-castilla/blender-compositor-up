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

#include "COM_TextureOperation.h"
#include "COM_ExecutionManager.h"
#include "COM_WorkScheduler.h"

#include "BKE_image.h"
#include "BKE_node.h"

#include "BLI_listbase.h"

#include "COM_kernel_cpu.h"

TextureBaseOperation::TextureBaseOperation() : NodeOperation()
{
  this->addInputSocket(SocketType::VECTOR);  // offset
  this->addInputSocket(SocketType::VECTOR);  // size
  this->m_texture = NULL;
  this->m_inputSize = NULL;
  this->m_inputOffset = NULL;
  this->m_rd = NULL;
  this->m_pool = NULL;
  this->m_sceneColorManage = false;
}
TextureOperation::TextureOperation() : TextureBaseOperation()
{
  this->addOutputSocket(SocketType::COLOR);
}
TextureAlphaOperation::TextureAlphaOperation() : TextureBaseOperation()
{
  this->addOutputSocket(SocketType::VALUE);
}

void TextureBaseOperation::initExecution()
{
  this->m_inputOffset = getInputOperation(0);
  this->m_inputSize = getInputOperation(1);
  this->m_pool = BKE_image_pool_new();
  if (this->m_texture != NULL && this->m_texture->nodetree != NULL && this->m_texture->use_nodes) {
    ntreeTexBeginExecTree(this->m_texture->nodetree);
  }
  NodeOperation::initExecution();
}
void TextureBaseOperation::deinitExecution()
{
  this->m_inputSize = NULL;
  this->m_inputOffset = NULL;
  BKE_image_pool_free(this->m_pool);
  this->m_pool = NULL;
  if (this->m_texture != NULL && this->m_texture->use_nodes && this->m_texture->nodetree != NULL &&
      this->m_texture->nodetree->execdata != NULL) {
    ntreeTexEndExecTree(this->m_texture->nodetree->execdata);
  }
  NodeOperation::deinitExecution();
}

void TextureBaseOperation::hashParams()
{
  NodeOperation::hashParams();
  if (m_texture) {
    hashParam(m_texture->id.session_uuid);
  }
}

ResolutionType TextureBaseOperation::determineResolution(int resolution[2],
                                                         int preferredResolution[2],
                                                         bool /*setResolution*/)
{
  if (preferredResolution[0] == 0 || preferredResolution[1] == 0) {
    int width = this->m_rd->xsch * this->m_rd->size / 100;
    int height = this->m_rd->ysch * this->m_rd->size / 100;
    resolution[0] = width;
    resolution[1] = height;
    return ResolutionType::Fixed;
  }
  else {
    resolution[0] = preferredResolution[0];
    resolution[1] = preferredResolution[1];
    return ResolutionType::Determined;
  }
}

void TextureBaseOperation::writePixels(ExecutionManager &man, bool is_alpha_only)
{
  const int width = this->getWidth();
  const int height = this->getHeight();
  const float cx = width / 2;
  const float cy = height / 2;
  auto size = m_inputSize->getPixels(this, man);
  auto offset = m_inputOffset->getPixels(this, man);

  auto cpu_write = [&](PixelsRect &dst, const WriteRectContext & /*ctx*/) {
    TexResult texres = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, NULL};
    CCL::float4 texture_vec;
    int retval;

    READ_DECL(size);
    READ_DECL(offset);
    WRITE_DECL(dst);

    CPU_LOOP_START(dst);

    COPY_COORDS(size, dst_coords);
    COPY_COORDS(offset, dst_coords);

    float u = (dst_coords.x - cx) / width * 2;
    float v = (dst_coords.y - cy) / height * 2;

    /* When no interpolation/filtering happens in multitex() force nearest interpolation.
     * We do it here because (a) we can't easily say multitex() that we want nearest
     * interpolation and (b) in such configuration multitex() simply floor's the value
     * which often produces artifacts.
     */
    if (m_texture != NULL && (m_texture->imaflag & TEX_INTERPOL) == 0) {
      u += 0.5f / cx;
      v += 0.5f / cy;
    }

    READ_IMG3(size, size_pix);
    READ_IMG3(offset, offset_pix);

    offset_pix.x += u;
    offset_pix.y += v;
    texture_vec = size_pix * offset_pix;

    const int thread_id = WorkScheduler::current_thread_id();
    retval = multitex_ext(this->m_texture,
                          (float *)&texture_vec,
                          NULL,
                          NULL,
                          0,
                          &texres,
                          thread_id,
                          m_pool,
                          m_sceneColorManage,
                          false);

    if (is_alpha_only) {
      dst_img.buffer[dst_offset] = texres.talpha ? texres.ta : texres.tin;
    }
    else {
      if ((retval & TEX_RGB)) {
        texture_vec = CCL::make_float4(
            texres.tr, texres.tg, texres.tb, texres.talpha ? texres.ta : texres.tin);
      }
      else {
        texture_vec = CCL::make_float4_1(texres.talpha ? texres.ta : texres.tin);
      }
      WRITE_IMG(dst, texture_vec);
    }

    CPU_LOOP_END;
  };

  cpuWriteSeek(man, cpu_write);
}

void TextureOperation::execPixels(ExecutionManager &man)
{
  writePixels(man, false);
}

void TextureAlphaOperation::execPixels(ExecutionManager &man)
{
  writePixels(man, true);
}
