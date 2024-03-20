#include <doctest/doctest.h>
#include <sstream>
#include "../src/data/Node.hpp"

class test_node {
public:
    data_structures::Node<int> node {1};
    test_node() {
        init_test_node();
    };
private:
    void init_test_node() {
        auto n1 = data_structures::Node<int> {2};
        auto n2 = data_structures::Node<int> {3};
        auto n3 = data_structures::Node<int> {4};

        n1.parent = std::make_shared<data_structures::Node<int>>(node);
        n2.parent = std::make_shared<data_structures::Node<int>>(n1);
        n3.parent = std::make_shared<data_structures::Node<int>>(n1);
        n1.children.emplace_back(n2);
        n1.children.emplace_back(n3);
        node.children.emplace_back(n1);
        node.children.emplace_back(5);
    }
};

TEST_CASE("Find method finds the Node with specified data if available or returns dummy Node") {
    auto tree = test_node{};
    SUBCASE("Finds deep Node with specified data when node is in tree.") {
        auto actual_node = tree.node.find(4);
        CHECK((actual_node.data == 4));
    }

    SUBCASE("Finds mid-ish Node with specified data when node is in tree.") {
        auto actual_node = tree.node.find(2);
        CHECK((actual_node.data == 2));
    }

    SUBCASE("Finds direct child Node with specified data when node is in tree.") {
        auto actual_node = tree.node.find(5);
        CHECK((actual_node.data == 5));
    }

    SUBCASE("Finds root Node with specified data when node is in tree.") {
        auto actual_node = tree.node.find(1);
        CHECK((actual_node.data == 1));
    }

    SUBCASE("Returns dummy node when node is not in tree.") {
        auto actual_node = tree.node.find(6);
        CHECK((actual_node.data == -1));
    }
}

TEST_CASE("operator<< prints correct tree structure with node data values") {
    auto tree = test_node{};
    SUBCASE("Prints tree nodes when given a populated tree") {
        auto os = std::ostringstream{};
        os << tree.node;

        auto expected_tree = std::string{"1{2{3 4 } 5 }"};

        CHECK(os.str() == expected_tree);
    }

    SUBCASE("Prints only one node if tree has only one node") {
        auto node = data_structures::Node<int>{1};

        auto os = std::ostringstream{};
        os << node;

        auto expected_tree = std::string{"1"};

        CHECK(os.str() == expected_tree);
    }
}