#pragma once
#include <bits/stdc++.h>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include "utils/structures.h"

class QuoteUpdateTableBuilder {
    public:
        QuoteUpdateTableBuilder() = default;
        QuoteUpdateTableBuilder(const std::string& symbol);
        void AddRow(const QuoteUpdate& row);
        void WriteToParquet();
    private:
        std::shared_ptr<arrow::Schema> schema = arrow::schema(
            {
                arrow::field("timestamp", arrow::uint64()),
                arrow::field("symbol", arrow::utf8()),
                arrow::field("bid_size", arrow::uint32()),
                arrow::field("bid_price", arrow::int64()),
                arrow::field("ask_price", arrow::int64()),
                arrow::field("ask_size", arrow::uint32())
            }
        );

        std::string symbol;
        arrow::UInt64Builder timestamps_builder;
        arrow::UInt32Builder bid_sizes_builder;
        arrow::Int64Builder bid_prices_builder;
        arrow::Int64Builder ask_prices_builder;
        arrow::UInt32Builder ask_sizes_builder;
};