#include "persistence/quote_update_table_builder.h"
#include "utils/structures.h"

QuoteUpdateTableBuilder::QuoteUpdateTableBuilder(const std::string& symbol) : symbol (symbol) {}

void QuoteUpdateTableBuilder::AddRow(const QuoteUpdate& row) {
    PARQUET_THROW_NOT_OK(timestamps_builder.Append(row.timestamp));
    PARQUET_THROW_NOT_OK(bid_sizes_builder.Append(row.bid_size));
    PARQUET_THROW_NOT_OK(bid_prices_builder.Append(row.bid_price));
    PARQUET_THROW_NOT_OK(ask_prices_builder.Append(row.ask_price));
    PARQUET_THROW_NOT_OK(ask_sizes_builder.Append(row.ask_size));
}

void QuoteUpdateTableBuilder::WriteToParquet() {
    std::shared_ptr<arrow::Array> timestamps;
    std::shared_ptr<arrow::Array> bid_sizes;
    std::shared_ptr<arrow::Array> bid_prices;
    std::shared_ptr<arrow::Array> ask_prices;
    std::shared_ptr<arrow::Array> ask_sizes;

    PARQUET_THROW_NOT_OK(timestamps_builder.Finish(&timestamps));
    PARQUET_THROW_NOT_OK(bid_sizes_builder.Finish(&bid_sizes));
    PARQUET_THROW_NOT_OK(bid_prices_builder.Finish(&bid_prices));
    PARQUET_THROW_NOT_OK(ask_prices_builder.Finish(&ask_prices));
    PARQUET_THROW_NOT_OK(ask_sizes_builder.Finish(&ask_sizes));

    uint64_t length = timestamps->length();
    auto symbol_scalar = std::make_shared<arrow::StringScalar>(symbol);
    auto symbols = arrow::MakeArrayFromScalar(*symbol_scalar, length).ValueOrDie();

    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, {timestamps, symbols, bid_sizes, bid_prices, ask_prices, ask_sizes});
    std::shared_ptr<arrow::io::FileOutputStream> outfile;

    std::string fname = symbol + ".parquet";
    PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(fname));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 1e6));
}