#include <orderbook/orderbook.hpp>
#include <iostream>

double OrderBook::to_double(const std::string &s){
    return std::stod(s);
}

void OrderBook::setLevel(std::map<double, double> &book, double price, double size){
    if (size == 0.0) {
        auto it = book.find(price);
        if (it != book.end()) book.erase(it);
    } else {
        book[price] = size;
    }
}

