#ifndef SIMCAN_EX_DC_SELECTION_STRATEGIES
#define SIMCAN_EX_DC_SELECTION_STRATEGIES

class DataCentreManagerBase; // forward declaration
class SM_UserVM_Cost;        // forward declaration
class DcResourceManager;     // forward declaration
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

        typedef SelectionStrategy *(*Constructor)(DataCentreManagerBase *m);
        const static std::map<const char *, Constructor, cmp_cstr> allocatorMap;

    protected:
        DataCentreManagerBase *manager;
        DcResourceManager *resourceManager;
        SelectionStrategy(DataCentreManagerBase *m);

    public:
        static SelectionStrategy *newStrategy(const char *type, DataCentreManagerBase *m);
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs) = 0;
    };

    class FirstFit : public SelectionStrategy
    {
    public:
        FirstFit(DataCentreManagerBase *m) : SelectionStrategy(m) {}
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs);
    };

    class BestFit : public SelectionStrategy
    {
    public:
        BestFit(DataCentreManagerBase *m) : SelectionStrategy(m) {}
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs);
    };

    class CostFit : public FirstFit
    {
    public:
        CostFit(DataCentreManagerBase *m) : FirstFit(m) {}
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs);
        virtual uint32_t selectNode(SM_UserVM_Cost *userVM_Rq, const VirtualMachine &vmSpecs);
    };
}

#endif /* SIMCAN_EX_DC_SELECTION_STRATEGIES */
