#pragma once

#include "graph/graph.h"

enum class OrderType {
    UNDEFINED,
    Plain,
    GQL,
    MAXTYPE,
};

const char* order_type_to_string(OrderType order_type);
OrderType order_type_from_string(const char* order_type_name);

class Order {
public:
    static void order(const Graph& data_graph, const Graph& query_graph, const char* filter_type_name, const std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& order, std::vector<VertexID>& pivot);

private:
    static void PlainOrder(const Graph& query_graph, std::vector<VertexID>& order);
    static void GQLOrder(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& order, std::vector<VertexID>& pivot);

private:
    static void update_valid_vertices(const Graph& query_graph, VertexID query_vertex_id, bool visited[], bool adjacent[]);
};

class OrderStart {
private:
    static VertexID select_GQL_start_vertex(const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates);

friend class Order;
};