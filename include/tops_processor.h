#pragma once
#include "pcap_processor.h"

class TopsProcessor : public PcapProcessor {
    public:
        TopsProcessor(std::string pcap_name);
        void ProcessPacket(std::span<const std::byte> view) override;
};