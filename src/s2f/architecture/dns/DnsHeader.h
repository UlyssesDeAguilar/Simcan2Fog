#include "s2f/core/tools.h"
#include "s2f/architecture/dns/DnsHeader_m.h"

namespace s2f::dns
{
    class DnsHeader : public DnsHeader_Base
    {
    private:
        void copy(const DnsHeader &other) {}

    public:
        DnsHeader() : DnsHeader_Base() {}
        DnsHeader(const DnsHeader &other) : DnsHeader_Base(other) { copy(other); }
        DnsHeader &operator=(const DnsHeader &other);
        virtual DnsHeader *dup() const override { return new DnsHeader(*this); }

        void setQueryOrResponseFlag(bool isResponse) { setBitInByteInPlace(flag_field_1, 7, isResponse); }
        bool getQueryOrResponseFlag() const { return getBitInByte(flag_field_1, 7); }

        void setOperationCode(OperationCode code);
        OperationCode getOperationCode() const;

        void setAuthoritativeAnswerFlag(bool isAuthoritativeAnswer) { setBitInByteInPlace(flag_field_1, 2, isAuthoritativeAnswer); }
        bool getAuthoritativeAnswerFlag() const {return getBitInByte(flag_field_1, 2); }

        void setTruncationFlag(bool isTruncated) {setBitInByteInPlace(flag_field_1, 1, isTruncated); }
        bool getTruncationFlag() const { return getBitInByte(flag_field_1, 1); }

        void setRecursionDesiredFlag(bool isRecursionDesired) { setBitInByteInPlace(flag_field_1, 0, isRecursionDesired); }
        bool getRecursionDesiredFlag() const { return getBitInByte(flag_field_1, 0); }

        void setRecursionAvailableFlag(bool isRecursionAvailable) { setBitInByteInPlace(flag_field_2, 7, isRecursionAvailable); }
        bool getRecursionAvailableFlag() const { return getBitInByte(flag_field_2, 7); }

        void setResponseCode(ReturnCode code);
        ReturnCode getResponseCode() const;
    };
}