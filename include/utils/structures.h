#pragma once
#include <cstdint>

struct QuoteUpdate {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint32_t bid_size;
    int64_t bid_price;
    int64_t ask_price;
    uint32_t ask_size;
};

struct QuoteUpdateMsg {
    uint64_t timestamp;
    int64_t  raw_symbol;
    uint32_t bid_size;
    int64_t  bid_price;
    int64_t  ask_price;
    uint32_t ask_size;
};

struct TradeReport {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
};

struct TradeReportMsg {
    uint64_t timestamp;
    int64_t  raw_symbol;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
};

struct TradeBreak {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
};

struct TradeBreakMsg {
    uint64_t timestamp;
    int64_t  raw_symbol;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
};