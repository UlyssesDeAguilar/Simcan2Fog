import inet.networklayer.common.L3Address;

//
// Class that contains peer information.
// The base protocol that manages connections needs nothing more than the
// base IP addresses. Specific implementations could require additional fields
// and should extend from this class.
// 
// @author Tomás Daniel Expósito Torre
// @version 1.0
class Peer
{
    inet::L3Address ipAddress;
};
