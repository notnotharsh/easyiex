#include "pcap_processor.h"

PcapProcessor::PcapProcessor(std::string pcap_name) : pcap_name(pcap_name) {}

void PcapProcessor::InitialPass() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_handle = pcap_open_offline(pcap_name.c_str(), errbuf);

    if (pcap_handle == NULL) {
        throw std::ios_base::failure(errbuf);
    }

    while (pcap_next_ex(pcap_handle, &pcap_header, &pcap_data) >= 0) {
        const int eth_header_length = 14;
        if (pcap_header->len <= eth_header_length) continue;
        std::cout << "captured packet of length " << std::dec << pcap_header->len << std::endl;

        u_char first_ip_byte = pcap_data[eth_header_length];
        const int ip_header_length = static_cast<int>(first_ip_byte & 0x0F) * 4;

        const u_char transport_protocol = pcap_data[eth_header_length + 9];
        int transport_header_length = 0;

        if (transport_protocol == 6) {
            const u_char tcp_protocol_offset_byte = pcap_data[eth_header_length + ip_header_length + 12];
            transport_header_length = static_cast<int>(tcp_protocol_offset_byte & 0x0F) * 4;
        } else if (transport_protocol == 17) {
            transport_header_length = 8;
        } else throw std::runtime_error("protocol needs to be tcp or udp");

        int total_header_length = eth_header_length + ip_header_length + transport_header_length;
        std::cout << "using packet of length " << std::dec << pcap_header->len - total_header_length << std::endl;

        std::span<const std::byte> view{reinterpret_cast<const std::byte*>(pcap_data + total_header_length),  pcap_header->len - total_header_length};
        ProcessPacket(view);
    }

    pcap_close(pcap_handle);
}