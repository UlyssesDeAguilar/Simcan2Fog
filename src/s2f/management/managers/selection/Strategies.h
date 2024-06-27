#ifndef SIMCAN_EX_DC_SELECTION_STRATEGIES
#define SIMCAN_EX_DC_SELECTION_STRATEGIES

class ResourceManager;       // forward declaration

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

        typedef SelectionStrategy *(*Constructor)(void);
        const static std::map<const char *, Constructor, cmp_cstr> allocatorMap;

    protected:
        ResourceManager *resourceManager{};

    public:
        static SelectionStrategy *newStrategy(const char *type);
        void setResourceManager(ResourceManager *rm) { resourceManager = rm; }
        virtual uint32_t selectNode(const VirtualMachine &vmSpecs) = 0;
    };

    class FirstFit : public SelectionStrategy
    {
    public:
        FirstFit() : SelectionStrategy() {}
        virtual uint32_t selectNode(const VirtualMachine &vmSpecs);
    };

    class BestFit : public SelectionStrategy
    {
    public:
        BestFit() : SelectionStrategy() {}
        virtual uint32_t selectNode(const VirtualMachine &vmSpecs);
    };

    /*class CostFit : public FirstFit
    {
    public:
        CostFit(DataCentreManagerBase *m) : FirstFit(m) {}
        virtual uint32_t selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs);
        virtual uint32_t selectNode(SM_UserVM_Cost *userVM_Rq, const VirtualMachine &vmSpecs);
    };*/
}

#endif /* SIMCAN_EX_DC_SELECTION_STRATEGIES */
