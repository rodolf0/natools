#include <stdlib.h>
#include "baas/graph.h"
#include "baas/heap.h" /* for dijkstra's algorithm */

graph_t * graph_init(free_func_t vfree) {
  graph_t *g = malloc(sizeof(graph_t));
  /* let stdlib's free clean graph_vertex_t structures */
  g->vertex = list_init(free, NULL);
  g->free = vfree; /* this one is for node data */
  return g;
}

void graph_destroy(graph_t *g) {
  list_node_t *it;
  for (it = g->vertex->first; it != NULL; it = it->next) {
    graph_vertex_t *v = (graph_vertex_t *)it->data;
    /* edge structures freed by provided stdlib's free (graph_add_edge) */
    if (v->edges) list_destroy(v->edges);
    /* free vertex data */
    if (g->free) g->free(v->data);
  }
  /* list cleanup will delete graph_vertex_t data structures */
  list_destroy(g->vertex);
  free(g);
}

graph_vertex_t * graph_add_vertex(graph_t *g, void *d) {
  graph_vertex_t *v = malloc(sizeof(graph_vertex_t));
  v->data = d;
  v->edges = NULL;
  /* add tracking info  */
  v->path.visited = 0;
  v->path.distance = 0.0;
  v->path.prev = NULL;
  list_push(g->vertex, v);
  return v;
}

graph_edge_t * graph_add_edge(graph_vertex_t *orig,
                    graph_vertex_t *dest, double weight) {
  graph_edge_t *e = malloc(sizeof(graph_edge_t));
  e->connection = dest;
  e->weight = weight;
  /* provide free func to clean up on graph_edge_t alloc's */
  if (!orig->edges)
    orig->edges = list_init(free, NULL);
  list_queue(orig->edges, e);
  return e;
}

void graph_clear_pathtrace(graph_t *g) {
  list_node_t *it;
  for (it = g->vertex->first; it != NULL; it = it->next) {
    graph_vertex_t *v = (graph_vertex_t *)it->data;
    v->path.visited = 0;
    v->path.distance = 0.0;
    v->path.prev = NULL;
  }
}

/* Explore the graph (we'll go over each reachable vertex once only).
 * The order in which vertex are explored depends on the explore function.
 *
 * previsit: call before doing anything on the vertex
 * postvisit: call after having visited all reachable edges, this means
 *            for DFS it will be called after the whole hierarchy under
 *            a vertex has being visited, for BFS after only direct childs
 *            and for other exploring functions a mix (eg: if using a heap).
 * */

/* Does a depth first search marking all reachable nodes (->visited = 1) */
void graph_explore_dfs(graph_t *g, graph_vertex_t *s, graph_vertex_t *e,
                       visit_func_t pre, visit_func_t post) {
  /* clear graph search assets unless we're exploring the whole graph */
  if (g) graph_clear_pathtrace(g);

  /* just a simple stack for depth first */
  list_t *stack = list_init(NULL, NULL);
  list_push(stack, s);

  while ((s = list_peek_head(stack))) {
    int clock = 0;
    list_node_t *it;
    /* break if searching for a specific destination */
    if (e && e == s) break;

    /* back at the parent: call post-operations */
    if (s->path.visited) {
      s = list_pop(stack);
      if (post) post(s);
      s->path.post_clock = clock++;
      continue;
    }

    s->path.visited = 1;
    s->path.pre_clock = clock++;

    if (pre) pre(s);
    for (it = s->edges->last; it != NULL; it = it->prev) {
      graph_vertex_t *v = ((graph_edge_t *)it->data)->connection;
      /* avoid back edges */
      if (!v->path.visited) {
        v->path.prev = s;
        v->path.distance = s->path.distance + 1;
        list_push(stack, v);
      }
    }
  }
  list_destroy(stack);
}

