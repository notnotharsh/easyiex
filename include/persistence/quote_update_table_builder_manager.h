#pragma once
#include <bits/stdc++.h>
#include "persistence/quote_update_table_builder.h"

class QuoteUpdateTableBuilderManager {
    public:
        QuoteUpdateTableBuilder& GetOrCreate(const std::string& symbol);
        void WriteAllBuilders();
    private:
        std::map<std::string, std::unique_ptr<QuoteUpdateTableBuilder>> builders;
};