#ifndef SIMCAN_EX_DC_SELECTION_STRATEGIES
#define SIMCAN_EX_DC_SELECTION_STRATEGIES

#include "Management/DataCentreManagers/ResourceManager/ResourceManager.h"

namespace dc
{
    class SelectionStrategy
    {
    private:
        struct cmp_cstr
        {
            bool operator()(char const *a, char const *b) const
            {
                return std::strcmp(a, b) < 0;
            }
        };

        typedef SelectionStrategy *(*Constructor)(DcResourceManager *rm);
        DcResourceManager *resourceManager;
        const static std::map<const char *, Constructor, cmp_cstr> allocatorMap;

    protected:
        SelectionStrategy(DcResourceManager *rm) : resourceManager(rm) {}

    public:
        SelectionStrategy *newStrategy(const char *type, DcResourceManager *rm);
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs) = 0;
    };

    class FirstFit : SelectionStrategy
    {
    public:
        FirstFit(DcResourceManager *rm) : SelectionStrategy(rm) {}
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs);
    };

    class BestFit : SelectionStrategy
    {
    public:
        BestFit(DcResourceManager *rm) : SelectionStrategy(rm) {}
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs);
    };

    class CostFit : FirstFit
    {
    public:
        CostFit(DcResourceManager *rm) : FirstFit(rm) {}
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs);
    };
}

#endif /* SIMCAN_EX_DC_SELECTION_STRATEGIES */
