#include "Strategies.h"
using namespace dc;

// Allocator map setup
const std::map<const char *, SelectionStrategy::Constructor, SelectionStrategy::cmp_cstr> 
SelectionStrategy::allocatorMap =
{
    {"FirstFit", [](DcResourceManager *rm) -> SelectionStrategy * { new FirstFit(rm); }},
    {"BestFit", [](DcResourceManager *rm) -> SelectionStrategy * { new BestFit(rm); }},
    {"CostFit", [](DcResourceManager *rm) -> SelectionStrategy * { new CostFit(rm); }}
};

SelectionStrategy *SelectionStrategy::newStrategy(const char *type, DcResourceManager *rm)
{
    // Try to find the type
    auto iter = allocatorMap.find(type);
    
    // If not found
    if (iter == allocatorMap.end())
        return nullptr;
    
    // If found construct
    return iter->second(rm);
}
