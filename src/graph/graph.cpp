#include "graph.h"

#include <iostream>
#include <fstream>
#include <algorithm>

void Graph::build_reverse_index_() {
    reverse_index_.clear();
    reverse_index_.resize(labels_count_);
    for (VertexID vertex_id = 0; vertex_id < vertices_.size(); vertex_id++) {
        reverse_index_[vertices_[vertex_id].label].push_back(vertex_id);
    }
}

void Graph::build_nlf_() {
    neighbors_label_count_map_.clear();
    neighbors_label_count_map_.resize(vertices_.size());
    for (VertexID vertex_id = 0; vertex_id < vertices_.size(); vertex_id++) {
        for (VertexID neighbor_id : vertices_[vertex_id].neighbors_id) {
            if (neighbors_label_count_map_[vertex_id].count(vertices_[neighbor_id].label) == 0) {
                neighbors_label_count_map_[vertex_id][vertices_[neighbor_id].label] = 1;
            } else {
                neighbors_label_count_map_[vertex_id][vertices_[neighbor_id].label]++;
            }
        }
    }
}

void Graph::load_graph_from_file(const std::string &input_file) {
    std::ifstream in_file(input_file);
    if (!in_file.is_open()) {
        std::cerr << "Error: cannot open the input file: " << input_file << std::endl;
        exit(EXIT_FAILURE);
    }

    char type;
    ui vertices_count;
    in_file >> type >> vertices_count >> edges_count_;
    vertices_.resize(vertices_count);
    labels_count_ = 0;
    max_degree_ = 0;

    ui max_label_id = 0;

    while (in_file >> type) {
        if (type == 'v') {
            // read vertex
            VertexID id;
            VertexLabel label;
            ui degree;
            in_file >> id >> label >> degree;

            vertices_[id].label = label;
            if (degree > max_degree_) {
                max_degree_ = degree;
            }

            if (!labels_frequency_.count(label)) {
                labels_frequency_[label] = 1;
                if (label > max_label_id) {
                    max_label_id = label;
                }
            } else {
                labels_frequency_[label]++;
            }

        } else if (type == 'e') {
            // read edge
            VertexID src_id, dst_id;
            in_file >> src_id >> dst_id;
            vertices_[src_id].neighbors_id.push_back(dst_id);
            vertices_[dst_id].neighbors_id.push_back(src_id);
        }
    }

    in_file.close();
    labels_count_ = (ui)labels_frequency_.size() > (max_label_id + 1) ? (ui)labels_frequency_.size() : max_label_id + 1;

    for (auto& element : labels_frequency_) {
        if (element.second > max_label_frequency_) {
            max_label_frequency_ = element.second;
        }
    }

    for (auto& vertex : vertices_) {
        std::sort(vertex.neighbors_id.begin(), vertex.neighbors_id.end());
    }

    build_reverse_index_();

#if OPTIMIZED_LABELED_GRAPH == 1
    build_nlf_();
#endif
}

void Graph::print_graph_meta_data() const {
    std::cout << "|V|: " << vertices_.size() << ", |E|: " << edges_count_ << ", |\u03A3|: " << labels_count_ << std::endl;
    std::cout << "Max Degree: " << max_degree_ << ", Max Label Frequency: " << max_label_frequency_ << std::endl;
}