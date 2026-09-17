#include "persistence/quote_update_table_builder.h"

QuoteUpdateTableBuilder::QuoteUpdateTableBuilder(
    const std::vector<std::string>& ids_to_symbols,
    const std::string& filename,
    size_t flush_threshold)
    : TableBuilderBase(ids_to_symbols, filename, flush_threshold) {}

void QuoteUpdateTableBuilder::AddRow(const QuoteUpdate& row) {
    PARQUET_THROW_NOT_OK(timestamps_builder_.Append(row.timestamp));
    PARQUET_THROW_NOT_OK(symbol_ids_builder_.Append(row.symbol_id));
    PARQUET_THROW_NOT_OK(bid_sizes_builder_.Append(row.bid_size));
    PARQUET_THROW_NOT_OK(bid_prices_builder_.Append(row.bid_price));
    PARQUET_THROW_NOT_OK(ask_prices_builder_.Append(row.ask_price));
    PARQUET_THROW_NOT_OK(ask_sizes_builder_.Append(row.ask_size));
    PARQUET_THROW_NOT_OK(flags_builder_.Append(row.flags));
    PARQUET_THROW_NOT_OK(session_builder_.Append(row.session));
    if (++row_count_ >= flush_threshold_) Flush();
}

void QuoteUpdateTableBuilder::Flush() {
    if (row_count_ == 0) return;
    if (!writer_) OpenWriter(*schema_);

    std::shared_ptr<arrow::Array> timestamps, symbol_ids, bid_sizes, bid_prices, ask_prices, ask_sizes, flags, session;
    PARQUET_THROW_NOT_OK(timestamps_builder_.Finish(&timestamps));
    PARQUET_THROW_NOT_OK(symbol_ids_builder_.Finish(&symbol_ids));
    PARQUET_THROW_NOT_OK(bid_sizes_builder_.Finish(&bid_sizes));
    PARQUET_THROW_NOT_OK(bid_prices_builder_.Finish(&bid_prices));
    PARQUET_THROW_NOT_OK(ask_prices_builder_.Finish(&ask_prices));
    PARQUET_THROW_NOT_OK(ask_sizes_builder_.Finish(&ask_sizes));
    PARQUET_THROW_NOT_OK(flags_builder_.Finish(&flags));
    PARQUET_THROW_NOT_OK(session_builder_.Finish(&session));

    auto table = arrow::Table::Make(schema_, {timestamps, BuildSymbolColumn(symbol_ids), bid_sizes, bid_prices, ask_prices, ask_sizes, flags, session});
    PARQUET_THROW_NOT_OK(writer_->WriteTable(*table, row_count_));
    row_count_ = 0;
}

void QuoteUpdateTableBuilder::Close() {
    Flush();
    CloseWriter();
}
