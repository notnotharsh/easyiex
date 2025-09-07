#pragma once
#include <bits/stdc++.h>
#include <pcap.h>

class PcapProcessor {
    public:
        PcapProcessor(std::string pcap_name);
        void InitialPass();
        virtual void ProcessPacket(std::span<const std::byte> view) = 0;
    
    private:
        std::string pcap_name;
        pcap_t *pcap_handle;
        pcap_pkthdr *pcap_header;
        const u_char *pcap_data;
};