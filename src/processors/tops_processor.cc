#include "processors/tops_processor.h"
#include "utils/endian_utils.h"
#include "utils/time_utils.h"
#include "utils/structures.h"

TopsProcessor::TopsProcessor(std::string pcap_name)
    : PcapProcessor(pcap_name)
    , quote_update_table_builder(ids_to_symbols, "data.parquet") {}

void TopsProcessor::ProcessPacket(std::span<const std::byte> packet) {
    int size = ReadLittleEndian<uint16_t>(packet, 0);
    uint8_t message_byte = ReadLittleEndian<uint8_t>(packet, 2);

    if (static_cast<TopsMessageType>(message_byte) == TopsMessageType::SystemEventMessage) {
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
        case TopsMessageType::QuoteUpdateMessage:
            ProcessQuoteUpdateMessage(packet);
            break;
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

void TopsProcessor::ProcessSystemEventMessage(std::span<const std::byte> packet) {
    uint8_t system_event = ReadLittleEndian<uint8_t>(packet, 3);
    switch (system_event) {
        case 0x52: 
            active_hours_ = true;
            break;
        case 0x4d:
            active_hours_ = false;
            break;
    }
}

void TopsProcessor::ProcessQuoteUpdateMessage(std::span<const std::byte> packet) {

    uint8_t message_type = ReadLittleEndian<uint8_t>(packet, 2);
    uint8_t flags = ReadLittleEndian<uint8_t>(packet, 3);
    uint64_t timestamp = ReadLittleEndian<int64_t>(packet, 4);
    std::string symbol = Int64ToTicker(ReadLittleEndian<int64_t>(packet, 12));
    uint32_t bid_size = ReadLittleEndian<uint32_t>(packet, 20);
    int64_t bid_price = ReadLittleEndian<int64_t>(packet, 24);
    int64_t ask_price = ReadLittleEndian<int64_t>(packet, 32);
    uint32_t ask_size = ReadLittleEndian<uint32_t>(packet, 40);

    uint16_t symbol_id;
    if (ids_to_symbols.size() >= std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("symbol dictionary overflow");
    }

    if (symbols_to_ids.count(symbol)) {
        symbol_id = symbols_to_ids[symbol];
    } else {
        symbol_id = static_cast<uint16_t>(ids_to_symbols.size());
        ids_to_symbols.push_back(symbol);
        symbols_to_ids.insert({symbol, symbol_id});
    }

    QuoteUpdate row {
        .timestamp = timestamp,
        .symbol_id = symbol_id,
        .bid_size = bid_size,
        .bid_price = bid_price,
        .ask_price = ask_price,
        .ask_size = ask_size
    };
    
    quote_update_table_builder.AddRow(row);
}

void TopsProcessor::WriteToParquet() {
    quote_update_table_builder.Close();
}