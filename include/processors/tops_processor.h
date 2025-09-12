#pragma once
#include "processors/pcap_processor.h"

class TopsProcessor : public PcapProcessor {
    public:
        TopsProcessor(std::string pcap_name);
        void ProcessPacket(const std::byte *packet) override;
    
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

        void ProcessSystemEventMessage(const std::byte *packet) ;
        void ProcessQuoteUpdateMessage(const std::byte *packet) ;
};