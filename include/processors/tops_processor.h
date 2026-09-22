#pragma once
#include "processors/pcap_processor.h"
#include "utils/worker_pool.h"
#include "processors/tops_workers/quote_update_worker.h"
#include "processors/tops_workers/trade_report_worker.h"
#include "processors/tops_workers/trade_break_worker.h"
#include "processors/tops_workers/official_price_worker.h"

class TopsProcessor : public PcapProcessor<TopsProcessor> {
public:
    TopsProcessor(std::string pcap_name);
    void ProcessPacket(std::span<const std::byte> packet);
    void WriteToParquet();

private:
    enum class MarketSession : uint8_t {
        PreMarket  = 0,
        Regular    = 1,
        PostMarket = 2
    };
    MarketSession session_ = MarketSession::PreMarket;
    enum class TopsMessageType : uint8_t {
        SystemEventMessage = 0x53,
        SecurityDirectoryMessage = 0x44,
        TradingStatusMessage = 0x48,
        RetailLiquidityIndicatorMessage = 0x49,
        OperationalHaltStatusMessage = 0x4f,
        ShortSalePriceTestStatusMessage = 0x50,
        QuoteUpdateMessage = 0x51,
        TradeReportMessage = 0x54,
        OfficialPriceMessage = 0x58,
        TradeBreakMessage = 0x42,
        AuctionInformationMessage = 0x41
    };

    std::string output_dir_;
    WorkerPool<QuoteUpdateWorker, QuoteUpdateMsg> quote_pool_;
    WorkerPool<TradeReportWorker, TradeReportMsg> trade_pool_;
    WorkerPool<TradeBreakWorker, TradeBreakMsg> break_pool_;
    WorkerPool<OfficialPriceWorker, OfficialPriceMsg> official_pool_;

    static std::string SetupOutputDir(const std::string& pcap_name);
    void WriteShardMap() const;
    void ProcessSystemEventMessage(std::span<const std::byte> packet);
    void ProcessQuoteUpdateMessage(std::span<const std::byte> packet);
    void ProcessTradeReportMessage(std::span<const std::byte> packet);
    void ProcessTradeBreakMessage(std::span<const std::byte> packet);
    void ProcessOfficialPriceMessage(std::span<const std::byte> packet);
};
