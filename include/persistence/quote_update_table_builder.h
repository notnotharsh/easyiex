#pragma once
#include "persistence/table_builder_base.h"
#include "utils/structures.h"

class QuoteUpdateTableBuilder : public TableBuilderBase {
public:
    QuoteUpdateTableBuilder(const std::vector<std::string>& ids_to_symbols,
                            const std::string& filename,
                            size_t flush_threshold = 1'000'000);
    void AddRow(const QuoteUpdate& row);
    void Close();

private:
    void Flush();

    std::shared_ptr<arrow::Schema> schema_ = arrow::schema({
        arrow::field("timestamp", arrow::uint64()),
        arrow::field("symbol",    arrow::dictionary(arrow::uint16(), arrow::utf8())),
        arrow::field("bid_size",  arrow::uint32()),
        arrow::field("bid_price", arrow::int64()),
        arrow::field("ask_price", arrow::int64()),
        arrow::field("ask_size",  arrow::uint32()),
        arrow::field("flags",     arrow::uint8()),
        arrow::field("session",   arrow::uint8())
    });

    arrow::UInt64Builder timestamps_builder_;
    arrow::UInt16Builder symbol_ids_builder_;
    arrow::UInt32Builder bid_sizes_builder_;
    arrow::Int64Builder  bid_prices_builder_;
    arrow::Int64Builder  ask_prices_builder_;
    arrow::UInt32Builder ask_sizes_builder_;
    arrow::UInt8Builder  flags_builder_;
    arrow::UInt8Builder  session_builder_;
};
