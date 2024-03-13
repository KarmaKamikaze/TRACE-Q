#pragma once
#ifndef TRACE_Q_NODE_HPP
#define TRACE_Q_NODE_HPP

#include <vector>
#include <memory>

namespace data_structures {

    template<typename T>
    class Node {
    public:
        T data {};
        std::shared_ptr<Node<T>> parent {};
        std::vector<Node<T>> children {};

        explicit Node(T data) : data(data) {};

        /**
         * Finds a node with the specified data value.
         * @param node_data The data value to search for.
         * @return A reference to the found node or a dummy node if not found.
         */
        Node<T>& find(const T& node_data) {
            static auto dummy = Node{T{}};

            if (this->data == node_data) {
                return *this; // Found the node
            }

            for (auto& child : children) {
                auto& found_node = child.find(node_data);
                if (found_node.data == node_data) {
                    return found_node; // Found the node in a child
                }
            }

            return dummy; // Data not found
        }

        /**
         * Overloads the << operator to print the node and its children.
         * @param os The output stream.
         * @param node The node to print.
         * @return The modified output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Node& node) {
            os << node.data;
            if (!node.children.empty()) {
                os << "{";
            }
            for (const auto& x : node.children) {
                os << x << " "; // Recursively print child nodes
            }
            if (!node.children.empty()) {
                os << "}";
            }

            return os;
        }

    };

} // data_structures

#endif //TRACE_Q_NODE_HPP
