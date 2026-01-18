#pragma once
#include "processors/pcap_processor.h"
#include "persistence/quote_update_table_builder.h"

class TopsProcessor : public PcapProcessor {
    public:
        TopsProcessor(std::string pcap_name);
        void ProcessPacket(std::span<const std::byte> packet) override;
        void WriteToParquet() override;
    
    private:
        bool active_hours_;
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

        QuoteUpdateTableBuilder quote_update_table_builder;

        std::unordered_map<std::string, uint16_t> symbols_to_ids;
        std::vector<std::string> ids_to_symbols;

        void ProcessSystemEventMessage(std::span<const std::byte> packet);
        void ProcessQuoteUpdateMessage(std::span<const std::byte> packet);
};