#include "pcap_processor.h"

PcapProcessor::PcapProcessor(std::string pcap_name) : pcap_name(pcap_name) {}

void PcapProcessor::InitialPass() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_handle = pcap_open_offline(pcap_name.c_str(), errbuf);

    if (pcap_handle == NULL) {
        throw std::ios_base::failure(errbuf);
    }

    while (pcap_next_ex(pcap_handle, &pcap_header, &pcap_data) >= 0) {
        std::cout << "captured packet of length " << pcap_header->len << std::endl;
        std::span<const u_char> view(pcap_data, pcap_header->len);
        ProcessPacket(view);
    }

    pcap_close(pcap_handle);
}