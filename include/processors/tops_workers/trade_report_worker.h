#pragma once
#include "persistence/trade_report_table_builder.h"
#include "processors/tops_workers/symbol_dict_worker.h"

class TradeReportWorker : public SymbolDictWorker<TradeReportTableBuilder> {
public:
    explicit TradeReportWorker(int shard_idx)
        : SymbolDictWorker(shard_idx, "data_trades_") {}

    void Process(const TradeReportMsg& msg) {
        builder_.AddRow({msg.timestamp, ResolveSymbol(msg.raw_symbol), msg.size, msg.price, msg.trade_id, msg.sale_condition_flags});
    }
};
