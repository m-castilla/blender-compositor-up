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

#include <cstring>

#include "COM_GlobalManager.h"
#include "COM_RenderLayersNode.h"
#include "COM_RenderLayersProg.h"
#include "COM_ScaleOperation.h"
#include "COM_SetColorOperation.h"
#include "COM_SetValueOperation.h"
#include "COM_SetVectorOperation.h"

RenderLayersNode::RenderLayersNode(bNode *editorNode) : Node(editorNode)
{
  /* pass */
}

void RenderLayersNode::testSocketLink(NodeConverter &converter,
                                      CompositorContext &context,
                                      NodeOutput *output,
                                      RenderLayersProg *operation,
                                      Scene *scene,
                                      ViewLayer *view_layer,
                                      bool is_preview) const
{
  operation->setScene(scene);
  operation->setViewLayer(view_layer);
  operation->setRenderData(context.getRenderData());
  operation->setViewName(context.getViewName());

  // We must allow mapping different socket types. For example vector output in node is VECTOR but
  // in Operation is COLOR.
  converter.mapOutputSocket(output, operation->getOutputSocket(), false);
  converter.addOperation(operation);

  if (is_preview) { /* only for image socket */
    converter.addPreview(operation->getOutputSocket());
  }
}

void RenderLayersNode::testRenderLink(NodeConverter &converter,
                                      CompositorContext &context,
                                      Render *re,
                                      ViewLayer *view_layer) const
{
  Scene *scene = (Scene *)this->getbNode()->id;
  RenderResult *rr = RE_AcquireResultRead(re);
  if (rr == NULL) {
    missingRenderLink(converter);
    return;
  }
  if (view_layer == NULL) {
    missingRenderLink(converter);
    return;
  }
  RenderLayer *rl = RE_GetRenderLayer(rr, view_layer->name);
  if (rl == NULL) {
    missingRenderLink(converter);
    return;
  }
  const int num_outputs = this->getNumberOfOutputSockets();
  for (int i = 0; i < num_outputs; i++) {
    NodeOutput *output = this->getOutputSocket(i);
    NodeImageLayer *storage = (NodeImageLayer *)output->getbNodeSocket()->storage;
    RenderPass *rpass = (RenderPass *)BLI_findstring(
        &rl->passes, storage->pass_name, offsetof(RenderPass, name));
    if (rpass == NULL) {
      missingSocketLink(converter, output);
      continue;
    }
    RenderLayersProg *operation;
    bool is_preview;
    if (STREQ(rpass->name, RE_PASSNAME_COMBINED) &&
        STREQ(output->getbNodeSocket()->name, "Alpha")) {
      operation = new RenderLayersAlphaProg(rpass->name, DataType::VALUE, rpass->channels);
      is_preview = false;
    }
    else if (STREQ(rpass->name, RE_PASSNAME_Z)) {
      operation = new RenderLayersDepthProg(rpass->name, DataType::VALUE, rpass->channels);
      is_preview = false;
    }
    else {
      DataType type;
      switch (rpass->channels) {
        case 4:
          type = DataType::COLOR;
          break;
        case 3:
          type = DataType::VECTOR;
          break;
        case 1:
          type = DataType::VALUE;
          break;
        default:
          BLI_assert(!"Unexpected number of channels for pass");
          type = DataType::VALUE;
          break;
      }
      operation = new RenderLayersProg(rpass->name, type, rpass->channels);
      is_preview = STREQ(output->getbNodeSocket()->name, "Image");
    }
    testSocketLink(converter, context, output, operation, scene, view_layer, is_preview);
  }
}

void RenderLayersNode::missingSocketLink(NodeConverter &converter, NodeOutput *output) const
{
  NodeOperation *operation;
  switch (output->getDataType()) {
    case DataType::COLOR: {
      const float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
      SetColorOperation *color_operation = new SetColorOperation();
      color_operation->setChannels(color);
      operation = color_operation;
      break;
    }
    case DataType::VECTOR: {
      const float vector[3] = {0.0f, 0.0f, 0.0f};
      SetVectorOperation *vector_operation = new SetVectorOperation();
      vector_operation->setVector(vector);
      operation = vector_operation;
      break;
    }
    case DataType::VALUE: {
      SetValueOperation *value_operation = new SetValueOperation();
      value_operation->setValue(0.0f);
      operation = value_operation;
      break;
    }
    default: {
      BLI_assert("!Unexpected data type");
      return;
    }
  }

  converter.mapOutputSocket(output, operation->getOutputSocket());
  converter.addOperation(operation);
}

void RenderLayersNode::missingRenderLink(NodeConverter &converter) const
{
  const int num_outputs = this->getNumberOfOutputSockets();
  for (int i = 0; i < num_outputs; i++) {
    NodeOutput *output = this->getOutputSocket(i);
    missingSocketLink(converter, output);
  }
}

void RenderLayersNode::convertToOperations(NodeConverter &converter,
                                           CompositorContext &context) const
{
  Scene *scene = (Scene *)this->getbNode()->id;
  int layer_id = this->getbNode()->custom1;
  ViewLayer *view_layer = (ViewLayer *)BLI_findlink(&scene->view_layers, layer_id);
  Render *re = GlobalMan->renderer()->getSceneRender(scene, view_layer);

  if (re != NULL) {
    testRenderLink(converter, context, re, view_layer);
    RE_ReleaseResult(re);
  }
  else {
    missingRenderLink(converter);
  }
}
