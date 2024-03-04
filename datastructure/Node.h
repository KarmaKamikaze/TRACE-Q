//
// Created by karma on 3/1/24.
//

#ifndef TRACE_Q_NODE_H
#define TRACE_Q_NODE_H

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
        Node<T>& find(T);
    };

} // data_structures

#endif //TRACE_Q_NODE_H
