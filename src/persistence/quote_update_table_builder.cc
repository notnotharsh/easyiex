#include "persistence/quote_update_table_builder.h"
#include "utils/structures.h"

QuoteUpdateTableBuilder::QuoteUpdateTableBuilder(
    const std::vector<std::string>& ids_to_symbols,
    const std::string& filename,
    size_t flush_threshold)
    : ids_to_symbols_(ids_to_symbols)
    , filename_(filename)
    , flush_threshold_(flush_threshold) {}

void QuoteUpdateTableBuilder::OpenWriter() {
    PARQUET_ASSIGN_OR_THROW(outfile_, arrow::io::FileOutputStream::Open(filename_));
    PARQUET_ASSIGN_OR_THROW(writer_, parquet::arrow::FileWriter::Open(
        *schema,
        arrow::default_memory_pool(),
        outfile_,
        parquet::default_writer_properties(),
        parquet::ArrowWriterProperties::Builder().build()));
}

void QuoteUpdateTableBuilder::AddRow(const QuoteUpdate& row) {
    PARQUET_THROW_NOT_OK(timestamps_builder.Append(row.timestamp));
    PARQUET_THROW_NOT_OK(symbol_ids_builder.Append(row.symbol_id));
    PARQUET_THROW_NOT_OK(bid_sizes_builder.Append(row.bid_size));
    PARQUET_THROW_NOT_OK(bid_prices_builder.Append(row.bid_price));
    PARQUET_THROW_NOT_OK(ask_prices_builder.Append(row.ask_price));
    PARQUET_THROW_NOT_OK(ask_sizes_builder.Append(row.ask_size));

    if (++row_count_ >= flush_threshold_) {
        Flush();
    }
}

void QuoteUpdateTableBuilder::Flush() {
    if (row_count_ == 0) return;

    if (!writer_) {
        OpenWriter();
    }

    std::shared_ptr<arrow::Array> timestamps;
    std::shared_ptr<arrow::Array> symbol_ids;
    std::shared_ptr<arrow::Array> bid_sizes;
    std::shared_ptr<arrow::Array> bid_prices;
    std::shared_ptr<arrow::Array> ask_prices;
    std::shared_ptr<arrow::Array> ask_sizes;

    PARQUET_THROW_NOT_OK(timestamps_builder.Finish(&timestamps));
    PARQUET_THROW_NOT_OK(symbol_ids_builder.Finish(&symbol_ids));
    PARQUET_THROW_NOT_OK(bid_sizes_builder.Finish(&bid_sizes));
    PARQUET_THROW_NOT_OK(bid_prices_builder.Finish(&bid_prices));
    PARQUET_THROW_NOT_OK(ask_prices_builder.Finish(&ask_prices));
    PARQUET_THROW_NOT_OK(ask_sizes_builder.Finish(&ask_sizes));

    const std::shared_ptr<arrow::DataType> dict_type = arrow::dictionary(arrow::uint16(), arrow::utf8());
    arrow::StringBuilder symbols_dict_builder;
    std::shared_ptr<arrow::Array> symbols_dict;
    std::shared_ptr<arrow::Array> symbols;

    PARQUET_THROW_NOT_OK(symbols_dict_builder.AppendValues(ids_to_symbols_));
    PARQUET_ASSIGN_OR_THROW(symbols_dict, symbols_dict_builder.Finish());
    PARQUET_ASSIGN_OR_THROW(symbols, arrow::DictionaryArray::FromArrays(dict_type, symbol_ids, symbols_dict));

    std::shared_ptr<arrow::Table> table = arrow::Table::Make(
        schema, {timestamps, symbols, bid_sizes, bid_prices, ask_prices, ask_sizes});

    PARQUET_THROW_NOT_OK(writer_->WriteTable(*table, row_count_));
    row_count_ = 0;
}

void QuoteUpdateTableBuilder::Close() {
    Flush();
    if (writer_) {
        PARQUET_THROW_NOT_OK(writer_->Close());
    }
    if (outfile_) {
        PARQUET_THROW_NOT_OK(outfile_->Close());
    }
}
