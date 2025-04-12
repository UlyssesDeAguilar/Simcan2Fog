#include "s2f/management/managers/Node.h"

using namespace omnetpp;

void Node::setInactive() { state ^= state; }
void Node::setActive()
{
    state ^= state;
    state |= ACTIVE;
}
void Node::setInUse()
{
    state ^= state;
    state |= IN_USE;
}
void Node::setMaxedOut()
{
    state ^= state;
    state |= MAXED_OUT;
}

bool Node::isActive() const { return state & ACTIVE; }

bool Node::inUse() const { return state & IN_USE; }

bool Node::isMaxedOut() const { return state & MAXED_OUT; }

std::ostream &operator<<(std::ostream &os, const Node &node)
{
    const char *state[4] = {"INACTIVE", "ACTIVE", "IN_USE", "MAXED_OUT"};
    return os << "Node [ address:" << node.address
              << ", state: " << state[node.state] << " ]";
}
