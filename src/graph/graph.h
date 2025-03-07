#pragma once

#include "common/config.h"
#include "common/vertex.h"

#include <unordered_map>
#include <algorithm>

/**
 * A graph is stored as the CSR format.
 */

class Graph {
public:
    Graph() {
        edges_count_ = 0;
        labels_count_ = 0;
        max_degree_ = 0;
        max_label_frequency_ = 0;
    };

public:
    // getters and setters
    const ui get_labels_count() const { return labels_count_; }

    const ui get_vertices_count() const { return vertices_.size(); }

    const ui get_edges_count() const { return edges_count_; }

    const ui get_max_degree() const { return max_degree_; }

    const ui get_max_label_frequency() const { return max_label_frequency_; }

    const ui get_vertex_degree(const VertexID vertex_id) const { return vertices_[vertex_id].neighbors_id.size(); }

    const ui get_labels_frequency(const VertexLabel vertex_label) const {
        return labels_frequency_.count(vertex_label) ? labels_frequency_.at(vertex_label) : 0;
    }

    const VertexLabel get_vertex_label(const VertexID id) const { return vertices_[id].label; }

    const std::vector<VertexID>& get_vertex_neighbors(const VertexID vertex_id) const { return vertices_[vertex_id].neighbors_id; }

    const std::vector<VertexID>& get_vertices_by_label(const VertexLabel vertex_label) const { return reverse_index_[vertex_label]; }

#if OPTIMIZED_LABELED_GRAPH == 1
    const std::unordered_map<VertexLabel, ui>& get_vertex_neighbors_label2count_map(const VertexID vertex_id) const { return neighbors_label_count_map_[vertex_id]; }

    const ui get_vertex_neighbors_label2count(const VertexID vertex_id, const VertexLabel vertex_label) const {
        return neighbors_label_count_map_[vertex_id].count(vertex_label) ? neighbors_label_count_map_[vertex_id].at(vertex_label) : 0;
    }
#endif

    bool check_edge_existence(VertexID src_id, VertexID dst_id) const {
        if (get_vertex_degree(src_id) < get_vertex_degree(dst_id)) {
            std::swap(src_id, dst_id);
        }
        const auto& neighbors = vertices_[src_id].neighbors_id;
        return std::binary_search(neighbors.begin(), neighbors.end(), dst_id);
    }

public:
    // other functions
    void load_graph_from_file(const std::string &input_file);

    void print_graph_meta_data() const;

private:
    void build_reverse_index_();

#if OPTIMIZED_LABELED_GRAPH == 1
    void build_nlf_();
#endif

private:
    std::vector<Vertex> vertices_;
    ui edges_count_;
    ui labels_count_;
    ui max_degree_;
    ui max_label_frequency_;
    std::unordered_map<VertexLabel, ui> labels_frequency_;
    std::vector<std::vector<VertexID>> reverse_index_;

#if OPTIMIZED_LABELED_GRAPH == 1
    std::vector<std::unordered_map<VertexLabel, ui>> neighbors_label_count_map_;
#endif
};