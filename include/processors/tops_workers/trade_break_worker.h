#pragma once
#include "persistence/trade_break_table_builder.h"
#include "processors/tops_workers/symbol_dict_worker.h"

class TradeBreakWorker : public SymbolDictWorker<TradeBreakTableBuilder> {
public:
    TradeBreakWorker(int shard_idx, const std::string& base_path)
        : SymbolDictWorker(shard_idx, base_path) {}

    void Process(const TradeBreakMsg& msg) {
        builder_.AddRow({msg.timestamp, ResolveSymbol(msg.raw_symbol), msg.size, msg.price, msg.trade_id, msg.sale_condition_flags, msg.session});
    }
};
