#pragma once
#include "processors/pcap_processor.h"

class TopsProcessor : public PcapProcessor {
    public:
        TopsProcessor(std::string pcap_name);
        void ProcessPacket(int packet_size, const std::byte *packet) const override;
};