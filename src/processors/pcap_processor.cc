#include "processors/pcap_processor.h"
#include "utils/endian_utils.h"
#include "utils/time_utils.h"

PcapProcessor::PcapProcessor(std::string pcap_name) : pcap_name_(pcap_name) {}

void PcapProcessor::InitialPass() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_handle_ = pcap_open_offline(pcap_name_.c_str(), errbuf);

    if (pcap_handle_ == NULL) {
        throw std::ios_base::failure(errbuf);
    }

    while (pcap_next_ex(pcap_handle_, &pcap_header_, &pcap_data_) >= 0) {

        const std::byte *data = reinterpret_cast<const std::byte*>(pcap_data_);

        const int eth_header_length = 14;
        if (pcap_header_->len <= eth_header_length) continue;

        const std::byte first_ip_byte = data[eth_header_length];
        const int ip_header_length = static_cast<int>(first_ip_byte & static_cast<std::byte>(0x0F)) * 4;

        const std::byte transport_protocol = data[eth_header_length + 9];
        int transport_header_length = 0;

        if (transport_protocol == static_cast<std::byte>(6)) {
            const std::byte tcp_protocol_offset_byte = data[eth_header_length + ip_header_length + 12];
            transport_header_length = static_cast<int>(tcp_protocol_offset_byte & static_cast<std::byte>(0x0F)) * 4;
        } else if (transport_protocol == static_cast<std::byte>(17)) {
            transport_header_length = 8;
        } else throw std::runtime_error("protocol needs to be tcp or udp");

        MessageInfo info = ProcessHeader(data + eth_header_length + ip_header_length + transport_header_length);

        int iex_header_length = 40;
        int total_header_length = eth_header_length + ip_header_length + transport_header_length + iex_header_length;
        const std::byte *packet = data + total_header_length;

        ProcessPacket(packet);
    }
    pcap_close(pcap_handle_);
}

PcapProcessor::MessageInfo PcapProcessor::ProcessHeader(const std::byte *packet) const {
    MessageInfo info;
    info.version = read_little_endian<uint8_t>(packet + 0);
    info.reserved = read_little_endian<uint8_t>(packet + 1);
    info.message_protocol_id = read_little_endian<uint16_t>(packet + 2);
    info.channel_id = read_little_endian<uint32_t>(packet + 4);
    info.session_id = read_little_endian<uint32_t>(packet + 8);
    info.payload_length = read_little_endian<uint16_t>(packet + 12);
    info.message_count = read_little_endian<uint16_t>(packet + 14);
    info.stream_offset = read_little_endian<int64_t>(packet + 16);
    info.first_message_sequence_number = read_little_endian<int64_t>(packet + 24);
    info.timestamp = read_little_endian<int64_t>(packet + 32);
    return info;
}