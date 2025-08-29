#include <iostream>
#include <chrono>
#include <nlohmann/json.hpp>
#include <utils/logger.hpp>

using json = nlohmann::json;
using steady = std::chrono::steady_clock;

void printMessage(const json &j){
    static auto window_start = steady::now();
    static auto window_msgs = std::size_t{0};

    static auto last_hb = steady::time_point{};
    static auto hb_count = std::size_t{0};
    static auto tkr_count = std::size_t{0};

    ++window_msgs;
    const auto now = steady::now();
    auto secs = std::chrono::duration<double>(now-window_start).count(); 

    if (secs>5.0) {
        const double mps = window_msgs/secs;
        std::cout << "[STATS] " << mps << " msgs/sec ";
        if (last_hb.time_since_epoch().count() != 0){
            const double hb_age = std::chrono::duration<double>(now-last_hb).count();
            std::cout << "last_hb: " << hb_age << 's';
            if (hb_age > 10) std::cout << " (stale)";
        }
        std::cout << '\n';
        window_start = now;
        window_msgs = 0;
    }
    const auto type = j.value("type", "");
    if (type == "heartbeat"){
        last_hb = now;
        if (hb_count%10 == 0){
            std::cout << "[HB] "
                    << j.value("product_id", "???") << " "
                    << j.value("sequence", 0) << " "
                    << j.value("time", "") << '\n';
        }
    }
    else if (type == "ticker" && ++tkr_count%10 == 0){
        if (j.contains("price"))
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
