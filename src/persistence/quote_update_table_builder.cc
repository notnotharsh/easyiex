#include "persistence/quote_update_table_builder.h"
#include "utils/structures.h"

void QuoteUpdateTableBuilder::AddRow(const QuoteUpdate& row) {
    PARQUET_THROW_NOT_OK(timestamps_builder.Append(row.timestamp));
    PARQUET_THROW_NOT_OK(symbol_ids_builder.Append(row.symbol_id));
    PARQUET_THROW_NOT_OK(bid_sizes_builder.Append(row.bid_size));
    PARQUET_THROW_NOT_OK(bid_prices_builder.Append(row.bid_price));
    PARQUET_THROW_NOT_OK(ask_prices_builder.Append(row.ask_price));
    PARQUET_THROW_NOT_OK(ask_sizes_builder.Append(row.ask_size));
}

void QuoteUpdateTableBuilder::WriteToParquet(const std::vector<std::string>& ids_to_symbols) {
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
    arrow::StringBuilder symbols_dict_builder = arrow::StringBuilder();
    std::shared_ptr<arrow::Array> symbols_dict;
    std::shared_ptr<arrow::Array> symbols;

    PARQUET_THROW_NOT_OK(symbols_dict_builder.AppendValues(ids_to_symbols));
    PARQUET_ASSIGN_OR_THROW(symbols_dict, symbols_dict_builder.Finish());
    PARQUET_ASSIGN_OR_THROW(symbols, arrow::DictionaryArray::FromArrays(dict_type, symbol_ids, symbols_dict));

    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, {timestamps, symbols, bid_sizes, bid_prices, ask_prices, ask_sizes});
    std::shared_ptr<arrow::io::FileOutputStream> outfile;

    std::string fname = "data.parquet";
    PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(fname));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 1e6));
}