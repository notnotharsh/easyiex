#include "persistence/official_price_table_builder.h"

OfficialPriceTableBuilder::OfficialPriceTableBuilder(
    const std::vector<std::string>& ids_to_symbols,
    const std::string& filename,
    size_t flush_threshold)
    : TableBuilderBase(ids_to_symbols, filename, flush_threshold) {}

void OfficialPriceTableBuilder::AddRow(const OfficialPrice& row) {
    PARQUET_THROW_NOT_OK(timestamps_builder_.Append(row.timestamp));
    PARQUET_THROW_NOT_OK(symbol_ids_builder_.Append(row.symbol_id));
    PARQUET_THROW_NOT_OK(price_types_builder_.Append(row.price_type));
    PARQUET_THROW_NOT_OK(prices_builder_.Append(row.price));
    PARQUET_THROW_NOT_OK(session_builder_.Append(row.session));
    if (++row_count_ >= flush_threshold_) Flush();
}

void OfficialPriceTableBuilder::Flush() {
    if (row_count_ == 0) return;
    if (!writer_) OpenWriter(*schema_);

    std::shared_ptr<arrow::Array> timestamps, symbol_ids, price_types, prices, session;
    PARQUET_THROW_NOT_OK(timestamps_builder_.Finish(&timestamps));
    PARQUET_THROW_NOT_OK(symbol_ids_builder_.Finish(&symbol_ids));
    PARQUET_THROW_NOT_OK(price_types_builder_.Finish(&price_types));
    PARQUET_THROW_NOT_OK(prices_builder_.Finish(&prices));
    PARQUET_THROW_NOT_OK(session_builder_.Finish(&session));

    auto table = arrow::Table::Make(schema_, {timestamps, BuildSymbolColumn(symbol_ids), price_types, prices, session});
    PARQUET_THROW_NOT_OK(writer_->WriteTable(*table, row_count_));
    row_count_ = 0;
}

void OfficialPriceTableBuilder::Close() {
    Flush();
    CloseWriter();
}
