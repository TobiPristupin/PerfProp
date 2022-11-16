#include <gtest/gtest.h>
#include "FactorGraph.h"


class FactorGraphTest : public ::testing::Test {
protected:

    //factor graph corresponding to fig. 1 pg. 1746 in https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf
    FactorGraph  fig1Graph;
    PmuEvent x1 = {"X1", PmuEvent::Type::HARDWARE};
    PmuEvent x2 = {"X2", PmuEvent::Type::HARDWARE};
    PmuEvent x3 = {"X3", PmuEvent::Type::HARDWARE};
    PmuEvent x4 = {"X4", PmuEvent::Type::HARDWARE};
    PmuEvent x5 = {"X5", PmuEvent::Type::HARDWARE};
    PmuEvent x6 = {"X6", PmuEvent::Type::HARDWARE};
    PmuEvent x7 = {"X7", PmuEvent::Type::HARDWARE};
    PmuEvent x8 = {"X8", PmuEvent::Type::HARDWARE};
    PmuEvent x9 = {"X9", PmuEvent::Type::HARDWARE};
    std::vector<PmuEvent> fig1Events = {x1, x2, x3, x4, x5, x6, x7, x8, x9};

    void SetUp() override {
        fig1Graph = generateFig1ExampleGraph();
    }

    FactorGraph generateFig1ExampleGraph(){
        ProbabilityFunction dummyFunc = [](auto p) -> double {return 0;};
        std::vector<ProbabilityNode> factors = {
                {{x1, x2, x3}, dummyFunc},

                {{x1, x2},                   dummyFunc},
                {{x2, x3},                   dummyFunc},

                {{x1, x4},                   dummyFunc},
                {{x2, x5},                   dummyFunc},
                {{x3, x6},                   dummyFunc},

                {{x4, x5},                   dummyFunc},
                {{x5, x6},                   dummyFunc},

                {{x4, x7},                   dummyFunc},
                {{x5, x8},                   dummyFunc},
                {{x6, x9},                   dummyFunc},

                {{x7, x8},                   dummyFunc},
                {{x8, x9},                   dummyFunc},
        };

        return generateGraph(fig1Events, factors);
    }

};

bool testMarkovBlanket(std::initializer_list<PmuEvent> eventsForBlanket, const std::unordered_set<std::string>& expectedBlanket, const FactorGraph &graph){
    std::unordered_set<std::string> obtained;
    for (const PmuEvent &n : markovBlanket(eventsForBlanket, graph)){
        obtained.insert(n.name);
    }
    return obtained == expectedBlanket;
}

TEST_F(FactorGraphTest, testMarkovBlanket){
    EXPECT_EQ(markovBlanket({x1}, fig1Graph), std::unordered_set<PmuEvent>({x2, x3, x4}));
    EXPECT_EQ(markovBlanket({x1, x2}, fig1Graph), std::unordered_set<PmuEvent>({x3, x4, x5}));
    EXPECT_EQ(markovBlanket({x5}, fig1Graph), std::unordered_set<PmuEvent>({x2, x4, x6, x8}));
}

