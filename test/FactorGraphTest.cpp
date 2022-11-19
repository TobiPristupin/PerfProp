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
    std::vector<PmuEvent> allEvents = {x1, x2, x3, x4, x5, x6, x7, x8, x9};

    FactorGraph disconnectedGraph;

    void SetUp() override {
        fig1Graph = generateFig1ExampleGraph();
        disconnectedGraph = generateDisconnectedGraph();
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

        return generateGraph(allEvents, factors);
    }

    FactorGraph generateDisconnectedGraph(){
        ProbabilityFunction dummyFunc = [](auto p) -> double {return 0;};
        std::vector<ProbabilityNode> factors = {
                {{x1, x2, x3}, dummyFunc},

                {{x1, x2},                   dummyFunc},
                {{x2, x3},                   dummyFunc},

//                {{x1, x4},                   dummyFunc},
//                {{x2, x5},                   dummyFunc},
//                {{x3, x6},                   dummyFunc},

                {{x4, x5},                   dummyFunc},
                {{x5, x6},                   dummyFunc},

                {{x4, x7},                   dummyFunc},
                {{x5, x8},                   dummyFunc},
                {{x6, x9},                   dummyFunc},

                {{x7, x8},                   dummyFunc},
                {{x8, x9},                   dummyFunc},
        };

        return generateGraph(allEvents, factors);
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
    EXPECT_EQ(markovBlanket({x4, x5, x6}, fig1Graph), std::unordered_set<PmuEvent>({x1, x2, x3, x7, x8, x9}));
}

bool testGrouping(const std::vector<std::unordered_set<PmuEvent>> &groups, const std::unordered_set<PmuEvent> &events, const FactorGraph &graph){
    for (int i = 0; i < groups.size() - 1; i++){
        std::unordered_set<PmuEvent> intersection = statisticalDependenceSet(groups[i], groups[i+1], graph);
        EXPECT_TRUE(!intersection.empty());
    }

    for (const PmuEvent &event : events){
        bool found = false;
        for (const auto & group : groups){
            if (group.find(event) != group.end()){
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found);
    }
}

TEST_F(FactorGraphTest, testGrouping){
    std::unordered_set<PmuEvent> events(allEvents.begin(), allEvents.end());

    auto groups = performGrouping(events, 3, 1, fig1Graph);
    testGrouping(groups, events, fig1Graph);
    groups = performGrouping(events, 3, 2, fig1Graph);
    testGrouping(groups, events, fig1Graph);
    groups = performGrouping(events, 2, 1, fig1Graph);
    testGrouping(groups, events, fig1Graph);

    groups = performGrouping(events, 3, 1, disconnectedGraph);
//    testGrouping(groups, events, disconnectedGraph);


    for (const std::unordered_set<PmuEvent> &group : groups){
        for (const PmuEvent &event : group){
            std::cout << event.name << " ";
        }
        std::cout << "\t blanket: ";
        for (const PmuEvent &event : markovBlanket(group, disconnectedGraph)){
            std::cout << event.name << " ";
        }
        std::cout << "\n";
    }


}

