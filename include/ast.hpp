#pragma once
#include <string>
#include <vector>

namespace deadbeat {
    struct Node {
        virtual ~Node() = default;
    };

    struct Note : public Node {
        std::string name;
        double frequency;
        double amplitude;
        double duration;
    };

    struct Block : public Node {
        std::vector<Node*> children;
    };
}
