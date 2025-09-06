#pragma once
#include <bits/stdc++.h>
#include <pcap.h>

class PcapProcessor {
    public:
        PcapProcessor(std::string pcap_name);
        void InitialPass();
        virtual void ProcessPacket(std::span<const u_char> view) = 0;

    protected:
        std::string GetName() const { return pcap_name; }
        pcap_t *GetHandle() const { return pcap_handle; }
        pcap_pkthdr *GetPcapHeader() const { return pcap_header; }
        const u_char *data() const { return pcap_data; }
    
    private:
        std::string pcap_name;
        pcap_t *pcap_handle;
        pcap_pkthdr *pcap_header;
        const u_char *pcap_data;
};