//
//  MDUserClient.cpp
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#define kClassName "MDUserClient"
#define super IOUserClient

#include "MDUserClient.h"
#include "MDMainClass.h"

OSDefineMetaClassAndStructors(com_aqnichol_MemDisk_UserClient, IOUserClient)

const IOExternalMethodDispatch MDUserClient::gMethods[6] = {
    {(IOExternalMethodAction)&MDUserClient::gOpenCall,
		0,
		0,
		0,
		0},
    {(IOExternalMethodAction)&MDUserClient::gCloseCall,
		0,
		0,
		0,
		0},
    {(IOExternalMethodAction) &MDUserClient::gGetCount,
		0,
		0,
		1,
		0},
    {(IOExternalMethodAction) &MDUserClient::gGetName,
		1,
		0,
		0,
		sizeof(MDInfoNameStruct)},
    {(IOExternalMethodAction) &MDUserClient::gRemove,
		1,
		0,
		0,
		0},
    {(IOExternalMethodAction) &MDUserClient::gAdd,
		0,
		sizeof(MDRegisterStruct),
		1,
		0}
};

IOReturn MDUserClient::externalMethod(uint32_t selector, IOExternalMethodArguments * arguments,
                                      IOExternalMethodDispatch * dispatch, OSObject * target, void * reference) {
	if (selector >= 6) return super::externalMethod(selector, arguments, dispatch, target, reference);
    
    dispatch = (IOExternalMethodDispatch *)&gMethods[selector];
        
    if (!target) {
        target = this;
    }
    
	return super::externalMethod(selector, arguments, dispatch, target, reference);
}

#pragma mark - Lifecycle -

void MDUserClient::stop(IOService * provider) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    super::stop(provider);
}

bool MDUserClient::start(IOService * provider) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    if (!super::start(provider)) return false;
    providerObject = OSDynamicCast(MDMainClass, provider);
    return providerObject != NULL;
}

bool MDUserClient::initWithTask(task_t owningTask, void * securityToken, UInt32 type, OSDictionary * properties) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    providerObject = NULL;
    return super::initWithTask(owningTask, securityToken, type, properties);
}

IOReturn MDUserClient::clientClose() {
    this->closeCall();
    super::terminate((IOOptionBits)0);
    return kIOReturnSuccess;
}

IOReturn MDUserClient::clientDied() {
    this->closeCall();
    return super::clientDied();
}

#pragma mark - Static -

// static calls
IOReturn MDUserClient::gOpenCall(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->openCall();
}

IOReturn MDUserClient::gCloseCall(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->closeCall();
}

IOReturn MDUserClient::gGetCount(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->getCount(arguments->scalarOutput);
}

IOReturn MDUserClient::gGetName(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->getName(arguments->scalarInput[0], (MDInfoNameStruct *)arguments->structureOutput);
}

IOReturn MDUserClient::gRemove(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->remove(arguments->scalarInput[0]);
}

IOReturn MDUserClient::gAdd(MDUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->add((MDRegisterStruct *)arguments->structureInput, arguments->scalarOutput);
}

#pragma mark - Controlled Methods -

// methods for static calls
IOReturn MDUserClient::openCall() {
    IOLog("%s::%s() %p\n", kClassName, __FUNCTION__, providerObject);
    if (!providerObject) return kIOReturnNotAttached;
    if (!providerObject->open(this)) return kIOReturnExclusiveAccess;
    return kIOReturnSuccess;
}

IOReturn MDUserClient::closeCall() {
    IOLog("%s::%s() %p\n", kClassName, __FUNCTION__, providerObject);
    if (!providerObject) return kIOReturnNotAttached;
    if (providerObject->isOpen(this)) providerObject->close(this);
    return kIOReturnSuccess;
}

IOReturn MDUserClient::getCount(uint64_t * numOut) {
    IOLog("%s::%s() %p\n", kClassName, __FUNCTION__, providerObject);
    if (!providerObject) return kIOReturnNotAttached;
    if (!providerObject->isOpen(this)) return kIOReturnNotOpen;
    
    *numOut = (uint64_t)providerObject->deviceCount();
    IOLog("%s::%s() - there are %llu devices\n", kClassName, __FUNCTION__, *numOut);
    
    return kIOReturnSuccess;
}

IOReturn MDUserClient::getName(uint64_t index, MDInfoNameStruct * output) {
    IOLog("%s::%s() %p\n", kClassName, __FUNCTION__, providerObject);
    if (!providerObject) return kIOReturnNotAttached;
    if (!providerObject->isOpen(this)) return kIOReturnNotOpen;
    
    OSString * nameStr = providerObject->getDeviceName((int)index);
    if (!nameStr) {
        return kIOReturnNotFound;
    }
    strncpy(output->name, nameStr->getCStringNoCopy(), sizeof(output->name));
    nameStr->release();
    
    return kIOReturnSuccess;
}

IOReturn MDUserClient::remove(uint64_t index) {
    IOLog("%s::%s() %p\n", kClassName, __FUNCTION__, providerObject);
    if (!providerObject) return kIOReturnNotAttached;
    if (!providerObject->isOpen(this)) return kIOReturnNotOpen;
    
    if (!providerObject->removeDevice((int)index)) {
        return kIOReturnNotFound;
    }
    
    return kIOReturnSuccess;
}

IOReturn MDUserClient::add(MDRegisterStruct * reg, uint64_t * newCount) {
    IOLog("%s::%s() %p\n", kClassName, __FUNCTION__, providerObject);
    if (!providerObject) return kIOReturnNotAttached;
    if (!providerObject->isOpen(this)) return kIOReturnNotOpen;
    
    // register that sucker
    OSString * string = OSString::withCString(reg->name);
    bool result = providerObject->addDevice(string, (int)reg->size);
    string->release();
    
    *newCount = (uint64_t)providerObject->deviceCount();
    
    return result ? kIOReturnSuccess : kIOReturnNoMemory;
}
