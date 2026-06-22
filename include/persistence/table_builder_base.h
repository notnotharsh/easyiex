#pragma once
#include <bits/stdc++.h>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

class TableBuilderBase {
protected:
    TableBuilderBase(const std::vector<std::string>& ids_to_symbols,
                     const std::string& filename,
                     size_t flush_threshold)
        : ids_to_symbols_(ids_to_symbols)
        , filename_(filename)
        , flush_threshold_(flush_threshold) {}

    void OpenWriter(const arrow::Schema& schema) {
        PARQUET_ASSIGN_OR_THROW(outfile_, arrow::io::FileOutputStream::Open(filename_));
        PARQUET_ASSIGN_OR_THROW(writer_, parquet::arrow::FileWriter::Open(
            schema,
            arrow::default_memory_pool(),
            outfile_,
            parquet::default_writer_properties(),
            parquet::ArrowWriterProperties::Builder().build()));
    }

    void CloseWriter() {
        if (writer_)  PARQUET_THROW_NOT_OK(writer_->Close());
        if (outfile_) PARQUET_THROW_NOT_OK(outfile_->Close());
    }

    std::shared_ptr<arrow::Array> BuildSymbolColumn(std::shared_ptr<arrow::Array> symbol_ids) {
        const auto dict_type = arrow::dictionary(arrow::uint16(), arrow::utf8());
        arrow::StringBuilder dict_builder;
        std::shared_ptr<arrow::Array> symbols_dict, symbols;
        PARQUET_THROW_NOT_OK(dict_builder.AppendValues(ids_to_symbols_));
        PARQUET_ASSIGN_OR_THROW(symbols_dict, dict_builder.Finish());
        PARQUET_ASSIGN_OR_THROW(symbols, arrow::DictionaryArray::FromArrays(dict_type, symbol_ids, symbols_dict));
        return symbols;
    }

    const std::vector<std::string>& ids_to_symbols_;
    std::string filename_;
    size_t flush_threshold_;
    size_t row_count_ = 0;
    std::shared_ptr<arrow::io::FileOutputStream> outfile_;
    std::unique_ptr<parquet::arrow::FileWriter> writer_;
};
