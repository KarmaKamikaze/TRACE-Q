//
// Created by karma on 3/1/24.
//

#include "Node.h"

namespace data_structures {

    template<typename T>
    Node<T>& Node<T>::find(T node_data) {
        if (this->data == node_data) {
            return *this;
        }

        for (const auto& child : children) {
            auto& found_node = child.find(node_data);
            if (found_node.data == node_data) {
                return found_node;
            }
        }

        throw std::runtime_error("Node not found.");
    }
} // data_structures