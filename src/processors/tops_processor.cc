#include "processors/tops_processor.h"
#include "utils/endian_utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>

std::string TopsProcessor::SetupOutputDir(const std::string& pcap_name) {
    namespace fs = std::filesystem;
    std::string out = "output/" + fs::path(pcap_name).stem().string();
    fs::create_directories(out + "/quotes");
    fs::create_directories(out + "/trades");
    fs::create_directories(out + "/breaks");
    fs::create_directories(out + "/official_prices");
    return out;
}

TopsProcessor::TopsProcessor(std::string pcap_name)
    : PcapProcessor(pcap_name)
    , output_dir_(SetupOutputDir(pcap_name))
    , quote_pool_(output_dir_ + "/quotes/quotes_")
    , trade_pool_(output_dir_ + "/trades/trades_")
    , break_pool_(output_dir_ + "/breaks/breaks_")
    , official_pool_(output_dir_ + "/official_prices/official_prices_") {}

void TopsProcessor::ProcessPacket(std::span<const std::byte> packet) {
    uint8_t message_byte = ReadLittleEndian<uint8_t>(packet, 2);

    if (static_cast<TopsMessageType>(message_byte) == TopsMessageType::SystemEventMessage) {
        ProcessSystemEventMessage(packet);
        return;
    }

    switch (static_cast<TopsMessageType>(message_byte)) {
        case TopsMessageType::SecurityDirectoryMessage:           break;
        case TopsMessageType::TradingStatusMessage:               break;
        case TopsMessageType::RetailLiquidityIndicatorMessage:    break;
        case TopsMessageType::OperationalHaltStatusMessage:       break;
        case TopsMessageType::ShortSalePriceTestStatusMessage:    break;
        case TopsMessageType::QuoteUpdateMessage:
            ProcessQuoteUpdateMessage(packet);
            break;
        case TopsMessageType::TradeReportMessage:
            ProcessTradeReportMessage(packet);
            break;
        case TopsMessageType::OfficialPriceMessage:
            ProcessOfficialPriceMessage(packet);
            break;
        case TopsMessageType::TradeBreakMessage:
            ProcessTradeBreakMessage(packet);
            break;
        case TopsMessageType::AuctionInformationMessage:          break;
        default: std::cout << message_byte << std::endl;
    }
}

void TopsProcessor::ProcessSystemEventMessage(std::span<const std::byte> packet) {
    uint8_t system_event = ReadLittleEndian<uint8_t>(packet, 3);
    switch (system_event) {
        case 0x53: session_ = MarketSession::PreMarket;  break;  // 'S' Start of System Hours
        case 0x52: session_ = MarketSession::Regular;    break;  // 'R' Start of Regular Market Hours
        case 0x4d: session_ = MarketSession::PostMarket; break;  // 'M' End of Regular Market Hours
        case 0x45: session_ = MarketSession::PostMarket; break;  // 'E' End of System Hours
        case 0x43: session_ = MarketSession::PostMarket; break;  // 'C' End of Messages
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
        .flags      = ReadLittleEndian<uint8_t>(packet, 3),
        .session    = static_cast<uint8_t>(session_),
    });
}

void TopsProcessor::ProcessTradeReportMessage(std::span<const std::byte> packet) {
    int64_t raw_symbol = ReadLittleEndian<int64_t>(packet, 12);
    trade_pool_.Dispatch(raw_symbol, TradeReportMsg{
        .timestamp            = ReadLittleEndian<uint64_t>(packet, 4),
        .raw_symbol           = raw_symbol,
        .size                 = ReadLittleEndian<uint32_t>(packet, 20),
        .price                = ReadLittleEndian<int64_t>(packet, 24),
        .trade_id             = ReadLittleEndian<int64_t>(packet, 32),
        .sale_condition_flags = ReadLittleEndian<uint8_t>(packet, 3),
        .session              = static_cast<uint8_t>(session_),
    });
}

void TopsProcessor::ProcessTradeBreakMessage(std::span<const std::byte> packet) {
    int64_t raw_symbol = ReadLittleEndian<int64_t>(packet, 12);
    break_pool_.Dispatch(raw_symbol, TradeBreakMsg{
        .timestamp            = ReadLittleEndian<uint64_t>(packet, 4),
        .raw_symbol           = raw_symbol,
        .size                 = ReadLittleEndian<uint32_t>(packet, 20),
        .price                = ReadLittleEndian<int64_t>(packet, 24),
        .trade_id             = ReadLittleEndian<int64_t>(packet, 32),
        .sale_condition_flags = ReadLittleEndian<uint8_t>(packet, 3),
        .session              = static_cast<uint8_t>(session_),
    });
}

void TopsProcessor::ProcessOfficialPriceMessage(std::span<const std::byte> packet) {
    int64_t raw_symbol = ReadLittleEndian<int64_t>(packet, 12);
    official_pool_.Dispatch(raw_symbol, OfficialPriceMsg{
        .timestamp  = ReadLittleEndian<uint64_t>(packet, 4),
        .raw_symbol = raw_symbol,
        .price_type = ReadLittleEndian<uint8_t>(packet, 3),
        .price      = ReadLittleEndian<int64_t>(packet, 20),
        .session    = static_cast<uint8_t>(session_),
    });
}

void TopsProcessor::WriteShardMap() const {
    std::ofstream f(output_dir_ + "/shard_map.json");
    f << "{\n  \"quotes\": {";
    bool first = true;
    quote_pool_.ForEachWorker([&](int shard, const QuoteUpdateWorker& w) {
        for (const auto& sym : w.Symbols()) {
            f << (first ? "\n" : ",\n") << "    \"" << sym << "\": " << shard;
            first = false;
        }
    });
    f << "\n  },\n  \"trades\": {";
    first = true;
    trade_pool_.ForEachWorker([&](int shard, const TradeReportWorker& w) {
        for (const auto& sym : w.Symbols()) {
            f << (first ? "\n" : ",\n") << "    \"" << sym << "\": " << shard;
            first = false;
        }
    });
    f << "\n  },\n  \"breaks\": {";
    first = true;
    break_pool_.ForEachWorker([&](int shard, const TradeBreakWorker& w) {
        for (const auto& sym : w.Symbols()) {
            f << (first ? "\n" : ",\n") << "    \"" << sym << "\": " << shard;
            first = false;
        }
    });
    f << "\n  },\n  \"official_prices\": {";
    first = true;
    official_pool_.ForEachWorker([&](int shard, const OfficialPriceWorker& w) {
        for (const auto& sym : w.Symbols()) {
            f << (first ? "\n" : ",\n") << "    \"" << sym << "\": " << shard;
            first = false;
        }
    });
    f << "\n  }\n}\n";
}

void TopsProcessor::WriteToParquet() {
    quote_pool_.Close();
    trade_pool_.Close();
    break_pool_.Close();
    official_pool_.Close();
    WriteShardMap();
}
