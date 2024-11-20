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
#include <assert.h>
#include <stdio.h>
#include <string.h> // why the hell is memset here

#include "nag.h"




NAG_Graph nag_make_graph(Arena *arena, NAG_Idx n_nodes)
{
    NAG_Graph graph = { .arena = arena,
                        .n_nodes = n_nodes,
                        .neighbor_list = m_arena_alloc(arena, sizeof(NAG_GraphNode) * n_nodes ) };
    return graph;
}

void nag_add_edge(NAG_Graph *graph, NAG_Idx from, NAG_Idx to)
{
    assert(from <= graph->n_nodes);
    // NOTE: This is the most naive way we can add eges and probably quite poor for performance 
    //       I will improve this if/when it becomes noticable.
    NAG_GraphNode *first = graph->neighbor_list[from];
    NAG_GraphNode *new_node = m_arena_alloc(graph->arena, sizeof(NAG_GraphNode));
    new_node->id = to;
    new_node->next = first;
    graph->neighbor_list[from] = new_node;
}

void nag_print(NAG_Graph *graph)
{
    for (NAG_Idx i = 0; i < graph->n_nodes; i++) {
        printf("[%d] -> ", i);
        NAG_GraphNode *node = graph->neighbor_list[i];
        while (node != NULL) {
            printf("%d, ", node->id);
            node = node->next;
        }
        putchar('\n');
    }
}

static NAG_Order nag_dfs_internal(NAG_Graph *graph, NAG_Idx start_node, u8 *visited)
{
    NAG_Idx ordered_len = 0;
    NAG_Idx *ordered = m_arena_alloc(graph->arena, sizeof(NAG_Idx) * graph->n_nodes);

    /* 
     * Everything from here on and below is dynamic temporary data that will be linearly allocated
     * on the temporary arena and released before returning.
     */
    ArenaTmp tmp_arena = m_arena_tmp_init(graph->arena);

    NAG_Idx stack_size = NAG_STACK_GROW_SIZE;
    NAG_Idx stack_top = 1;
    NAG_Idx *stack = m_arena_alloc_internal(graph->arena, sizeof(NAG_Idx) * stack_size, 4, false);
    stack[0] = start_node;

    while (stack_top != 0) {
        NAG_Idx current_node = stack[--stack_top];
        if (visited[current_node]) {
            continue;
        }
        visited[current_node] = true;
        ordered[ordered_len++] = current_node;
        for (NAG_GraphNode *n = graph->neighbor_list[current_node]; n != NULL; n = n->next) {
            stack[stack_top++] = n->id;
            if (stack_top == stack_size) {
                /* Linear increases of the allocation for the stack */
                m_arena_alloc_internal(graph->arena, sizeof(NAG_Idx) * NAG_STACK_GROW_SIZE, 4, false);
                stack_size += NAG_STACK_GROW_SIZE;
            }
        }
    }
    /* NOTE(nic): This only reclaims the memory to the arena, not to the OS */
    m_arena_tmp_release(tmp_arena);
    return (NAG_Order){ .n_nodes = ordered_len, .nodes = ordered };
}

NAG_Order nag_dfs_from(NAG_Graph *graph, NAG_Idx start_node)
{
    u8 *visited = m_arena_alloc(graph->arena, sizeof(bool) * graph->n_nodes);
    memset(visited, false, sizeof(NAG_Idx) * graph->n_nodes);
    return nag_dfs_internal(graph, start_node, visited);
}

NAG_OrderList nag_dfs(NAG_Graph *graph)
{
    u8 *visited = m_arena_alloc(graph->arena, sizeof(bool) * graph->n_nodes);
    memset(visited, false, sizeof(NAG_Idx) * graph->n_nodes);
    NAG_OrderList result = {0};
    u32 n_orders_allocated = 8;
    result.orders = malloc(sizeof(NAG_Order) * n_orders_allocated);

    for (NAG_Idx i = 0; i < graph->n_nodes; i++) {
        if (visited[i]) {
            continue;
        }
        NAG_Order dfs_from_i = nag_dfs_internal(graph, i, visited);
        result.orders[result.n++] = dfs_from_i;
        if (result.n == n_orders_allocated) {
            n_orders_allocated += 8;
            result.orders = realloc(result.orders, sizeof(NAG_Order) * n_orders_allocated);
        }
    }
    return result;
}

