#include <bits/stdc++.h>
#include <pcap.h>

enum class PcapProcessorReturn {
    kOk = 0,
    kBadArgument,
    kOpenFile
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "usage: pcap_processor [pcap file]" << std::endl;
        return static_cast<int>(PcapProcessorReturn::kBadArgument);
    }

    std::string pcap_name(argv[1]);
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap_handle = pcap_open_offline(pcap_name.c_str(), errbuf);

    if (pcap_handle == NULL) {
        std::cerr << errbuf << std::endl;
        return static_cast<int>(PcapProcessorReturn::kOpenFile);
    }

    pcap_pkthdr *pcap_header;
    const u_char *pcap_data;

    while (pcap_next_ex(pcap_handle, &pcap_header, &pcap_data) >= 0) {
        std::cout << "captured packet of length " << pcap_header->len << std::endl;
    }

    pcap_close(pcap_handle);
    return static_cast<int>(PcapProcessorReturn::kOk);
}