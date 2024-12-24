/*
 *  Copyright (C) 2024 Nicolai Brand (https://lytix.dev)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef NAG_H
#define NAG_H

/* NOTE: Copy of type.h found in the metagen project  */
#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

#define U8_MIN 0u
#define U8_MAX 0xffu
#define I8_MIN (-0x7f - 1)
#define I8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define I16_MIN (-0x7fff - 1)
#define I16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define I32_MIN (-0x7fffffff - 1)
#define I32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define I64_MIN (-0x7fffffffffffffffll - 1)
#define I64_MAX 0x7fffffffffffffffll

#endif /* TYPE_H */

#include "sac_single.h"


/* 
 * The index type.
 * If u16 does not suffice, just change this typedef.
 */
typedef u16 NAG_Idx;

#define NAG_STACK_GROW_SIZE (NAG_Idx)256 // at least 8
#define NAG_QUEUE_GROW_SIZE (NAG_Idx)32 // at least 8
                                        //
#define NAG_MIN(a, b) ((a) < (b) ? (a) : (b))

#define NAG_UNDISCOVERED U16_MAX

typedef struct nag_graph_node_t NAG_GraphNode;
struct nag_graph_node_t {
    NAG_Idx id;
    NAG_GraphNode *next;
};

typedef struct {
    NAG_Idx n_nodes;
    NAG_GraphNode **neighbor_list;
    Arena *scratch_arena;
    Arena *persist_arena;
} NAG_Graph;

typedef struct {
    NAG_Idx n_nodes;
    NAG_Idx *nodes; // of n_nodes len
} NAG_Order;

typedef struct {
    u32 n; // how many orders
    NAG_Order *orders; // NOTE: Heap allocated!
} NAG_OrderList;


NAG_Graph nag_make_graph(Arena *persist, Arena *scratch, NAG_Idx n_nodes);
/* Expects node indices between 0 and graph->n_nodes - 1 */
void nag_add_edge(NAG_Graph *graph, NAG_Idx from, NAG_Idx to);
void nag_print(NAG_Graph *graph);

NAG_OrderList nag_dfs(NAG_Graph *graph);
NAG_Order nag_dfs_from(NAG_Graph *graph, NAG_Idx start_node);

NAG_OrderList nag_bfs(NAG_Graph *graph);
NAG_Order nag_bfs_from(NAG_Graph *graph, NAG_Idx start_node);

/* Assumes graph contains no cycles */
NAG_Order nag_rev_toposort(NAG_Graph *graph);

NAG_OrderList nag_scc(NAG_Graph *graph);



#endif /* NAG_H */
