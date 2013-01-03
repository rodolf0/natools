#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "common.h"
#include "list.h"

typedef struct graph_vertex_t {
  /* keep a list of reachable vertex */
  list_t *edges;
  /* vertex data */
  void *data;
  /* aux data for path search */
  struct {
    /* node has being visited and it's edges explored */
    int visited;
    /* times of pre visit / post visit */
    int pre_clock, post_clock;
    /* node from wich this was reached */
    struct graph_vertex_t *prev;
    /* total distance to here */
    double distance;
  } path;
} graph_vertex_t;

typedef struct graph_edge_t {
  /* vertex that the edge connects to */
  graph_vertex_t *connection;
  /* edge traversal cost */
  double weight;
} graph_edge_t;

typedef struct graph_t {
  list_t *vertex;
  /* function to free vertex data */
  free_func_t free;
} graph_t;

/* function to call uppon node visit */
typedef void * (*visit_func_t)(graph_vertex_t *);

graph_t * graph_init(free_func_t);
void graph_destroy(graph_t *g);

graph_vertex_t * graph_add_vertex(graph_t *g, void *d);
graph_edge_t * graph_add_edge(graph_vertex_t *orig,
                              graph_vertex_t *dest, double weight);

void graph_clear_pathtrace(graph_t *g);

/* If end vertex (e) is specified, search will end as soon as found. Else
 * it will explore the whole graph to link it according to the chosen algo */
void graph_explore_dfs(graph_t *g, graph_vertex_t *s, graph_vertex_t *e,
                       visit_func_t pre, visit_func_t post);
void graph_explore_bfs(graph_t *g, graph_vertex_t *s, graph_vertex_t *e,
                       visit_func_t visit);
void graph_explore_dijkstra(graph_t *g, graph_vertex_t *s,
                            graph_vertex_t *e, visit_func_t visit);
void graph_dag_shortest_path(graph_t *g, graph_vertex_t *s);
#endif /* _GRAPH_H_ */

/* vim: set sw=2 sts=2 : */
