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
 * The Original Code is Copyright (C) 2006 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup cmpnodes
 */

#include "node_composite_util.h"

/* **************** extend  ******************** */

static bNodeSocketTemplate cmp_node_randomize_in[] = {
    {SOCK_FLOAT, N_("Image")},
    {-1, ""},
};
static bNodeSocketTemplate cmp_node_randomize_out[] = {
    {SOCK_FLOAT, N_("Image")},
    {-1, ""},
};

static void node_composit_init_randomize(bNodeTree *UNUSED(ntree), bNode *node)
{
  NodeRandomize *data = MEM_callocN(sizeof(NodeRandomize), "node extend data");
  data->flag = 0;
  data->seed = 0;
  data->min = 0.0f;
  data->max = 1.0f;
  data->variance_down = 1.0f;
  data->variance_up = 1.0f;
  data->variance_steps = 0;
  node->storage = data;
}

void register_node_type_cmp_randomize(void)
{
  static bNodeType ntype;

  cmp_node_type_base(&ntype, CMP_NODE_RANDOMIZE, "Randomize", NODE_CLASS_SCRIPT, 0);
  node_type_socket_templates(&ntype, cmp_node_randomize_in, cmp_node_randomize_out);
  node_type_init(&ntype, node_composit_init_randomize);
  node_type_storage(
      &ntype, "NodeRandomize", node_free_standard_storage, node_copy_standard_storage);

  nodeRegisterType(&ntype);
}
