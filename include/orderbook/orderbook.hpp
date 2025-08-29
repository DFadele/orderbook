#pragma once
#include <map>
#include <string>
#include <utility>
#include <functional>
#include <nlohmann/json.hpp>

class OrderBook {

    public:
        OrderBook() = default;

        void applySnapshot(const nlohmann::json &j);
        void applyL2Update(const nlohmann::json &j);

        std::pair<double, double> topOfBook() const;
        void printTOB() const;

    private:
        std::map<double, double, std::greater<>> bids_;
        std::map<double, double> asks_;

        static double to_double(const std::string &s);
        static void setLevel(std::map<double, double> &book, double price, double size);
};