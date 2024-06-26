#include "../../../management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"
#include "../../../management/DataCentreManagers/ResourceManager/ResourceManager.h"
#include "../../../management/DataCentreManagers/Selection/Strategies.h"

using namespace dc;

SelectionStrategy::SelectionStrategy(DataCentreManagerBase *m) : manager(m), resourceManager(m->resourceManager) {}

// Allocator map setup
const std::map<const char *, SelectionStrategy::Constructor, SelectionStrategy::cmp_cstr>
    SelectionStrategy::allocatorMap =
        {
            {"FirstFit", [](DataCentreManagerBase *m) -> SelectionStrategy *
             { return new FirstFit(m); }},
            {"BestFit", [](DataCentreManagerBase *m) -> SelectionStrategy *
             { return new BestFit(m); }},
            {"CostFit", [](DataCentreManagerBase *m) -> SelectionStrategy *
             { return new CostFit(m); }}};

SelectionStrategy *SelectionStrategy::newStrategy(const char *type, DataCentreManagerBase *m)
{
    // Try to find the type
    auto iter = allocatorMap.find(type);

    // If not found
    if (iter == allocatorMap.end())
        return nullptr;

    // If found construct
    return iter->second(m);
}
