#pragma once
#include "persistence/quote_update_table_builder.h"
#include "processors/tops_workers/symbol_dict_worker.h"

class QuoteUpdateWorker : public SymbolDictWorker<QuoteUpdateTableBuilder> {
public:
    explicit QuoteUpdateWorker(int shard_idx)
        : SymbolDictWorker(shard_idx, "data_quotes_") {}

    void Process(const QuoteUpdateMsg& msg) {
        builder_.AddRow({msg.timestamp, ResolveSymbol(msg.raw_symbol), msg.bid_size, msg.bid_price, msg.ask_price, msg.ask_size});
    }
};
