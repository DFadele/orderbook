#include <orderbook/orderbook.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

double OrderBook::to_double(const std::string &s){
    return std::stod(s);
}

template <typename MapT>
void OrderBook::setLevel(MapT &book, double price, double size){
    if (size == 0.0) {
        auto it = book.find(price);
        if (it != book.end()) book.erase(it);
    } else {
        book[price] = size;
    }
}

void OrderBook::applySnapshot(const nlohmann::json &j) {
    bids_.clear(); asks_.clear();

    for (const auto &row: j["bids"]) {
        double p = to_double(row.at(0).get<std::string>()); //price
        double s = to_double(row.at(1).get<std::string>()); //size
        OrderBook::setLevel(bids_,p,s);
    }
    for (const auto &row: j["asks"]) {
        double p = to_double(row.at(0).get<std::string>());
        double s = to_double(row.at(1).get<std::string>());
        OrderBook::setLevel(asks_, p, s);
    }
}

void OrderBook::applyL2Update(const nlohmann::json &j) {
    for (const auto &ch: j["changes"]) {
        const auto side = ch.at(0).get<std::string>();
        double price = to_double(ch.at(1).get<std::string>());
        double size = to_double(ch.at(2).get<std::string>());
        if (side == "buy") setLevel(bids_, price, size);
        else setLevel(asks_, price, size);
    }
}

std::pair<double, double> OrderBook::topOfBook() const {
    const double bestBid = bids_.empty() ? 0.0: bids_.begin()->first;
    const double bestAsk = asks_.empty() ? 0.0:asks_.begin()->first;
    return {bestBid, bestAsk};
}

void OrderBook::printTOB() const {
    auto [bb, ba] = topOfBook();
    std::cout << "[TOB] bid " << bb << "  ask " << ba << '\n';
}