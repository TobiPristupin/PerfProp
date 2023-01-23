#ifndef PERFPROP_STATPRINTER_H
#define PERFPROP_STATPRINTER_H

#include "VariadicTable.h"
#include "PmuEvent.h"


class StatPrinter {
public:
    //columns: event name, meanCountPerMillis, varPerMillis, count, timeEnabled, samples, propagations
    using Table = VariadicTable<
            decltype(PmuEvent::name),
            Statistic,
            Statistic,
            EventCount,
            Nanosecs::rep,
            EventCount,
            EventCount
            >;

    std::vector<std::string> columns = {"Event", "meanCountPerMillis", "varPerMillis", "Count", "Time Enabled (ns)", "Samples", "Propagations"};

    StatPrinter() : table(columns) {};

    void addRow(const std::string& eventName, const PmuEvent::Stats& stats){
        table.addRow(eventName, stats.meanCountsPerMillis, stats.varianceCountPerMillis, stats.count, stats.timeEnabled.count(), stats.samples, stats.propagations);
    }

    void print(){
        table.print(std::cout);
    }

private:
    Table table;

};

#endif
