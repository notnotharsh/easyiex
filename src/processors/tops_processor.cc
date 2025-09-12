#include "processors/tops_processor.h"
#include "utils/endian_utils.h"

TopsProcessor::TopsProcessor(std::string pcap_name) : PcapProcessor(pcap_name) {}

void TopsProcessor::ProcessPacket(const std::byte *packet) {
    int size = ReadLittleEndian<uint16_t>(packet); packet += 2;
    uint8_t message_byte = ReadLittleEndian<uint8_t>(packet);

    if (static_cast<TopsMessageType>(message_byte) == TopsMessageType::SystemEventMessage) [[unlikely]] {
        ProcessSystemEventMessage(packet);
        return;
    }

    if (!active_hours_) return;

    switch (static_cast<TopsMessageType>(message_byte)) {
        case TopsMessageType::SecurityDirectoryMessage:
            break;
        case TopsMessageType::TradingStatusMessage:
            break;
        case TopsMessageType::RetailLiquidityIndicatorMessage:
            break;
        case TopsMessageType::OperationalHaltStatusMessage:
            break;
        case TopsMessageType::ShortSalePriceTestStatusMessage:
            break;
        case TopsMessageType::QuoteUpdateMessage: [[likely]]
            ProcessQuoteUpdateMessage(packet);
        case TopsMessageType::TradeReportMessage:
            break;
        case TopsMessageType::OfficialPriceMessage:
            break;
        case TopsMessageType::TradeBreakMessage:
            break;
        case TopsMessageType::AuctionInformationMessage:
            break;
        default: std::cout << message_byte << std::endl;
    }
}

void TopsProcessor::ProcessSystemEventMessage(const std::byte *packet) {
    uint8_t system_event = ReadLittleEndian<uint8_t>(packet + 1);
    switch (system_event) {
        case 0x52: 
            active_hours_ = true;
            break;
        case 0x4d:
            active_hours_ = false;
            break;
    }
}

void TopsProcessor::ProcessQuoteUpdateMessage(const std::byte *packet) {
    uint8_t message_type = ReadLittleEndian<uint8_t>(packet + 0);
    uint8_t flags = ReadLittleEndian<uint8_t>(packet + 1);
    uint64_t timestamp = ReadLittleEndian<int64_t>(packet + 2);
    std::string symbol = Int64ToTicker(ReadLittleEndian<int64_t>(packet + 10));

    uint32_t bid_size = ReadLittleEndian<uint32_t>(packet + 18);
    int64_t bid_price = ReadLittleEndian<int64_t>(packet + 22);
    int64_t ask_price = ReadLittleEndian<int64_t>(packet + 30);
    uint32_t ask_size = ReadLittleEndian<uint32_t>(packet + 38);
}