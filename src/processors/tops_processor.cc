#include "processors/tops_processor.h"
#include "utils/endian_utils.h"

TopsProcessor::TopsProcessor(std::string pcap_name)
    : PcapProcessor(pcap_name) {}

void TopsProcessor::ProcessPacket(std::span<const std::byte> packet) {
    uint8_t message_byte = ReadLittleEndian<uint8_t>(packet, 2);

    if (static_cast<TopsMessageType>(message_byte) == TopsMessageType::SystemEventMessage) {
        ProcessSystemEventMessage(packet);
        return;
    }

    if (!active_hours_) return;

    switch (static_cast<TopsMessageType>(message_byte)) {
        case TopsMessageType::SecurityDirectoryMessage:           break;
        case TopsMessageType::TradingStatusMessage:               break;
        case TopsMessageType::RetailLiquidityIndicatorMessage:    break;
        case TopsMessageType::OperationalHaltStatusMessage:       break;
        case TopsMessageType::ShortSalePriceTestStatusMessage:    break;
        case TopsMessageType::QuoteUpdateMessage:
            ProcessQuoteUpdateMessage(packet);
            break;
        case TopsMessageType::TradeReportMessage:                 break;
        case TopsMessageType::OfficialPriceMessage:               break;
        case TopsMessageType::TradeBreakMessage:                  break;
        case TopsMessageType::AuctionInformationMessage:          break;
        default: std::cout << message_byte << std::endl;
    }
}

void TopsProcessor::ProcessSystemEventMessage(std::span<const std::byte> packet) {
    uint8_t system_event = ReadLittleEndian<uint8_t>(packet, 3);
    switch (system_event) {
        case 0x52: active_hours_ = true;  break;
        case 0x4d: active_hours_ = false; break;
    }
}

void TopsProcessor::ProcessQuoteUpdateMessage(std::span<const std::byte> packet) {
    int64_t raw_symbol = ReadLittleEndian<int64_t>(packet, 12);
    quote_pool_.Dispatch(raw_symbol, QuoteUpdateMsg{
        .timestamp  = ReadLittleEndian<uint64_t>(packet, 4),
        .raw_symbol = raw_symbol,
        .bid_size   = ReadLittleEndian<uint32_t>(packet, 20),
        .bid_price  = ReadLittleEndian<int64_t>(packet, 24),
        .ask_price  = ReadLittleEndian<int64_t>(packet, 32),
        .ask_size   = ReadLittleEndian<uint32_t>(packet, 40),
    });
}

void TopsProcessor::WriteToParquet() {
    quote_pool_.Close();
}
