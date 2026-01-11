#ifndef __SIMCAN2FOG_TOOLS_H_
#define __SIMCAN2FOG_TOOLS_H_

#include <cstdint>

namespace s2f
{
    /**
     * Set the bit in a byte at the given position to the given value
     *
     * @param byte The byte to set the bit in
     * @param position The position of the bit to set indexed at 0
     * @param value The value to set the bit to
     * @return New byte with the bit set
     */
    constexpr uint8_t setBitInByte(uint8_t byte, uint8_t position, bool value)
    {
        uint8_t mask = value << position;
        return (byte & ~(1 << position)) | mask;
    }

    /**
     * Set the bit in a byte at the given position to the given value
     *
     * @param byte The byte to set the bit in
     * @param position The position of the bit to set indexed at 0
     * @param value The value to set the bit to
     */
    constexpr void setBitInByteInPlace(uint8_t &byte, uint8_t position, bool value)
    {
        uint8_t mask = value << position;
        byte = (byte & ~(1 << position)) | mask;
    }

    /**
     * Get the bit in a byte at the given position
     *
     * @param byte The byte to get the bit from
     * @param position The position of the bit to get indexed at 0
     * @return The value of the bit
     */
    constexpr bool getBitInByte(uint8_t byte, uint8_t position)
    {
        return (byte >> position) & 1;
    }
}
#endif /*__SIMCAN2FOG_TOOLS_H_ */