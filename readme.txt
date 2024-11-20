--- NAG - Nicolai's Amazing Graph Library ---
A specialized graph algorithms library for static directed acyclic graphs that can have disconnected components. NAG is designed for use in the metagen compiler (https://github.com/LytixDev/metagan). NAG uses memory arenas from (https://github.com/LytixDev/sac) for allocation.

Algorithms provided:
- DFS -> nag_dfs() 
         nag_dfs_from(start_node)
- BFS -> nag_bfs() 
         nag_bfs_from(start_node)

Both DFS and BFS return the order of which the nodes were visited.

- Tarjan -> nag_scc()
Returns only strongly connected components larger than one. This is because the metagen compiler only cares about identifying circular dependencies.

Reversed topological sorting -> nag_rev_toposort()
Leaf-first instead of root-first as this is useful in the metagen compiler. If you want root-first toposort, just iterate over the array in reversed order :-). 


Further work:
- Implement nag_scc().
- Implement nag_dfs_from_to(start_node, target_node) and nag_bfs_from_to(start_node, target_node).
- There is a lot of cut-n-pase code the functions share. Does not follow DRY principles!!!11. In reality, this is a non-issue, but just for fun, it would be cool to factor out parts each function share without introducing too much voodoo.
