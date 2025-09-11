#pragma once
#include <bits/stdc++.h>
#include <pcap.h>

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

        PcapProcessor(std::string pcap_name);
        void InitialPass();
        virtual void ProcessPacket(const std::byte *packet) const = 0;
        MessageInfo ProcessHeader(const std::byte *packet) const;
    
    private:
        std::string pcap_name_;
        pcap_t *pcap_handle_;
        pcap_pkthdr *pcap_header_;
        const u_char *pcap_data_;
};