#ifndef GRAPH_H
#define GRAPH_H

#include <iterator>
#include <vector>

template <typename NodeT>
class Graph {
 public:
  Graph();

  void AddEdge(NodeT node);

  int Edges();
  int Nodes();

  typedef std::iterator<std::forward_iterator_tag, NodeT> iterator;

  iterator adj(const NodeT& node);

 private:
};

#endif // GRAPH_H
