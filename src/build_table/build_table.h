#pragma once

#include "graph/graph.h"
#include "common/aux.h"

#include <vector>

class BuildTable {
public:
    static void build_table(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>> candidates, Table& table);
};