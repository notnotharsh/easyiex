#include "processors/tops_processor.h"
#include "utils/endian_utils.h"

TopsProcessor::TopsProcessor(std::string pcap_name) : PcapProcessor(pcap_name) {}

void TopsProcessor::ProcessPacket(const std::byte *packet) const {
    int size = read_little_endian<uint16_t>(packet); packet += 2;
    uint8_t message_byte = read_little_endian<uint8_t>(packet);

    switch (static_cast<TopsMessageType>(message_byte)) {
        case TopsMessageType::SystemEventMessage:
        case TopsMessageType::SecurityDirectoryMessage:
        case TopsMessageType::TradingStatusMessage:
        case TopsMessageType::RetailLiquidityIndicatorMessage:
        case TopsMessageType::OperationalHaltStatusMessage:
        case TopsMessageType::ShortSalePriceTestStatusMessage:
        case TopsMessageType::QuoteUpdateMessage: [[likely]]
        case TopsMessageType::TradeReportMessage:
        case TopsMessageType::OfficialPriceMessage:
        case TopsMessageType::TradeBreakMessage:
        case TopsMessageType::AuctionInformationMessage:
        default: std::cout << message_byte << std::endl;
    }
}