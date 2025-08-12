#include <iostream>
#include <string>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct ConnData {
    std::string product;
    std::size_t count;
};

int main() {
    ix::WebSocket ws;
    auto url = std::string{"wss://ws-feed-public.sandbox.exchange.coinbase.com"};
    ws.setUrl(url);
    ws.setPingInterval(30);
 
    ws.setOnMessageCallback([&ws](const ix::WebSocketMessagePtr &msg){
        if (msg->type == ix::WebSocketMessageType::Open){
            std::cout << "Connected \n";
            std::cout << "Press Enter to quit...\n \n";

            auto sub = json{
                {"type", "subscribe"},
                {"product_ids", {"BTC-USD"}},
                {"channels", {"heartbeat", "ticker"}},
            };
            ws.send(sub.dump());
        }
        else if (msg->type == ix::WebSocketMessageType::Close){
            std::cout << "Disconnected" << '\n'
            << msg->closeInfo.code << '\n'
            << msg->closeInfo.reason << '\n';
        }
        else if (msg->type == ix::WebSocketMessageType::Message){
            static auto count = std::size_t{0};
            try
            {
                const auto j = json::parse(msg->str);
                const auto type = j.value("type", "");
                if (type == "heartbeat" && ++count %10 == 0){
                    std::cout << "[HB] "
                              << j.value("product_id", "???") << " "
                              << j.value("sequence", 0) << " "
                              << j.value("time", "") << '\n';
                }
                else if (type == "ticker"){
                    std::cout << "[TKR] "
                              << j.value("price", "???") << " "
                              << j.value("best_bid", "???") << " "
                              << j.value("best_ask", "???") << " "
                              << j.value("side", "???") << " "
                              << j.value("time", "") << '\n';
                }
                else {
                    /**/
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << "JSON parse error: " << e.what() << '\n';
            }
            
        }
        else if (msg->type == ix::WebSocketMessageType::Error){
            std::cerr << "WS Error" << msg->errorInfo.reason << '\n';
        }
    });

    ws.start();
    std::string _;
    std::getline(std::cin, _);
    ws.stop();
    return 0;
}