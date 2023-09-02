#ifndef _SM_FOG_IO_MESSAGE_H
#define _SM_FOG_IO_MESSAGE_H

#include "SM_FogIO_m.h"

class SM_FogIO : public SM_FogIO_Base{
public:
    SM_FogIO(const char *name=nullptr, short kind=0);
    SM_FogIO(const SM_FogIO& other);
    virtual ~SM_FogIO();
    
    SM_FogIO& operator=(const SM_FogIO& other);
    virtual SM_FogIO *dup() const;

    /**
     * @brief Set the request size. It also updates the byte length
     * field for the cPacket base (coming from SIMCAN_Message).
     * 
     * @param requestSize In MB
     */
    virtual void setRequestSize(double requestSize);
};

#endif