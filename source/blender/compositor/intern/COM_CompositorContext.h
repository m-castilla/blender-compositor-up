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

#include <string>
#include <vector>

#include "BKE_node.h"
#include "DNA_color_types.h"
#include "DNA_node_types.h"
#include "DNA_scene_types.h"
#ifdef WITH_CXX_GUARDEDALLOC
#  include "MEM_guardedalloc.h"
#endif

#include "COM_Pixels.h"
#include "COM_Renderer.h"
#include "COM_defines.h"

/**
 * \brief Overall context of the compositor
 */
class CompositorContext {
 private:
  struct CompositTreeExec *m_exec_data;

  /**
   * \brief The quality of the composite.
   * This field is initialized in ExecutionSystem and must only be read from that point on.
   * \see ExecutionSystem
   */
  CompositorQuality m_quality;

  /**
   * \brief Preview image hash table
   * This field is initialized in ExecutionSystem and must only be read from that point on.
   */
  bNodeInstanceHash *m_previews;

  std::string m_execution_id;

  int m_cpu_work_threads;

  float m_inputs_scale;
  uint64_t m_max_mem_cache_bytes;
  uint64_t m_max_disk_cache_bytes;
  const char *m_disk_cache_dir;
  bool m_use_disk_cache;

 private:
  CompositorContext();

 public:
  /**
   * \brief constructor initializes the context with default values.
   */
  static CompositorContext build(const std::string &execution_id,
                                 struct CompositTreeExec *exec_data);

  void initialize();
  void deinitialize();

  struct Main *getMain() const
  {
    return m_exec_data->main;
  }
  struct Depsgraph *getDepsgraph() const
  {
    return m_exec_data->depsgraph;
  }
  ViewLayer *getViewLayer() const
  {
    return m_exec_data->view_layer;
  }
  CompositTreeExec *getTreeExecData()
  {
    return m_exec_data;
  }
  PixelsSampler getDefaultSampler() const
  {
    return PixelsSampler{PixelInterpolation::BILINEAR, PixelExtend::CLIP};
  }

  PixelsSampler getViewsSampler() const
  {
    return PixelsSampler{PixelInterpolation::NEAREST, PixelExtend::CLIP};
  }

  int getMaxImgW() const;
  int getMaxImgH() const;

  // Is video sequencer activated in the post-processing pipeline
  bool isVseSequencerPassOn() const
  {
    return (m_exec_data->scene->r.scemode & R_DOSEQ);
  }

  bool isBreaked() const;
  void updateDraw() const;

  int getPreviewSize() const;

  float getInputsScale() const
  {
    return m_exec_data->ntree->inputs_scale;
  }

  int getNCpuWorkThreads() const
  {
    return m_cpu_work_threads;
  }
  void setNCpuWorkThreads(int n_threads)
  {
    m_cpu_work_threads = n_threads;
  }

  std::string getExecutionId() const
  {
    return m_execution_id;
  }

  void setMemCacheBytes(size_t bytes)
  {
    m_max_mem_cache_bytes = bytes;
  }

  size_t getMemCacheBytes() const
  {
    return m_max_mem_cache_bytes;
  }

  size_t getDiskCacheBytes() const
  {
    return useDiskCache() ? m_max_disk_cache_bytes : 0;
  }

  bool useDiskCache() const
  {
    return m_use_disk_cache;
  }

  const char *getDiskCacheDir() const
  {
    return m_disk_cache_dir;
  }

  bool isRendering() const
  {
    return m_exec_data->rendering;
  }

  const RenderData *getRenderData() const
  {
    return m_exec_data->rd;
  }

  int getRenderWidth() const
  {
    return m_exec_data->rd->xsch;
  }
  int getRenderHeight() const
  {
    return m_exec_data->rd->ysch;
  }

  bNodeTree *getbNodeTree() const
  {
    return this->m_exec_data->ntree;
  }

  Scene *getScene() const
  {
    return m_exec_data->scene;
  }

  /**
   * \brief get the preview image hash table
   */
  bNodeInstanceHash *getPreviewsHashTable() const
  {
    return this->m_previews;
  }

  /**
   * \brief get view settings of color color management
   */
  const ColorManagedViewSettings *getViewSettings() const
  {
    return m_exec_data->view_settings;
  }

  /**
   * \brief get display settings of color color management
   */
  const ColorManagedDisplaySettings *getDisplaySettings() const
  {
    return m_exec_data->display_settings;
  }

  /**
   * \brief get the quality
   */
  CompositorQuality getQuality() const
  {
    return this->m_quality;
  }

  /**
   * \brief get the current frame-number of the scene in this context
   */
  int getCurrentFrame() const;

  /**
   * \brief get the active rendering view
   */
  const char *getViewName() const
  {
    return m_exec_data->viewname;
  }

#ifdef WITH_CXX_GUARDEDALLOC
  MEM_CXX_CLASS_ALLOC_FUNCS("COM:CompositorContext")
#endif
};
