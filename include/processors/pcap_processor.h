#pragma once
#include <bits/stdc++.h>
#include <pcap.h>

#include "utils/endian_utils.h"
#include "utils/time_utils.h"

template <typename Derived>
class PcapProcessor {
    public:
        struct MessageInfo {
            uint8_t version;
            uint8_t reserved;
            uint16_t message_protocol_id;
            uint32_t channel_id;
            uint32_t session_id;
            uint16_t payload_length;
            uint16_t message_count;
            uint64_t stream_offset;
            uint64_t first_message_sequence_number;
            uint64_t timestamp;
        };

        PcapProcessor(std::string pcap_name) : pcap_name_(pcap_name) {}

        void InitialPass() {
            char errbuf[PCAP_ERRBUF_SIZE];
            pcap_handle_ = pcap_open_offline(pcap_name_.c_str(), errbuf);

            if (!pcap_handle_) {
                throw std::ios_base::failure(errbuf);
            }

            while (pcap_next_ex(pcap_handle_, &pcap_header_, &pcap_data_) >= 0) {

                std::span<const std::byte> data{
                    reinterpret_cast<const std::byte*>(pcap_data_),
                    pcap_header_->len
                };

                const int eth_header_length = 14;
                if (data.size() <= eth_header_length) continue;

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

                int iex_header_length = 40;
                int total_header_length = eth_header_length + ip_header_length + transport_header_length + iex_header_length;

                std::span<const std::byte> iex_header = data.subspan(eth_header_length + ip_header_length + transport_header_length, iex_header_length);
                MessageInfo info = ProcessHeader(iex_header);

                if (data.size() <= total_header_length) continue;
                std::span<const std::byte> packet = data.subspan(total_header_length);
                static_cast<Derived*>(this)->ProcessPacket(packet);
            }
            pcap_close(pcap_handle_);
            static_cast<Derived*>(this)->WriteToParquet();
        }

        MessageInfo ProcessHeader(std::span<const std::byte> packet) const {
            MessageInfo info;
            info.version = ReadLittleEndian<uint8_t>(packet, 0);
            info.reserved = ReadLittleEndian<uint8_t>(packet, 1);
            info.message_protocol_id = ReadLittleEndian<uint16_t>(packet, 2);
            info.channel_id = ReadLittleEndian<uint32_t>(packet, 4);
            info.session_id = ReadLittleEndian<uint32_t>(packet, 8);
            info.payload_length = ReadLittleEndian<uint16_t>(packet, 12);
            info.message_count = ReadLittleEndian<uint16_t>(packet, 14);
            info.stream_offset = ReadLittleEndian<int64_t>(packet, 16);
            info.first_message_sequence_number = ReadLittleEndian<int64_t>(packet, 24);
            info.timestamp = ReadLittleEndian<int64_t>(packet, 32);
            return info;
        }

    private:
        std::string pcap_name_;
        pcap_t *pcap_handle_;
        pcap_pkthdr *pcap_header_;
        const u_char *pcap_data_;
};
