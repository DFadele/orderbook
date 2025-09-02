#include <orderbook/orderbook.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

double OrderBook::to_double(const std::string &s){
    return std::stod(s);
}

template <typename MapT>
int OrderBook::setLevel(MapT &book, double price, double size){
    // 0 = insert, 1 = modify, 2=erase, 3 = no-op
    if (size == 0.0) {
        auto it = book.find(price);
        if (it != book.end()) {book.erase(it); return 2;}
        return 3;
    } else {
        auto [it, inserted] = book.insert_or_assign(price, size);
        return inserted? 0:1;
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
    metrics_.snapshots ++;
}

void OrderBook::applyL2Update(const nlohmann::json &j) {
    metrics_.updates++;
    for (const auto &ch: j["changes"]) {
        const auto side = ch.at(0).get<std::string>();
        double price = to_double(ch.at(1).get<std::string>());
        double size = to_double(ch.at(2).get<std::string>());
        
        int kind = (side=="buy")? 
                    setLevel(bids_, price, size):setLevel(asks_, price, size);

        metrics_.changes++;
        if (kind==0) metrics_.inserts ++;
        else if (kind==1) metrics_.modifies++;
        else if (kind==2) metrics_.erases++;
    }
}

std::pair<double, double> OrderBook::topOfBook() const {
    const double bestBid = bids_.empty() ? 0.0: bids_.begin()->first;
    const double bestAsk = asks_.empty() ? 0.0:asks_.begin()->first;
    return {bestBid, bestAsk};
}

void OrderBook::printTOB() const {
    auto [bb, ba] = topOfBook();
    double spread = ba - bb;
    double mid = (ba+bb) /2.0;
    std::cout << "[TOB] bid " << bb 
              << "  ask " << ba
              << " spread " << spread
              << " mid " << mid << '\n';
    if (bb >= ba) std::cerr << "WARNING: crossed book \n";
}