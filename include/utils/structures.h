#pragma once
#include <bits/stdc++.h>

struct QuoteUpdate {
    uint64_t timestamp;
    uint16_t symbol_id;
    uint32_t bid_size;
    int64_t bid_price;
    int64_t ask_price;
    uint32_t ask_size;
};