#include "s2f/management/managers/ResourceManager.h"
#include "Strategies.h"

using namespace dc;

// Allocator map setup
const std::map<const char *, SelectionStrategy::Constructor, SelectionStrategy::cmp_cstr>
    SelectionStrategy::allocatorMap =
        {
            {"FirstFit", []() -> SelectionStrategy *
             { return new FirstFit(); }},
            {"BestFit", []() -> SelectionStrategy *
             { return new BestFit(); }}
            /*{"CostFit", [](DataCentreManagerBase *m) -> SelectionStrategy *
             { return new CostFit(m); }}*/};

SelectionStrategy *SelectionStrategy::newStrategy(const char *type)
{
    // Try to find the type
    auto iter = allocatorMap.find(type);

    // If not found
    if (iter == allocatorMap.end())
        return nullptr;

    // If found construct
    return iter->second();
}
