#pragma once
#include "persistence/table_builder_base.h"
#include "utils/structures.h"

class OfficialPriceTableBuilder : public TableBuilderBase {
public:
    OfficialPriceTableBuilder(const std::vector<std::string>& ids_to_symbols,
                              const std::string& filename,
                              size_t flush_threshold = 1'000'000);
    void AddRow(const OfficialPrice& row);
    void Close();

private:
    void Flush();

    std::shared_ptr<arrow::Schema> schema_ = arrow::schema({
        arrow::field("timestamp",  arrow::uint64()),
        arrow::field("symbol",     arrow::dictionary(arrow::uint16(), arrow::utf8())),
        arrow::field("price_type", arrow::uint8()),
        arrow::field("price",      arrow::int64()),
        arrow::field("session",    arrow::uint8())
    });

    arrow::UInt64Builder timestamps_builder_;
    arrow::UInt16Builder symbol_ids_builder_;
    arrow::UInt8Builder  price_types_builder_;
    arrow::Int64Builder  prices_builder_;
    arrow::UInt8Builder  session_builder_;
};
