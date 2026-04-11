#pragma once
#include <bits/stdc++.h>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include "utils/structures.h"

class QuoteUpdateTableBuilder {
    public:
        QuoteUpdateTableBuilder(const std::vector<std::string>& ids_to_symbols,
                                const std::string& filename,
                                size_t flush_threshold = 1'000'000);
        void AddRow(const QuoteUpdate& row);
        void Close();
    private:
        void Flush();
        void OpenWriter();

        const std::vector<std::string>& ids_to_symbols_;
        std::string filename_;
        size_t flush_threshold_;
        size_t row_count_ = 0;

        std::shared_ptr<arrow::Schema> schema = arrow::schema(
            {
                arrow::field("timestamp", arrow::uint64()),
                arrow::field("symbol", arrow::dictionary(arrow::uint16(), arrow::utf8())),
                arrow::field("bid_size", arrow::uint32()),
                arrow::field("bid_price", arrow::int64()),
                arrow::field("ask_price", arrow::int64()),
                arrow::field("ask_size", arrow::uint32())
            }
        );

        arrow::UInt64Builder timestamps_builder;
        arrow::UInt16Builder symbol_ids_builder;
        arrow::UInt32Builder bid_sizes_builder;
        arrow::Int64Builder bid_prices_builder;
        arrow::Int64Builder ask_prices_builder;
        arrow::UInt32Builder ask_sizes_builder;

        std::shared_ptr<arrow::io::FileOutputStream> outfile_;
        std::unique_ptr<parquet::arrow::FileWriter> writer_;
};
