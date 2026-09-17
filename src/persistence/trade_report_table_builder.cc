#include "persistence/trade_report_table_builder.h"

TradeReportTableBuilder::TradeReportTableBuilder(
    const std::vector<std::string>& ids_to_symbols,
    const std::string& filename,
    size_t flush_threshold)
    : TableBuilderBase(ids_to_symbols, filename, flush_threshold) {}

void TradeReportTableBuilder::AddRow(const TradeReport& row) {
    PARQUET_THROW_NOT_OK(timestamps_builder_.Append(row.timestamp));
    PARQUET_THROW_NOT_OK(symbol_ids_builder_.Append(row.symbol_id));
    PARQUET_THROW_NOT_OK(sizes_builder_.Append(row.size));
    PARQUET_THROW_NOT_OK(prices_builder_.Append(row.price));
    PARQUET_THROW_NOT_OK(trade_ids_builder_.Append(row.trade_id));
    PARQUET_THROW_NOT_OK(sale_condition_flags_builder_.Append(row.sale_condition_flags));
    PARQUET_THROW_NOT_OK(session_builder_.Append(row.session));
    if (++row_count_ >= flush_threshold_) Flush();
}

void TradeReportTableBuilder::Flush() {
    if (row_count_ == 0) return;
    if (!writer_) OpenWriter(*schema_);

    std::shared_ptr<arrow::Array> timestamps, symbol_ids, sizes, prices, trade_ids, sale_condition_flags, session;
    PARQUET_THROW_NOT_OK(timestamps_builder_.Finish(&timestamps));
    PARQUET_THROW_NOT_OK(symbol_ids_builder_.Finish(&symbol_ids));
    PARQUET_THROW_NOT_OK(sizes_builder_.Finish(&sizes));
    PARQUET_THROW_NOT_OK(prices_builder_.Finish(&prices));
    PARQUET_THROW_NOT_OK(trade_ids_builder_.Finish(&trade_ids));
    PARQUET_THROW_NOT_OK(sale_condition_flags_builder_.Finish(&sale_condition_flags));
    PARQUET_THROW_NOT_OK(session_builder_.Finish(&session));

    auto table = arrow::Table::Make(schema_, {timestamps, BuildSymbolColumn(symbol_ids), sizes, prices, trade_ids, sale_condition_flags, session});
    PARQUET_THROW_NOT_OK(writer_->WriteTable(*table, row_count_));
    row_count_ = 0;
}

void TradeReportTableBuilder::Close() {
    Flush();
    CloseWriter();
}
