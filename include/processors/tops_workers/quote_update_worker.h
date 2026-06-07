#pragma once
#include <bits/stdc++.h>
#include "persistence/quote_update_table_builder.h"
#include "utils/endian_utils.h"

class QuoteUpdateWorker {
public:
    explicit QuoteUpdateWorker(int shard_idx)
        : builder_(ids_to_symbols_, "data_quotes_" + std::to_string(shard_idx) + ".parquet") {}

    void Process(const QuoteUpdateMsg& msg) {
        std::string symbol = Int64ToTicker(msg.raw_symbol);
        uint16_t symbol_id;
        auto it = symbols_to_ids_.find(symbol);
        if (it != symbols_to_ids_.end()) {
            symbol_id = it->second;
        } else {
            if (ids_to_symbols_.size() >= std::numeric_limits<uint16_t>::max())
                throw std::runtime_error("symbol dictionary overflow");
            symbol_id = static_cast<uint16_t>(ids_to_symbols_.size());
            ids_to_symbols_.push_back(symbol);
            symbols_to_ids_[symbol] = symbol_id;
        }
        builder_.AddRow({msg.timestamp, symbol_id, msg.bid_size, msg.bid_price, msg.ask_price, msg.ask_size});
    }

    void Close() { builder_.Close(); }

private:
    std::unordered_map<std::string, uint16_t> symbols_to_ids_;
    std::vector<std::string> ids_to_symbols_;
    QuoteUpdateTableBuilder builder_;
};
