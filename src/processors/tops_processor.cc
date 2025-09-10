#include "processors/tops_processor.h"

TopsProcessor::TopsProcessor(std::string pcap_name) : PcapProcessor(pcap_name) {}

void TopsProcessor::ProcessPacket(int packet_size, const std::byte *packet) const {
    // for (std::size_t i = 0; i < packet_size; i++) std::cout << std::hex << static_cast<int>(packet[i]) << " ";
    // ::cout << std::endl;
    // std::cout << std::endl;
}