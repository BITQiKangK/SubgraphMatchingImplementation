#include "order.h"

#include <string.h>
#include <iostream>

const char* ORDER_TYPE_NAME[] = {"UNDEFINED", "Plain", "GQL"};

const char* order_type_to_string(OrderType order_type) {
    if (order_type > OrderType::UNDEFINED && order_type < OrderType::MAXTYPE) {
        return ORDER_TYPE_NAME[static_cast<int>(order_type)];
    }
    return "UNKNOWN";
}

OrderType order_type_from_string(const char* order_type_name) {
    for (unsigned int i = 1; i < sizeof(ORDER_TYPE_NAME) / sizeof(ORDER_TYPE_NAME[0]); i++) {
        if (0 == strcmp(ORDER_TYPE_NAME[i], order_type_name)) {
            return (OrderType)i;
        }
    }
    return OrderType::UNDEFINED;
}

void Order::order(const Graph& data_graph, const Graph& query_graph, const char* order_type_name, const std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& order, std::vector<VertexID>& pivot) {
    OrderType order_type = order_type_from_string(order_type_name);
    switch(order_type) {
        case OrderType::Plain:
            PlainOrder(query_graph, order);
            break;
        case OrderType::GQL:
            GQLOrder(data_graph, query_graph, candidates, order, pivot);
            break;
        default:
            // Handle unknown order type
            std::cerr << "Unknown order type: " << order_type_name << std::endl;
    }

}


void Order::PlainOrder(const Graph& query_graph, std::vector<VertexID>& order) {
    order.resize(query_graph.get_vertices_count());
    for (int i = 0; i < order.size(); i++) {
        order[i] = i;
    }
}

void Order::GQLOrder(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& order, std::vector<VertexID>& pivot) {
    /**
      * Select the vertex v such that (1) v is adjacent to the selected vertices; and (2) v has the minimum number of candidates.
      */
    bool visited_vertices[query_graph.get_vertices_count()] = {};
    bool adjacent_vertices[query_graph.get_vertices_count()] = {};
    order.resize(query_graph.get_vertices_count());
    pivot.resize(query_graph.get_vertices_count());

    VertexID start_vertex_id = OrderStart::select_GQL_start_vertex(query_graph, candidates);
    order[0] = start_vertex_id;
    update_valid_vertices(query_graph, start_vertex_id, visited_vertices, adjacent_vertices);

    for (VertexID i = 1; i < query_graph.get_vertices_count(); i++) {
        VertexID next_vertex_id;
        ui min_value = data_graph.get_vertices_count() + 1;
        for (ui j = 0; j < query_graph.get_vertices_count(); j++) {
             VertexID cur_vertex_id = j;

             if (!visited_vertices[cur_vertex_id] && adjacent_vertices[cur_vertex_id]) {
                  if (candidates[cur_vertex_id].size() < min_value) {
                       min_value = candidates[cur_vertex_id].size();
                       next_vertex_id = cur_vertex_id;
                  } else if (candidates[cur_vertex_id].size() == min_value &&
                             query_graph.get_vertex_degree(cur_vertex_id) > query_graph.get_vertex_degree(next_vertex_id)) {
                        next_vertex_id = cur_vertex_id;
                  }
             }
        }
        update_valid_vertices(query_graph, next_vertex_id, visited_vertices, adjacent_vertices);
        order[i] = next_vertex_id;
   }

   // Pick a pivot randomly.
   for (ui i = 1; i < query_graph.get_vertices_count(); i++) {
       VertexID u = order[i];
       for (ui j = 0; j < i; ++j) {
           VertexID cur_vertex_id = order[j];
           if (query_graph.check_edge_existence(u, cur_vertex_id)) {
               pivot[i] = cur_vertex_id;
               break;
           }
       }
   }

}


/*
 * Below are Order helper functions.
 */

void Order::update_valid_vertices(const Graph& query_graph, VertexID query_vertex_id, bool visited[], bool adjacent[]) {
    visited[query_vertex_id] = true;
    const auto& nbrs = query_graph.get_vertex_neighbors(query_vertex_id);

    for (auto nbr : nbrs) {
        adjacent[nbr] = true;
    }
}


/*
 * Below are OrderStart functions.
 */

 VertexID OrderStart::select_GQL_start_vertex(const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates) {
    /**
     * Select the vertex with the minimum number of candidates as the start vertex.
     */

     VertexID start_vertex_id = 0;

     for (VertexID i = 1; i < query_graph.get_vertices_count(); i++) {
          VertexID cur_vertex_id = i;

          if (candidates[cur_vertex_id].size() < candidates[start_vertex_id].size()) {
               start_vertex_id = cur_vertex_id;
          } else if (candidates[cur_vertex_id].size() == candidates[start_vertex_id].size()
                   && query_graph.get_vertex_degree(cur_vertex_id) > query_graph.get_vertex_degree(start_vertex_id)) {
               start_vertex_id = cur_vertex_id;
          }
     }

     return start_vertex_id;
 }