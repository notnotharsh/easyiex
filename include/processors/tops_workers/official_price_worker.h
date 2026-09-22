#pragma once
#include "persistence/official_price_table_builder.h"
#include "processors/tops_workers/symbol_dict_worker.h"

class OfficialPriceWorker : public SymbolDictWorker<OfficialPriceTableBuilder> {
public:
    OfficialPriceWorker(int shard_idx, const std::string& base_path)
        : SymbolDictWorker(shard_idx, base_path) {}

    void Process(const OfficialPriceMsg& msg) {
        builder_.AddRow({msg.timestamp, ResolveSymbol(msg.raw_symbol), msg.price_type, msg.price, msg.session});
    }
};
