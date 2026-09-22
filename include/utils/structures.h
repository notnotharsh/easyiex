#pragma once
#include <cstdint>

struct QuoteUpdate {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint32_t bid_size;
    int64_t bid_price;
    int64_t ask_price;
    uint32_t ask_size;
    uint8_t  flags;
    uint8_t  session;
};

struct QuoteUpdateMsg {
    uint64_t timestamp;
    int64_t  raw_symbol;
    uint32_t bid_size;
    int64_t  bid_price;
    int64_t  ask_price;
    uint32_t ask_size;
    uint8_t  flags;
    uint8_t  session;
};

struct TradeReport {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
    uint8_t  session;
};

struct TradeReportMsg {
    uint64_t timestamp;
    int64_t  raw_symbol;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
    uint8_t  session;
};

struct TradeBreak {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
    uint8_t  session;
};

struct TradeBreakMsg {
    uint64_t timestamp;
    int64_t  raw_symbol;
    uint32_t size;
    int64_t  price;
    int64_t  trade_id;
    uint8_t  sale_condition_flags;
    uint8_t  session;
};

struct OfficialPrice {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint8_t  price_type;
    int64_t  price;
    uint8_t  session;
};

struct OfficialPriceMsg {
    uint64_t timestamp;
    int64_t  raw_symbol;
    uint8_t  price_type;
    int64_t  price;
    uint8_t  session;
};