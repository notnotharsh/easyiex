#include "tops_processor.h"

TopsProcessor::TopsProcessor(std::string pcap_name) : PcapProcessor(pcap_name) {}

void TopsProcessor::ProcessPacket(std::span<const u_char> view) {
    for (const auto& byte : view) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}