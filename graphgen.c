/**
 * Random graph generator.
 *
 * Prints graph adjacency matrix to stdout.
 *
 * This software is in public domain.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

typedef char ** char_matrix;

typedef struct graph_st
{
  int vertices, edges;
  char_matrix matrix;
} Graph_t;

typedef struct options
{
  unsigned int oriented : 1;
  unsigned int no_contours : 1;
} Options_t;

Graph_t * new_graph(int vertices, int edges);
void print_graph(Graph_t * g);
int generate_graph(Graph_t * g, Options_t opts);
void usage(void);

int main(int argc, char * argv[])
{
  unsigned int vertices = 0, edges = 0;

  int oc;
  extern char * optarg;
  
  struct timeval now;
  Graph_t * graph;
  struct options opts;
  
  if (argc < 2)
    {
      usage();
      return 1;
    }

  while ((oc = getopt(argc, argv, "olcv:e:")) != -1)
    {
      switch (oc)
        {
        case 'o':
          opts.oriented = 1;
          break;
        case 'c':
          opts.no_contours = 1;
          break;
        case 'v':
          vertices = atoi(optarg);
          break;
        case 'e':
          edges = atoi(optarg);
          break;
        }
    }

  if (!(vertices && edges))
    {
      usage();
      return 1;
    }
  
  /**
     Check if it's possible to meet user's requirements
  */
  if (vertices < edges)
    {
      printf("Cannot generate graph with %d vertices and %d edges", vertices, edges);
      return 2;
    }

  gettimeofday(&now, NULL);
  srandom((unsigned long int)(now.tv_usec));

  graph = new_graph(vertices, edges);
  if (generate_graph(graph, opts) == -1)
    {
      printf("Failed to generate graph!");
      return 2;
    }
  
  print_graph(graph);
  return 0;
}

/**
   Allocate memory for graph adjacency matrix

   @param vertices Vertice count in graph.
*/
Graph_t * new_graph(int vertices, int edges)
{
  int i;
  char_matrix matrix_h, matrix;
  
  Graph_t * graph = (Graph_t *)malloc(sizeof(Graph_t));

  matrix_h = matrix = (char_matrix)malloc(vertices*sizeof(char*));
  for (i = 0; i < vertices; i++)
    {
      *matrix++ = (char *)malloc(vertices*sizeof(char));
    }
  graph->vertices = vertices;
  graph->edges = edges;
  graph->matrix = matrix_h;
    
  return graph;
}

/**
   Fill graph adjacency matrix with good random values to produce a
   new random graph
*/
int generate_graph(Graph_t * g, Options_t opts)
{
  int count = 0;
  long int r;
  int i, j;
  long int size = g->vertices * g->vertices;

  if (g->edges > (size - g->vertices))
    return -1;
  
  for (count = 0; count < g->edges; count++)
    {
      r = random() % size;
      i = r / g->vertices;
      j = r - i * g->vertices;

      /**
         Skip in several cases
      */
      if (g->matrix[i][j] ||           \
          (i==j) ||                    \
          (opts.no_contours && i > j)  \
          )
        {
          count--;
          continue;
        }

      g->matrix[i][j] = 1;
      if (!opts.oriented)
        g->matrix[j][i] = 1;
    }
  
  return 0;
}
  
void usage(void)
{
  printf("Usage: graphgen [OPTIONS] -vVERTICES -eEDGES\n");
}

void print_graph(Graph_t * g)
{
  int i, j;
  for (i = 0; i < g->vertices; i++)
    {
      for (j = 0; j < g->vertices; j++)
        {
          printf("%d ", g->matrix[i][j]);
        }
      putchar('\n');
    }
}
