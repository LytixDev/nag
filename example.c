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
#include "nag.h"

#define SAC_IMPLEMENTATION
#include "sac_single.h"


void nag_order_print(NAG_Order order)
{
    for (u32 i = 0; i < order.n_nodes; i++) {
        printf("%d ", order.nodes[i]);
    }
    printf("\n");
}

void simple_dfs_bfs()
{
    Arena persist, scratch;
    m_arena_init_dynamic(&persist, 2, 4096);
    m_arena_init_dynamic(&scratch, 2, 4096);

    u32 n_nodes = 9;
    NAG_Graph graph = nag_make_graph(&persist, &scratch, n_nodes);

    nag_add_edge(&graph, 0, 1);
    nag_add_edge(&graph, 0, 4);
    nag_add_edge(&graph, 1, 2);
    nag_add_edge(&graph, 1, 6);
    nag_add_edge(&graph, 2, 3);
    nag_add_edge(&graph, 4, 5);
    nag_add_edge(&graph, 4, 8);
    nag_add_edge(&graph, 6, 7);

    //nag_print(&graph);

    NAG_Order dfs_order = nag_dfs_from(&graph, 0);
    printf("--- dfs ---\n");
    nag_order_print(dfs_order);

    NAG_Order bfs_order = nag_bfs_from(&graph, 0);
    printf("--- bfs ---\n");
    nag_order_print(bfs_order);

    m_arena_release(&persist);
    m_arena_release(&scratch);
}

void toposort()
{
    Arena persist, scratch;
    m_arena_init_dynamic(&persist, 2, 4096);
    m_arena_init_dynamic(&scratch, 2, 4096);

    u32 n_nodes = 9;
    NAG_Graph graph = nag_make_graph(&persist, &scratch, n_nodes);

    nag_add_edge(&graph, 0, 1);
    nag_add_edge(&graph, 0, 2);
    nag_add_edge(&graph, 1, 3);
    nag_add_edge(&graph, 3, 4);
    /* disconnect */
    nag_add_edge(&graph, 5, 6);
    nag_add_edge(&graph, 5, 7);
    nag_add_edge(&graph, 5, 8);
    nag_add_edge(&graph, 8, 9);

    NAG_OrderList r = nag_dfs(&graph);
    printf("--- dfs ---\n");
    for (u32 i = 0; i < r.n; i++) {
        printf("[%d]: ", i);
        nag_order_print(r.orders[i]);
    }
    free(r.orders);

    r = nag_bfs(&graph);
    printf("--- bfs ---\n");
    for (u32 i = 0; i < r.n; i++) {
        printf("[%d]: ", i);
        nag_order_print(r.orders[i]);
    }
    free(r.orders);

    r = nag_rev_toposort(&graph);
    printf("--- reversed toposort ---\n");
    for (u32 i = 0; i < r.n; i++) {
        printf("[%d]: ", i);
        nag_order_print(r.orders[i]);
    }
    free(r.orders);

    m_arena_release(&persist);
    m_arena_release(&scratch);
}

void scc()
{
    Arena persist, scratch;
    m_arena_init_dynamic(&persist, 2, 4096);
    m_arena_init_dynamic(&scratch, 2, 4096);

    u32 n_nodes = 6;
    NAG_Graph graph = nag_make_graph(&persist, &scratch, n_nodes);

    nag_add_edge(&graph, 1, 4);
    nag_add_edge(&graph, 4, 5);
    nag_add_edge(&graph, 5, 4);

    nag_add_edge(&graph, 0, 1);
    nag_add_edge(&graph, 1, 2);
    nag_add_edge(&graph, 2, 3);
    nag_add_edge(&graph, 3, 1);


    NAG_OrderList r = nag_scc(&graph);
    printf("--- scc ---\n");
    for (u32 i = 0; i < r.n; i++) {
        printf("[%d]: ", i);
        nag_order_print(r.orders[i]);
    }
    free(r.orders);
    m_arena_release(&persist);
    m_arena_release(&scratch);
}

void scc2()
{
    Arena persist, scratch;
    m_arena_init_dynamic(&persist, 2, 4096);
    m_arena_init_dynamic(&scratch, 2, 4096);

    u32 n_nodes = 3;
    NAG_Graph graph = nag_make_graph(&persist, &scratch, n_nodes);

    nag_add_edge(&graph, 0, 1);
    nag_add_edge(&graph, 0, 2);
    nag_add_edge(&graph, 1, 2);

    NAG_OrderList r = nag_scc(&graph);
    printf("--- scc ---\n");
    for (u32 i = 0; i < r.n; i++) {
        printf("[%d]: ", i);
        nag_order_print(r.orders[i]);
    }
    free(r.orders);
    m_arena_release(&persist);
    m_arena_release(&scratch);
}

int main(void)
{
    printf("[Example 1]: dfs & bfs\n");
    simple_dfs_bfs();

    printf("[Example 2]: reversed toposort\n");
    toposort();

    printf("[Example 3] scc:\n");
    scc();

    printf("[Example 4] scc 2:\n");
    scc2();
}
