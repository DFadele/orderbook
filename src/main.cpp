#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <utils/logger.hpp>
#include <orderbook/orderbook.hpp>

using json = nlohmann::json;


int main(int argc, char *argv[]) {
    std::string product(argc > 1 ? argv[1] : "BTC-USD");
    OrderBook book;
    ix::WebSocket ws;
    auto url = std::string{"wss://ws-feed.exchange.coinbase.com"};
    ws.setUrl(url);
    ws.setPingInterval(30);
 
    ws.setOnMessageCallback([&ws, &product, &book](const ix::WebSocketMessagePtr &msg){

        if (msg->type == ix::WebSocketMessageType::Open){
            std::cout << "Connected \n";
            std::cout << "Press Enter to quit...\n \n";

            auto sub = json{
                {"type", "subscribe"},
                {"product_ids", {product}},
                {"channels", {"level2_batch"}},
            };
            ws.send(sub.dump());
        }
        else if (msg->type == ix::WebSocketMessageType::Close){
            std::cout << "Disconnected" << '\n'
            << msg->closeInfo.code << '\n'
            << msg->closeInfo.reason << '\n';
        }
        else if (msg->type == ix::WebSocketMessageType::Message){
            try
            {
                const auto j = json::parse(msg->str);
                const auto type = j.value("type", "");

                if (type == "snapshot") {
                    book.applySnapshot(j);
                    book.printTOB();
                }
                else if (type == "l2update") {
                    book.applyL2Update(j);
                    book.printTOB();
                }
                else
                printMessage(j);
            }
            catch(const std::exception& e)
            {
                std::cerr << "JSON parse error: " << e.what() << '\n';
            }
            
        }
        else if (msg->type == ix::WebSocketMessageType::Error){
            std::cerr << "WS Error: " << msg->errorInfo.reason << '\n';
        }
    });

    ws.start();
    std::string _;
    std::getline(std::cin, _);
    ws.stop();
    return 0;
}