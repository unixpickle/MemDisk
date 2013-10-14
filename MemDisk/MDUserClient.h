//
//  MDUserClient.h
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#ifndef __MemDisk__MDUserClient__
#define __MemDisk__MDUserClient__

#include <IOKit/IOUserClient.h>
#include "MDMainClass.h"
#include "MDStructs.h"

#define MDUserClient com_aqnichol_MemDisk_UserClient

class MDUserClient : public IOUserClient {
    OSDeclareDefaultStructors(com_aqnichol_MemDisk_UserClient);
private:
    MDMainClass * providerObject;
    static const IOExternalMethodDispatch gMethods[6];
public:
    // IOUserClient methods
    virtual void stop(IOService * provider);
    virtual bool start(IOService * provider);
    
    virtual IOReturn clientClose();
    virtual IOReturn clientDied();
    
	virtual bool initWithTask(task_t owningTask, void * securityToken, UInt32 type, OSDictionary * properties);
    virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments * arguments,
									IOExternalMethodDispatch * dispatch, OSObject * target, void * reference);
    
    // static calls
    static IOReturn gOpenCall(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gCloseCall(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gGetCount(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gGetName(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gRemove(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gAdd(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    
    // methods for static calls
    virtual IOReturn openCall();
    virtual IOReturn closeCall();
    virtual IOReturn getCount(uint64_t * numOut);
    virtual IOReturn getName(uint64_t index, MDInfoNameStruct * output);
    virtual IOReturn remove(uint64_t index);
    virtual IOReturn add(MDRegisterStruct * reg, uint64_t * newCount);
};

#endif /* defined(__MemDisk__MDUserClient__) */
