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

#ifndef __COM_DEFINES_H__
#define __COM_DEFINES_H__

/**
 * \brief Possible data types of images
 */
enum class DataType { VALUE, VECTOR, COLOR };

/**
 * \brief Possible socket types for sockets. For the first 3 there is a corresponding DataType
 * which will be the type of data that the socket has to input or output. For the DYNAMIC socket
 * type, the type of data that will input or output will be the DataType of the linked operation
 * input
 */
enum class SocketType { VALUE, VECTOR, COLOR, DYNAMIC };

enum class WriteType {
  // A full operation rect will be given to the write method with will be executed in a single
  // thread. Should be used only when the algorithm you need to implement for any reason requires
  // you to linearly write from position (0,0) to (width,height),
  // so that if you receive sliced rects the algorithm can't be implemented.
  // This is not compatible with computing, only can be done in CPU.
  SINGLE_THREAD,
  // Sliced rects will be given randomly to the write method one by one. Write method calls will be
  // done in parallel with each rect from different threads. This is the default
  MULTI_THREAD
};

enum class BufferType {
  /** \brief Temporal buffering in reused buffers */
  TEMPORAL,
  /** \brief Temporal buffering + cached in memory for next executions */
  CACHED,
  /** \brief The operation itself manages the creation, writing and
     destruction of the buffer. This is the case of already created images or that have been
     written on initialization */
  CUSTOM,
  /**
   * \brief
   * Might be used for operations that write to some external or internal output, or for single
   * element operations that needs reading it's input operations to calculate (write) the single
   * element.
   */
  NO_BUFFER_WITH_WRITE,
  /** Single element operations with a known value after initialization that needs no reading of
     input operations must use this type of buffering. Output view operations that need no update
     might use this type too. */
  NO_BUFFER_NO_WRITE
};

/**
 * \brief Possible quality settings
 * \see CompositorContext.quality
 * \ingroup Execution
 */
enum class CompositorQuality { HIGH = 0, MEDIUM = 1, LOW = 2 };

/**
 * \brief Resize modes of input sockets
 * How are the input and working resolutions matched
 * \ingroup Model
 */
enum class InputResizeMode {
  /** \brief Center the input image to the center of the working area of the node, no resizing
     occurs */
  CENTER,
  /** \brief The bottom left of the input image is the bottom left of the working area of the node,
     no resizing occurs */
  NO_RESIZE,
  /** \brief Fit the width of the input image to the width of the working area of the node */
  FIT_WIDTH,
  /** \brief Fit the height of the input image to the height of the working area of the node */
  FIT_HEIGHT,
  /** \brief Fit the width or the height of the input image to the width or height of the working
     area of the node, image will be larger than the working area */
  FIT,
  /** \brief Fit the width and the height of the input image to the width and height of the working
     area of the node, image will be equally larger than the working area */
  STRETCH
};

/**
 * \brief The order of chunks to be scheduled
 * \ingroup Execution
 */
enum class OrderOfChunks {
  /** \brief order from a distance to centerX/centerY */
  CENTER_OUT = 0,
  /** \brief order randomly */
  RANDOM = 1,
  /** \brief no ordering */
  TOP_DOWN = 2,
  /** \brief experimental ordering with 9 hotspots */
  RULE_OF_THIRDS = 3
};

enum class OperationMode { Optimize, Exec };

#define COM_NUM_CHANNELS_VALUE 1
#define COM_NUM_CHANNELS_VECTOR 3
#define COM_NUM_CHANNELS_COLOR 4

#define COM_BLUR_BOKEH_PIXELS 512

// configurable items

// chunk size determination
#define COM_PREVIEW_SIZE 140.0f
#define COM_OPENCL_ENABLED

// workscheduler threading models
/**
 * COM_TM_QUEUE is a multi-threaded model, which uses the BLI_thread_queue pattern.
 * This is the default option.
 */
#define COM_TM_QUEUE 1

/**
 * COM_TM_NOTHREAD is a single threading model, everything is executed in the caller thread.
 * easy for debugging
 */
#define COM_TM_NOTHREAD 0

/**
 * COM_CURRENT_THREADING_MODEL can be one of the above, COM_TM_QUEUE is currently default in
 * release.
 */
#if defined(COM_DEBUG) || defined(DEBUG)
#  define COM_CURRENT_THREADING_MODEL COM_TM_NOTHREAD
#  define ALLOW_COMPUTE 0
#else
#  define COM_CURRENT_THREADING_MODEL COM_TM_QUEUE
#  define ALLOW_COMPUTE 1
#endif

enum class ComDebugLevel {
  NORMAL,  // without graphviz output
  FULL     // with graphviz output
};
const ComDebugLevel COM_DEBUG_LEVEL = ComDebugLevel::NORMAL;

// chunk order
#define COM_ORDER_OF_CHUNKS_DEFAULT OrderOfChunks::CENTER_OUT

#define COM_RULE_OF_THIRDS_DIVIDER 100.0f
//
//#if COM_CURRENT_THREADING_MODEL == COM_TM_NOTHREAD
//#  if !(defined(DEBUG) || \
//        defined(COM_DEBUG)) /* test this so we dont get warnings in debug builds */
//#    warning COM_CURRENT_THREADING_MODEL COM_TM_NOTHREAD is activated. Use only for debugging.
//#  endif
//#elif COM_CURRENT_THREADING_MODEL == COM_TM_QUEUE
///* do nothing - default */
//#else
//#  error COM_CURRENT_THREADING_MODEL No threading model selected
//#endif

#endif /* __COM_DEFINES_H__ */
