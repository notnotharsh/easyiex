#include "persistence/quote_update_table_builder_manager.h"
#include "persistence/quote_update_table_builder.h"

QuoteUpdateTableBuilder& QuoteUpdateTableBuilderManager::GetOrCreate(const std::string& symbol) {
    auto it = builders.find(symbol);
    if (it != builders.end()) return *(it->second);

    auto [new_it, inserted] = builders.emplace(
        symbol, 
        std::make_unique<QuoteUpdateTableBuilder>(symbol)
    );
    return *(builders[symbol]);
}

void QuoteUpdateTableBuilderManager::WriteAllBuilders() {
    for (auto& [symbol, builder_ptr] : builders) {
        builder_ptr->WriteToParquet();
    }
}