#include <gtest/gtest.h>
#include "SampleCollector.h"
#include "Perf.h"

class SampleCollectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (const PmuEvent& e : events){
            baseCollector.registerEvent(e);
        }
    }

    void TearDown() override {

    }

    std::vector<PmuEvent> events = {
            {"branches", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"branch-misses:u", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"instructions:uv", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"bpf-output", PmuEvent::Type::PERF_TYPE_SOFTWARE},
            {"duration_time", PmuEvent::Type::PERF_TYPE_SOFTWARE},
            {"L1-dcache-loads", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"conntext-switches", PmuEvent::Type::PERF_TYPE_SOFTWARE},
            {"cpu-clock", PmuEvent::Type::PERF_TYPE_SOFTWARE},
    };

    SampleCollector baseCollector;

};


TEST_F(SampleCollectorTest, defaultStatisticsTest){
    for (const PmuEvent& e : events){
        ASSERT_EQ(baseCollector.getEventStatistics(e), std::nullopt);
    }
}

TEST_F(SampleCollectorTest, meanUpdates){
    PmuEvent e = events[0];

    ASSERT_EQ(baseCollector.getEventStatistics(e), std::nullopt);

    Nanosecs time = msToNs(Millis(1));
    EventCount count = 50;
    baseCollector.pushSample(e, {count, time, time});
    ASSERT_EQ(baseCollector.getEventStatistics(e).value().meanCountsPerMillis, 50);

    time += msToNs(Millis(1));
    count += 100;
    baseCollector.pushSample(e, {count, time, time});
    ASSERT_EQ(baseCollector.getEventStatistics(e).value().meanCountsPerMillis, 75);

    time += msToNs(Millis(1));
    count += 0;
    baseCollector.pushSample(e, {count, time, time});
    ASSERT_EQ(baseCollector.getEventStatistics(e).value().meanCountsPerMillis, 50);

    time += msToNs(Millis(2));
    count += 100;
    baseCollector.pushSample(e, {count, time, time});
    ASSERT_EQ(baseCollector.getEventStatistics(e).value().meanCountsPerMillis, 50);
}

TEST_F(SampleCollectorTest, meanPerMillisTest){
    PmuEvent e = events[0];

    ASSERT_EQ(baseCollector.getEventStatistics(e), std::nullopt);

    Nanosecs time = msToNs(Millis(2));
    EventCount count = 100;
    baseCollector.pushSample(e, {count, time, time});
    ASSERT_EQ(baseCollector.getEventStatistics(e).value().meanCountsPerMillis, 50);

    time += msToNs(Millis(10));
    count += 100;
    baseCollector.pushSample(e, {count, time, time});
    ASSERT_EQ(baseCollector.getEventStatistics(e).value().meanCountsPerMillis, 30);
}


TEST_F(SampleCollectorTest, basicStatUpdatesTest){
    PmuEvent e = events[0];

    ASSERT_EQ(baseCollector.getEventStatistics(e), std::nullopt);

    Nanosecs time = msToNs(Millis(1));
    EventCount count = 50;
    for (int i = 1; i <= 10; i++){
        baseCollector.pushSample(e, {count, time, time});
        PmuEvent::Stats stats = baseCollector.getEventStatistics(e).value();
        ASSERT_EQ(stats.samples, i);
        ASSERT_EQ(stats.count, count);
        ASSERT_EQ(stats.timeEnabled, time);
        ASSERT_EQ(stats.propagations, 0);

        count += i * 123;
        time += Nanosecs(i * 12345);
    }
}

TEST_F(SampleCollectorTest, propagateStatTest){
    PmuEvent e1 = events[0], e2 = events[1];
    Nanosecs time = msToNs(Millis(1));
    EventCount count = 50;


    baseCollector.addRelationship(e1, e2, [&] (const PmuEvent::Stats& relatedEventStats,
                                              PmuEvent::Stats &eventToUpdate){
        ASSERT_EQ(relatedEventStats.meanCountsPerMillis, count);
        ASSERT_EQ(relatedEventStats.count, count);
        ASSERT_EQ(eventToUpdate.meanCountsPerMillis, 0);
        ASSERT_EQ(eventToUpdate.samples, 0);

        eventToUpdate.meanCountsPerMillis = relatedEventStats.meanCountsPerMillis + 5;
    });

    baseCollector.pushSample(e1, {count, time, time});

    Statistic e1Mean = baseCollector.getEventStatistics(e1).value().meanCountsPerMillis;
    ASSERT_EQ(e1Mean, count);
    PmuEvent::Stats e2Stats = baseCollector.getEventStatistics(e2).value();
    ASSERT_EQ(e2Stats.meanCountsPerMillis, e1Mean+5);
    ASSERT_EQ(e2Stats.samples, 0);
    ASSERT_EQ(e2Stats.propagations, 1);
}
