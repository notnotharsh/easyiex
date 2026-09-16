#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/endian_utils.h"

template<typename BuilderT>
class SymbolDictWorker {
public:
    SymbolDictWorker(int shard_idx, const std::string& filename_prefix)
        : builder_(ids_to_symbols_, filename_prefix + std::to_string(shard_idx) + ".parquet") {}

    void Close() { builder_.Close(); }
    const std::vector<std::string>& Symbols() const { return ids_to_symbols_; }

protected:
    uint16_t ResolveSymbol(int64_t raw_symbol) {
        std::string symbol = Int64ToTicker(raw_symbol);
        auto it = symbols_to_ids_.find(symbol);
        if (it != symbols_to_ids_.end()) return it->second;
        if (ids_to_symbols_.size() >= std::numeric_limits<uint16_t>::max())
            throw std::runtime_error("symbol dictionary overflow");
        uint16_t id = static_cast<uint16_t>(ids_to_symbols_.size());
        ids_to_symbols_.push_back(symbol);
        symbols_to_ids_[symbol] = id;
        return id;
    }

    std::unordered_map<std::string, uint16_t> symbols_to_ids_;
    std::vector<std::string> ids_to_symbols_;
    BuilderT builder_;
};
