#include <vector>
#include <string>

namespace akiConvoTrees {
    struct exchange {
        int id;
        std::string sentence;
        std::vector<std::pair<std::string, int>> replies;
    };

    struct conversation {
        std::vector<exchange> exchanges;
        exchange beginning;
    };
}

namespace akiExampleConvo {

    akiConvoTrees::exchange ex1{
        1,
        "First Sentence",
        {{"R1", 2}, {"R2", 3}}
    };

    akiConvoTrees::exchange ex2{
        1,
        "First Sentence",
        {{"R1", 2}, {"R2", 3}}
    };

    akiConvoTrees::exchange ex3{
        1,
        "First Sentence",
        {{"R1", 2}, {"R2", 3}}
    };

    akiConvoTrees::conversation example{
        {ex1, ex2, ex3},
        ex1
    };

}