static NAG_Order nag_bfs_internal(NAG_Graph *graph, NAG_Idx start_node, u8 *visited)
{
    NAG_Idx ordered_len = 0;
    NAG_Idx *ordered = m_arena_alloc(graph->arena, sizeof(NAG_Idx) * graph->n_nodes);

    ArenaTmp tmp_arena = m_arena_tmp_init(graph->arena);
    NAG_Idx queue_size = NAG_QUEUE_GROW_SIZE;
    NAG_Idx queue_low = 0;
    NAG_Idx queue_high = 1;
    NAG_Idx *queue = m_arena_alloc_internal(graph->arena, sizeof(NAG_Idx) * NAG_QUEUE_GROW_SIZE, 4, false);
    queue[0] = start_node;

    while (queue_low != queue_high) {
        NAG_Idx current_node = queue[queue_low++];
        if (visited[current_node]) {
            continue;
        }
        visited[current_node] = true;
        ordered[ordered_len++] = current_node;
        
        for (NAG_GraphNode *n = graph->neighbor_list[current_node]; n != NULL; n = n->next) {
            queue[queue_high++] = n->id;
            /* 
             * Queue is full.
             * If we have a lot of unused space to the left, we shift the entire queue
             * downards. If not, we increase the allocation.
             */
            if (queue_high == queue_size) {
                /* Shift left */
                if (queue_low > queue_size / 2) {
                    memmove(queue, queue + queue_low, queue_high - queue_low + 1);
                    queue_high -= queue_low;
                    queue_low = 0;
                }
                /* Increase the allocation */
                m_arena_alloc_internal(graph->arena, sizeof(NAG_Idx) * NAG_QUEUE_GROW_SIZE, 4, false);
                queue_size += NAG_QUEUE_GROW_SIZE;
            }
        }
    }
    m_arena_tmp_release(tmp_arena);
    return (NAG_Order){ .n_nodes = ordered_len, .nodes = ordered };
}

NAG_Order nag_bfs_from(NAG_Graph *graph, NAG_Idx start_node)
{
    u8 *visited = m_arena_alloc(graph->arena, sizeof(bool) * graph->n_nodes);
    memset(visited, false, sizeof(NAG_Idx) * graph->n_nodes);
    return nag_bfs_internal(graph, start_node, visited);
}

NAG_OrderList nag_bfs(NAG_Graph *graph)
{
    u8 *visited = m_arena_alloc(graph->arena, sizeof(bool) * graph->n_nodes);
    memset(visited, false, sizeof(NAG_Idx) * graph->n_nodes);
    NAG_OrderList result = {0};
    u32 n_orders_allocated = 8;
    result.orders = malloc(sizeof(NAG_Order) * n_orders_allocated);

    for (NAG_Idx i = 0; i < graph->n_nodes; i++) {
        if (visited[i]) {
            continue;
        }
        NAG_Order dfs_from_i = nag_bfs_internal(graph, i, visited);
        result.orders[result.n++] = dfs_from_i;
        if (result.n == n_orders_allocated) {
            n_orders_allocated += 8;
            result.orders = realloc(result.orders, sizeof(NAG_Order) * n_orders_allocated);
        }
    }
    return result;
}


static NAG_Order nag_toposort_from_internal(NAG_Graph *graph, NAG_Idx start_node, u8 *visited)
{
    NAG_Idx ordered_len = 0;
    NAG_Idx *ordered = m_arena_alloc(graph->arena, sizeof(NAG_Idx) * graph->n_nodes);

    /* 
     * Everything from here on and below is dynamic temporary data that will be linearly allocated
     * on the temporary arena and released before returning.
     */
    ArenaTmp tmp_arena = m_arena_tmp_init(graph->arena);

    NAG_Idx stack_size = NAG_STACK_GROW_SIZE;
    NAG_Idx stack_top = 1;
    NAG_Idx *stack = m_arena_alloc_internal(graph->arena, sizeof(NAG_Idx) * stack_size, 4, false);
    stack[0] = start_node;

    while (stack_top != 0) {
        NAG_Idx current_node = stack[--stack_top];
        if (visited[current_node] == NAG_NODE_COMPLETED) {
            ordered[ordered_len++] = current_node;
            continue;
        }

        visited[current_node] = NAG_NODE_COMPLETED;
        stack[stack_top++] = current_node;
        if (stack_top == stack_size) {
            m_arena_alloc_internal(graph->arena, sizeof(NAG_Idx) * NAG_STACK_GROW_SIZE, 4, false);
            stack_size += NAG_STACK_GROW_SIZE;
        }

        for (NAG_GraphNode *n = graph->neighbor_list[current_node]; n != NULL; n = n->next) {
            stack[stack_top++] = n->id;
            if (stack_top == stack_size) {
                /* Linear increases of the allocation for the stack */
                m_arena_alloc_internal(graph->arena, sizeof(NAG_Idx) * NAG_STACK_GROW_SIZE, 4, false);
                stack_size += NAG_STACK_GROW_SIZE;
            }
        }
    }
    /* NOTE(nic): This only reclaims the memory to the arena, not to the OS */
    m_arena_tmp_release(tmp_arena);
    return (NAG_Order){ .n_nodes = ordered_len, .nodes = ordered };
}

NAG_OrderList nag_rev_toposort(NAG_Graph *graph)
{
    u8 *visited = m_arena_alloc(graph->arena, sizeof(bool) * graph->n_nodes);
    memset(visited, NAG_NODE_UNVISITED, sizeof(NAG_Idx) * graph->n_nodes);
    NAG_OrderList result = {0};
    u32 n_orders_allocated = 8;
    result.orders = malloc(sizeof(NAG_Order) * n_orders_allocated);

    for (NAG_Idx i = 0; i < graph->n_nodes; i++) {
        if (visited[i] == NAG_NODE_UNVISITED) {
            NAG_Order dfs_from_i = nag_toposort_from_internal(graph, i, visited);
            result.orders[result.n++] = dfs_from_i;
            if (result.n == n_orders_allocated) {
                n_orders_allocated += 8;
                result.orders = realloc(result.orders, sizeof(NAG_Order) * n_orders_allocated);
            }
        }
    }
    return result;
}
