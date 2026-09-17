#pragma once
#include "persistence/table_builder_base.h"
#include "utils/structures.h"

class TradeReportTableBuilder : public TableBuilderBase {
public:
    TradeReportTableBuilder(const std::vector<std::string>& ids_to_symbols,
                            const std::string& filename,
                            size_t flush_threshold = 1'000'000);
    void AddRow(const TradeReport& row);
    void Close();

private:
    void Flush();

    std::shared_ptr<arrow::Schema> schema_ = arrow::schema({
        arrow::field("timestamp",             arrow::uint64()),
        arrow::field("symbol",                arrow::dictionary(arrow::uint16(), arrow::utf8())),
        arrow::field("size",                  arrow::uint32()),
        arrow::field("price",                 arrow::int64()),
        arrow::field("trade_id",              arrow::int64()),
        arrow::field("sale_condition_flags",  arrow::uint8()),
        arrow::field("session",               arrow::uint8())
    });

    arrow::UInt64Builder timestamps_builder_;
    arrow::UInt16Builder symbol_ids_builder_;
    arrow::UInt32Builder sizes_builder_;
    arrow::Int64Builder  prices_builder_;
    arrow::Int64Builder  trade_ids_builder_;
    arrow::UInt8Builder  sale_condition_flags_builder_;
    arrow::UInt8Builder  session_builder_;
};
