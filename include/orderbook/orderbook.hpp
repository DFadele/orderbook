#pragma once
#include <map>
#include <string>
#include <utility>
#include <functional>
#include <nlohmann/json.hpp>

class OrderBook {

    public:

        struct Metrics {
            uint64_t snapshots = 0u;
            uint64_t updates = 0u;
            uint64_t changes = 0u;
            uint64_t inserts = 0u;
            uint64_t modifies = 0u;
            uint64_t erases = 0u;
        };

        OrderBook() = default;

        void applySnapshot(const nlohmann::json &j);
        void applyL2Update(const nlohmann::json &j);

        std::pair<double, double> topOfBook() const;
        void printTOB() const;

        const Metrics& metrics() const { return metrics_; }

    private:
        std::map<double, double, std::greater<>> bids_;
        std::map<double, double> asks_;

        Metrics metrics_;

        static double to_double(const std::string &s);

        template <typename MapT>
        static int setLevel(MapT &book, double price, double size);
};