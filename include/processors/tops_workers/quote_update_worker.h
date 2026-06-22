#pragma once
#include "persistence/quote_update_table_builder.h"
#include "processors/tops_workers/symbol_dict_worker.h"

class QuoteUpdateWorker : public SymbolDictWorker<QuoteUpdateTableBuilder> {
public:
    QuoteUpdateWorker(int shard_idx, const std::string& base_path)
        : SymbolDictWorker(shard_idx, base_path) {}

    void Process(const QuoteUpdateMsg& msg) {
        builder_.AddRow({msg.timestamp, ResolveSymbol(msg.raw_symbol), msg.bid_size, msg.bid_price, msg.ask_price, msg.ask_size});
    }
};
