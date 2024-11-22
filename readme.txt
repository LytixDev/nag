--- NAG - Nicolai's Amazing Graph Library ---
A specialized graph algorithms library for directed graphs that can have disconnected components. NAG is designed for use in the metagen compiler (https://github.com/LytixDev/metagan). NAG uses memory arenas from (https://github.com/LytixDev/sac) for allocation.

Algorithms provided:
- DFS -> nag_dfs() 
         nag_dfs_from(start_node)
- BFS -> nag_bfs() 
         nag_bfs_from(start_node)

Both DFS and BFS return the order of which the nodes were visited.

- Tarjan -> nag_scc()
Returns only strongly connected components larger than one. This is because the metagen compiler only cares about identifying circular dependencies aka cycles in the graph. While I used the wikipedia page (https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm) as the basis for the implementation, I started freestyling and my impl diverged quite a lot from the wikipedia pseudocode.

If nag_scc() returns [3, 2, 1], it can be interpreted as:
1 <- 3 <- 2 <- 1,
Or in other words: Nodes 1, 2, and 3 form a SCC because Node 3 points to Node 1 and Node 1 points to Node 2 which points to Node 3.


Reversed topological sorting -> nag_rev_toposort()
Leaf-first instead of root-first as this is useful in the metagen compiler. If you want root-first toposort, just iterate over the array in reversed order :-). 

Further work:
- Implement nag_dfs_from_to(start_node, target_node) and nag_bfs_from_to(start_node, target_node).
- There is a lot of cut-n-pase code the functions share. Does not follow DRY principles!!!11. In reality, this is a non-issue, but just for fun, it would be cool to factor out parts each function share without introducing too much voodoo.
