--- NAG - Nicolai's Amazing Graph Library ---
A specialized graph algorithms library for static directed acyclic graphs that can have disconnected components. NAG is designed for use in the metagen compiler (https://github.com/LytixDev/metagan). NAG uses memory arenas from (https://github.com/LytixDev/sac) for allocation.

Algorithms provided:
- DFS -> nag_dfs() or nag_dfs_from(start_node)
- BFS -> nag_bfs() or nag_bfs_from(start_node)
- Tarjan -> nag_scc()
    Returns only strongly connected components larger than one. This is because the metagen compiler only cares about identifying circular dependencies.
Topological sorting -> nag_toposort()
