#include <gtest/gtest.h>
#include "processors/tops_workers/symbol_dict_worker.h"

struct StubBuilder {
    StubBuilder(const std::vector<std::string>&, const std::string&) {}
    void Close() {}
};

class TestWorker : public SymbolDictWorker<StubBuilder> {
public:
    TestWorker() : SymbolDictWorker(0, "/tmp/stub_") {}
    uint16_t Resolve(int64_t raw) { return ResolveSymbol(raw); }
};

TEST(ResolveSymbol, FirstSymbolGetsIdZero) {
    TestWorker w;
    const char* s = "AAPL    ";
    int64_t raw = 0;
    for (int i = 0; i < 8; i++)
        raw |= (static_cast<int64_t>(static_cast<uint8_t>(s[i])) << (8 * i));

    EXPECT_EQ(w.Resolve(raw), 0);
}

TEST(ResolveSymbol, SameSymbolReturnsSameId) {
    TestWorker w;
    const char* s = "AAPL    ";
    int64_t raw = 0;
    for (int i = 0; i < 8; i++)
        raw |= (static_cast<int64_t>(static_cast<uint8_t>(s[i])) << (8 * i));

    uint16_t first  = w.Resolve(raw);
    uint16_t second = w.Resolve(raw);
    EXPECT_EQ(first, second);
}

TEST(ResolveSymbol, DifferentSymbolsGetDifferentIds) {
    TestWorker w;
    auto encode = [](const char* s) {
        int64_t raw = 0;
        for (int i = 0; i < 8; i++)
            raw |= (static_cast<int64_t>(static_cast<uint8_t>(s[i])) << (8 * i));
        return raw;
    };

    uint16_t id_aapl = w.Resolve(encode("AAPL    "));
    uint16_t id_spy  = w.Resolve(encode("SPY     "));
    uint16_t id_tsla = w.Resolve(encode("TSLA    "));

    EXPECT_NE(id_aapl, id_spy);
    EXPECT_NE(id_aapl, id_tsla);
    EXPECT_NE(id_spy,  id_tsla);
}

TEST(ResolveSymbol, IdsAreSequential) {
    TestWorker w;
    auto encode = [](const char* s) {
        int64_t raw = 0;
        for (int i = 0; i < 8; i++)
            raw |= (static_cast<int64_t>(static_cast<uint8_t>(s[i])) << (8 * i));
        return raw;
    };

    EXPECT_EQ(w.Resolve(encode("AAPL    ")), 0);
    EXPECT_EQ(w.Resolve(encode("SPY     ")), 1);
    EXPECT_EQ(w.Resolve(encode("TSLA    ")), 2);
}
