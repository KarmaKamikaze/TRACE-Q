#pragma once
#ifndef TRACE_Q_NODE_HPP
#define TRACE_Q_NODE_HPP

#include <vector>
#include <memory>
#include <optional>

namespace data_structures {

    template<typename T>
    class Node {
    public:
        T data {};
        std::shared_ptr<Node<T>> parent {};
        std::vector<Node<T>> children {};

        explicit Node(T data) : data(data) {};
        Node<T>& find(const T& node_data) {
            static auto dummy = Node{T{}};

            if (this->data == node_data) {
                return *this;
            }

            for (auto& child : children) {
                auto& found_node = child.find(node_data);
                if (found_node.data == node_data) {
                    return found_node;
                }
            }

            return dummy;
        }

        friend std::ostream& operator<<(std::ostream& os, const Node& node) {
            os << node.data;
            if (!node.children.empty()) {
                os << "{";
            }
            for (const auto& x : node.children) {
                os << x;
            }
            if (!node.children.empty()) {
                os << "}";
            }

            return os;
        }

    };

} // data_structures

#endif //TRACE_Q_NODE_HPP
