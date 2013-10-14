//
//  BlockDevice.cpp
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#include "MDBlockDevice.h"
#include "MDMainClass.h"

#define super IOBlockStorageDevice
#define kClassName "MDBlockDevice"

OSDefineMetaClassAndStructors(com_aqnichol_MemDisk_BlockDevice, IOBlockStorageDevice)

bool MDBlockDevice::init(OSString * _name, UInt64 _blockCount, char * _buffer, OSDictionary * dictionary) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    if (!super::init(dictionary)) return false;
    blockCount = _blockCount;
    deviceName = _name;
    deviceName->retain();
    buffer = _buffer;
    return true;
}

void MDBlockDevice::release() {
    IOLog("%s::%s() = %d\n", kClassName, __FUNCTION__, this->getRetainCount());
    super::release();
}

void MDBlockDevice::retain() {
    IOLog("%s::%s() = %d\n", kClassName, __FUNCTION__, this->getRetainCount());
    super::retain();
}

void MDBlockDevice::free() {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    deviceName->release();
    IOFree(buffer, kMDBlockSize * blockCount);
    super::free();
}

bool MDBlockDevice::attach(IOService * provider) {
    IOLog("%s::%s() - name %s\n", kClassName, __FUNCTION__, provider->getName());
    if (!super::attach(provider)) return false;
    if (OSDynamicCast(MDMainClass, provider) == NULL) {
        return false;
    }
    mainProvider = OSDynamicCast(MDMainClass, provider);
    IOLog("%s::%s() - success %s\n", kClassName, __FUNCTION__, provider->getName());
    return true;
}

void MDBlockDevice::detach(IOService * provider) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    if (provider == mainProvider) {
        mainProvider = NULL;
    }
    super::detach(provider);
}

bool MDBlockDevice::start(IOService * provider) {
    IOLog("%s::%s() - name %s\n", kClassName, __FUNCTION__, provider->getName());
    return super::start(provider);
}

void MDBlockDevice::stop(IOService * provider) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    super::stop(provider);
}

OSString * MDBlockDevice::getDeviceName() {
    return deviceName;
}

OSString * MDBlockDevice::getBSDName() {
    return OSDynamicCast(OSString, this->getProperty(kIOBSDNameKey));
}

#pragma mark - Overridden Media Methods -

IOReturn MDBlockDevice::doEjectMedia(void) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    if (mainProvider) {
        mainProvider->removeDevice(this);
    }
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::doFormatMedia(UInt64 byteCapacity)	{
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    return kIOReturnUnsupported;
}

UInt32 MDBlockDevice::doGetFormatCapacities(UInt64 * capacities,
                                            UInt32 capacitiesMaxCount) const {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    if (!capacities) return 1; // count query
    if (capacitiesMaxCount < 1) return 0; // invalid query
    
    // handle regular query
    capacities[0] = blockCount * kMDBlockSize;
    return 1;
}

IOReturn MDBlockDevice::doLockUnlockMedia(bool doLock) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    return kIOReturnUnsupported;
}

IOReturn MDBlockDevice::doSynchronizeCache(void) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    return kIOReturnUnsupported;
}

char * MDBlockDevice::getVendorString(void) {
    return (char *)"Jitsik";
}

char * MDBlockDevice::getProductString(void) {
    return (char *)deviceName->getCStringNoCopy();
}

char * MDBlockDevice::getRevisionString(void)	{
    return (char *)"1.0";
}

char * MDBlockDevice::getAdditionalDeviceInfoString(void)	{
    return (char *)"MDBlockDevice";
}

IOReturn MDBlockDevice::reportBlockSize(UInt64 * blockSize) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *blockSize = kMDBlockSize;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::reportEjectability(bool * isEjectable) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *isEjectable = true;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::reportLockability(bool * isLockable) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *isLockable = false;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::reportMaxValidBlock(UInt64 * maxBlock) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *maxBlock = blockCount - 1;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::reportMediaState(bool * mediaPresent, bool * changedState)	{
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *mediaPresent = mainProvider != NULL;
    *changedState = false;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::reportPollRequirements(bool * pollRequired, bool * pollIsExpensive) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *pollRequired = false;
    *pollIsExpensive = false;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::reportRemovability(bool * isRemovable) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *isRemovable = true;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::reportWriteProtection(bool * isWriteProtected) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *isWriteProtected = false;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::getWriteCacheState(bool * enabled) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    *enabled = false;
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::setWriteCacheState(bool enabled) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::doAsyncReadWrite(IOMemoryDescriptor * aBuffer,
                                         UInt64 block, UInt64 nblks,
                                         IOStorageAttributes * attributes,
                                         IOStorageCompletion * completion) {
    if (!mainProvider) return kIOReturnNotAttached;
    
    UInt64 offset = block * kMDBlockSize;
    UInt64 length = nblks * kMDBlockSize;
    if (aBuffer->getDirection() == kIODirectionIn) {
        aBuffer->writeBytes(0, &buffer[offset], length);
    } else if (aBuffer->getDirection() == kIODirectionOut) {
        aBuffer->readBytes(0, &buffer[offset], length);
    } else {
        return kIOReturnUnsupported;
    }
    completion->action(completion->target, completion->parameter, kIOReturnSuccess, length);
    return kIOReturnSuccess;
}

IOReturn MDBlockDevice::requestIdle(void) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    return kIOReturnUnsupported;
}

IOReturn MDBlockDevice::doDiscard(UInt64 block, UInt64 nblks) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    return kIOReturnUnsupported;
}

IOReturn MDBlockDevice::doUnmap(IOBlockStorageDeviceExtent * extents, UInt32 extentsCount, UInt32 options) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    return kIOReturnUnsupported;
}

