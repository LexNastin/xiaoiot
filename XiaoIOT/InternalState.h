#ifndef InternalState_h
#define InternalState_h

#include "Constants.h"

struct InternalState {
  public:
    bool getLocked();
    char* getPassword();

    void setLocked();
    void setPassword();

    InternalState();
};

#endif