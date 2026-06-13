#include "persistence/trade_report_table_builder.h"
#include "utils/structures.h"

TradeReportTableBuilder::TradeReportTableBuilder(
    const std::vector<std::string>& ids_to_symbols,
    const std::string& filename,
    size_t flush_threshold)
    : ids_to_symbols_(ids_to_symbols)
    , filename_(filename)
    , flush_threshold_(flush_threshold) {}

void TradeReportTableBuilder::OpenWriter() {
    PARQUET_ASSIGN_OR_THROW(outfile_, arrow::io::FileOutputStream::Open(filename_));
    PARQUET_ASSIGN_OR_THROW(writer_, parquet::arrow::FileWriter::Open(
        *schema,
        arrow::default_memory_pool(),
        outfile_,
        parquet::default_writer_properties(),
        parquet::ArrowWriterProperties::Builder().build()));
}

void TradeReportTableBuilder::AddRow(const TradeReport& row) {
    PARQUET_THROW_NOT_OK(timestamps_builder.Append(row.timestamp));
    PARQUET_THROW_NOT_OK(symbol_ids_builder.Append(row.symbol_id));
    PARQUET_THROW_NOT_OK(sizes_builder.Append(row.size));
    PARQUET_THROW_NOT_OK(prices_builder.Append(row.price));
    PARQUET_THROW_NOT_OK(trade_ids_builder.Append(row.trade_id));
    PARQUET_THROW_NOT_OK(sale_condition_flags_builder.Append(row.sale_condition_flags));

    if (++row_count_ >= flush_threshold_) {
        Flush();
    }
}

void TradeReportTableBuilder::Flush() {
    if (row_count_ == 0) return;

    if (!writer_) {
        OpenWriter();
    }

    std::shared_ptr<arrow::Array> timestamps;
    std::shared_ptr<arrow::Array> symbol_ids;
    std::shared_ptr<arrow::Array> sizes;
    std::shared_ptr<arrow::Array> prices;
    std::shared_ptr<arrow::Array> trade_ids;
    std::shared_ptr<arrow::Array> sale_condition_flags;

    PARQUET_THROW_NOT_OK(timestamps_builder.Finish(&timestamps));
    PARQUET_THROW_NOT_OK(symbol_ids_builder.Finish(&symbol_ids));
    PARQUET_THROW_NOT_OK(sizes_builder.Finish(&sizes));
    PARQUET_THROW_NOT_OK(prices_builder.Finish(&prices));
    PARQUET_THROW_NOT_OK(trade_ids_builder.Finish(&trade_ids));
    PARQUET_THROW_NOT_OK(sale_condition_flags_builder.Finish(&sale_condition_flags));

    const std::shared_ptr<arrow::DataType> dict_type = arrow::dictionary(arrow::uint16(), arrow::utf8());
    arrow::StringBuilder symbols_dict_builder;
    std::shared_ptr<arrow::Array> symbols_dict;
    std::shared_ptr<arrow::Array> symbols;

    PARQUET_THROW_NOT_OK(symbols_dict_builder.AppendValues(ids_to_symbols_));
    PARQUET_ASSIGN_OR_THROW(symbols_dict, symbols_dict_builder.Finish());
    PARQUET_ASSIGN_OR_THROW(symbols, arrow::DictionaryArray::FromArrays(dict_type, symbol_ids, symbols_dict));

    std::shared_ptr<arrow::Table> table = arrow::Table::Make(
        schema, {timestamps, symbols, sizes, prices, trade_ids, sale_condition_flags});

    PARQUET_THROW_NOT_OK(writer_->WriteTable(*table, row_count_));
    row_count_ = 0;
}

void TradeReportTableBuilder::Close() {
    Flush();
    if (writer_) {
        PARQUET_THROW_NOT_OK(writer_->Close());
    }
    if (outfile_) {
        PARQUET_THROW_NOT_OK(outfile_->Close());
    }
}
