#ifndef MAZE_SOLVER_STATSAGGREGATOR_H
#define MAZE_SOLVER_STATSAGGREGATOR_H

#include "Exceptions.h"
#include <algorithm>
#include <cstddef>
#include <numeric>
#include <type_traits>
#include <vector>



template <typename T>
class StatsAggregator {
    static_assert(std::is_arithmetic<T>::value, "StatsAggregator requires arithmetic types");

public:
    void addSample(const T& value) {
        samples_.push_back(value);
    }

    std::size_t count() const {
        return samples_.size();
    }

    T min() const {
        ensureSamples("minimum");
        return *std::min_element(samples_.begin(), samples_.end());
    }

    T max() const {
        ensureSamples("maximum");
        return *std::max_element(samples_.begin(), samples_.end());
    }

    double average() const {
        ensureSamples("average");
        double total = std::accumulate(samples_.begin(), samples_.end(), 0.0,
                                       [](double acc, const T& value) { return acc + static_cast<double>(value); });
        return total / static_cast<double>(samples_.size());
    }

    const std::vector<T>& values() const {
        return samples_;
    }

private:
    void ensureSamples(const char* operation) const {
        if (samples_.empty()) {
            throw AnalysisException(std::string("Cannot compute ") + operation + " without samples");
        }
    }

    std::vector<T> samples_;
};

#endif
