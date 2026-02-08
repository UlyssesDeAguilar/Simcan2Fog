cplusplus{{
#include "s2f/architecture/dns/RecordData.h"
}}

namespace s2f::dns;

class RecordData {
    @existingClass;
    @descriptor(readonly);
    uint16_t resourceDataLength; // Size of the RecordData field, in bytes (2 Bytes)
    uint8_t resourceData[] @getter(getResourceDataByte) @sizeGetter(getResourceDataLength); // Data portion of the resource record
}