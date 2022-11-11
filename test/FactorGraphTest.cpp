#include <gtest/gtest.h>
#include "FactorGraph.h"


class FactorGraphTest : public ::testing::Test {
protected:

    //factor graph corresponding to fig. 1 pg. 1746 in https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf
    FactorGraph  figure1Graph;
    std::vector<PmuEvent> figure1Events;

    void SetUp() override {
        figure1Graph = figure1ExampleGraph();
    }

    FactorGraph figure1ExampleGraph(){
        figure1Events = {
                {"X1", PmuEvent::Type::HARDWARE}, {"X2", PmuEvent::Type::HARDWARE}, {"X3", PmuEvent::Type::HARDWARE},
                {"X4", PmuEvent::Type::HARDWARE}, {"X5", PmuEvent::Type::HARDWARE}, {"X6", PmuEvent::Type::HARDWARE},
                {"X7", PmuEvent::Type::HARDWARE}, {"X8", PmuEvent::Type::HARDWARE}, {"X9", PmuEvent::Type::HARDWARE},
        };

        ProbabilityFunction dummyFunc = [](auto p) -> double {return 0;};
        std::vector<ProbabilityNode> factors = {
                {{figure1Events[0], figure1Events[1], figure1Events[2]}, dummyFunc},

                {{figure1Events[0], figure1Events[1]},                   dummyFunc},
                {{figure1Events[1], figure1Events[2]},                   dummyFunc},

                {{figure1Events[0], figure1Events[3]},                   dummyFunc},
                {{figure1Events[1], figure1Events[4]},                   dummyFunc},
                {{figure1Events[2], figure1Events[5]},                   dummyFunc},

                {{figure1Events[3], figure1Events[4]},                   dummyFunc},
                {{figure1Events[4], figure1Events[5]},                   dummyFunc},

                {{figure1Events[3], figure1Events[6]},                   dummyFunc},
                {{figure1Events[4], figure1Events[7]},                   dummyFunc},
                {{figure1Events[5], figure1Events[8]},                   dummyFunc},

                {{figure1Events[6], figure1Events[7]},                   dummyFunc},
                {{figure1Events[7], figure1Events[8]},                   dummyFunc},
        };

        FactorGraph  f = generateGraph(figure1Events, factors);
        PmuEvent e = {"X1", PmuEvent::Type::HARDWARE};
        Node n(&figure1Events[0]);
        std::initializer_list<Node> s = {n};
        markovBlanket(s, figure1Graph);
        return  f;
    }

};

TEST_F(FactorGraphTest, testMarkovBlanket){
   std::unordered_set<std::string> obtained;

//   for (const Node &n : markovBlanket({Node(&figure1Events[0])}, figure1Graph)){
//       obtained.insert(n.getEvent()->name);
//   }
//   std::unordered_set<std::string> expected = {"X2", "X3", "X4"};
//   EXPECT_EQ(obtained, expected);
}