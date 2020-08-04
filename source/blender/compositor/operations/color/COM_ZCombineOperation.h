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

<<<<<<< HEAD
#pragma once

=======
#ifndef __COM_ZCOMBINEOPERATION_H__
#define __COM_ZCOMBINEOPERATION_H__
>>>>>>> f7ffde26628... First commmit
#include "COM_MixOperation.h"

/**
 * this program converts an input color to an output value.
 * it assumes we are in sRGB color space.
 */
class ZCombineOperation : public NodeOperation {
 public:
  ZCombineOperation();

 protected:
  virtual void execPixels(ExecutionManager &man) override;
};
class ZCombineAlphaOperation : public ZCombineOperation {
 protected:
  virtual void execPixels(ExecutionManager &man) override;
};

class ZCombineMaskOperation : public NodeOperation {
 public:
  ZCombineMaskOperation();

 protected:
  virtual void execPixels(ExecutionManager &man) override;
};
class ZCombineMaskAlphaOperation : public ZCombineMaskOperation {
 protected:
  virtual void execPixels(ExecutionManager &man) override;
};
<<<<<<< HEAD
=======

#endif
>>>>>>> f7ffde26628... First commmit
