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

#include "COM_ScaleNode.h"

#include "BKE_node.h"
#include "COM_ExecutionSystem.h"
#include "COM_ScaleOperation.h"
#include "COM_SetValueOperation.h"

ScaleNode::ScaleNode(bNode *editorNode) : Node(editorNode)
{
  /* pass */
}

void ScaleNode::convertToOperations(NodeConverter &converter, CompositorContext &context) const
{
  bNode *bnode = this->getbNode();

  NodeInput *inputSocket = this->getInputSocket(0);
  NodeInput *inputXSocket = this->getInputSocket(1);
  NodeInput *inputYSocket = this->getInputSocket(2);
  NodeOutput *outputSocket = this->getOutputSocket(0);

  switch (bnode->custom1) {
    case CMP_SCALE_RELATIVE: {
      ScaleOperation *operation = new ScaleOperation();
      converter.addOperation(operation);

      converter.mapInputSocket(inputSocket, operation->getInputSocket(0));
      converter.mapInputSocket(inputXSocket, operation->getInputSocket(1));
      converter.mapInputSocket(inputYSocket, operation->getInputSocket(2));
      converter.mapOutputSocket(outputSocket, operation->getOutputSocket(0));
      operation->setRelative(true);
      break;
    }
    case CMP_SCALE_SCENEPERCENT: {
      SetValueOperation *scaleFactorOperation = new SetValueOperation();
      scaleFactorOperation->setValue(context.getRenderData()->size / 100.0f);
      converter.addOperation(scaleFactorOperation);

      ScaleOperation *operation = new ScaleOperation();
      converter.addOperation(operation);

      converter.mapInputSocket(inputSocket, operation->getInputSocket(0));
      converter.addLink(scaleFactorOperation->getOutputSocket(), operation->getInputSocket(1));
      converter.addLink(scaleFactorOperation->getOutputSocket(), operation->getInputSocket(2));
      converter.mapOutputSocket(outputSocket, operation->getOutputSocket(0));
      operation->setRelative(true);

      break;
    }
    case CMP_SCALE_RENDERPERCENT: {
      const RenderData *rd = context.getRenderData();
      int scale_w = rd->xsch * rd->size / 100.0f;
      int scale_h = rd->ysch * rd->size / 100.0f;
      ScaleFixedSizeOperation *operation = new ScaleFixedSizeOperation(scale_w, scale_h);

      /* framing options */
      operation->setIsAspect((bnode->custom2 & CMP_SCALE_RENDERSIZE_FRAME_ASPECT) != 0);
      operation->setIsCrop((bnode->custom2 & CMP_SCALE_RENDERSIZE_FRAME_CROP) != 0);
      operation->setOffset(bnode->custom3, bnode->custom4);
      operation->getInputSocket(0)->setResizeMode(InputResizeMode::NO_RESIZE);
      converter.addOperation(operation);

      converter.mapInputSocket(inputSocket, operation->getInputSocket(0));
      converter.mapOutputSocket(outputSocket, operation->getOutputSocket(0));

      break;
    }
    case CMP_SCALE_ABSOLUTE: {
      ScaleOperation *operation = new ScaleOperation();
      converter.addOperation(operation);

      converter.mapInputSocket(inputSocket, operation->getInputSocket(0));
      converter.mapInputSocket(inputXSocket, operation->getInputSocket(1));
      converter.mapInputSocket(inputYSocket, operation->getInputSocket(2));
      converter.mapOutputSocket(outputSocket, operation->getOutputSocket(0));

      operation->setRelative(false);

      break;
    }
  }
}
