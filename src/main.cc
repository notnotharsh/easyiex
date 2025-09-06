#include <bits/stdc++.h>
#include <pcap.h>

#include "tops_processor.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::invalid_argument("usage: pcap_processor [pcap file]");
    }

    std::string pcap_name(argv[1]);
    TopsProcessor tops_processor(pcap_name);
    tops_processor.InitialPass();
}