/* mark every vertex reachable from s with the distance to get there */
void graph_explore_bfs(graph_t *g, graph_vertex_t *s, graph_vertex_t *e,
                       visit_func_t visit) {
  /* clear graph search assets unless we're exploring the whole graph */
  if (g) graph_clear_pathtrace(g);
  /* mark starting point's prev as a self reference to avoid looping back */
  s->path.prev = s;
  /* just a simple queue for breadth first search */
  list_t *queue = list_init(NULL, NULL);
  list_queue(queue, s);

  while ((s = list_pop(queue))) {
    int clock = 0;
    list_node_t *it;
    /* break if searching for a specific destination */
    if (e && e == s) break;

    s->path.visited = 1;
    s->path.pre_clock = clock++;
    if (visit) visit(s);
    for (it = s->edges->first; it != NULL; it = it->next) {
      graph_vertex_t *v = ((graph_edge_t *)it->data)->connection;
      if (!v->path.prev) {
        v->path.prev = s;
        v->path.distance = s->path.distance + 1;
        list_queue(queue, v);
      }
    }
  }
  list_destroy(queue);
}

/* a comparison function to structure the (max)heap */
static inline int min_path_cmp(const graph_vertex_t *x,
                               const graph_vertex_t *y) {
  return (x->path.distance < y->path.distance ? 1 :
            x->path.distance > y->path.distance ? -1 : 0);
}

/* return a list of nodes representing the shortest path from v
 * we can't have negative edges, distances are either an overestimate
 * or exact (that's why we don't update downstream nodes */
void graph_explore_dijkstra(graph_t *g, graph_vertex_t *s,
                            graph_vertex_t *e, visit_func_t visit) {
  /* clear graph search assets unless we're exploring the whole graph */
  if (g) graph_clear_pathtrace(g);
  /* mark starting point's prev as a self reference to avoid looping back */
  s->path.prev = s;
  /* just a simple queue for breadth first search */
  heap_t *prioq = heap_init(NULL, (cmp_func_t)min_path_cmp);
  heap_insert(prioq, s);

  while ((s = heap_pop(prioq))) {
    int clock = 0;
    list_node_t *it;
    /* break if searching for a specific destination */
    if (e && e == s) break;

    s->path.visited = 1;
    s->path.pre_clock = clock++;
    if (visit) visit(s);
    for (it = s->edges->first; it != NULL; it = it->next) {
      graph_edge_t *ve = (graph_edge_t *)it->data;
      graph_vertex_t *v = ve->connection;
      if (!v->path.prev ||
          v->path.distance > s->path.distance + ve->weight) {

        v->path.prev = s;
        v->path.distance = s->path.distance + ve->weight;

        /* check if destination is already on the heap */
        size_t idx = heap_element_idx(prioq, v);
        if (idx == prioq->size)
          heap_insert(prioq, v);
        else
          heap_bubble_up(prioq, v, idx);
      }
    }
  }
  heap_destroy(prioq);
}

/* order vertex in decreasing post-clock order for linearizing */
static int post_clock_cmp(const graph_vertex_t *x, const graph_vertex_t *y) {
  return (x->path.post_clock > y->path.post_clock ? -1 :
            x->path.post_clock < y->path.post_clock ? 1 : 0);
}

void graph_dag_shortest_path(graph_t *g, graph_vertex_t *s) {
  /* linearize graph, TODO: consider not using original list for sorting?  */
  graph_explore_dfs(g, s, NULL, NULL, NULL);
  cmp_func_t oldcmp = g->vertex->cmp;
  g->vertex->cmp = (cmp_func_t)post_clock_cmp;
  g->vertex = list_mergesort(g->vertex);
  g->vertex->cmp = oldcmp;

  list_node_t *it;
  for (it = g->vertex->first; it != NULL; it = it->next) {
    graph_vertex_t *u = (graph_vertex_t *)it->data;
    /* update downstream distances */
    if (u->edges && u->edges->size > 0) {
      list_node_t *et;
      for (et = u->edges->first; et != NULL; et = et->next) {
        graph_edge_t *e = (graph_edge_t *)et->data;
        graph_vertex_t *v = e->connection;
        if (!v->path.prev ||
            v->path.distance > u->path.distance + e->weight) {
          v->path.prev = u;
          v->path.distance = u->path.distance + e->weight;
        }
      }
    }
  }
}

/* vim: set sw=2 sts=2 : */
