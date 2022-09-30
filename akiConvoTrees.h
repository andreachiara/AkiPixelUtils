#include <vector>
#include <string>

namespace akiConvoTrees {
    struct exchange {
        std::string sentence;
        std::vector<std::pair<std::string, exchange*>> replies;
    };

    struct conversation {
        std::vector<exchange> exchanges;
        exchange beginning;
    };
}


