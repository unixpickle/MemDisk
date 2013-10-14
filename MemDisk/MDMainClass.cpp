/* add your code here */

#include "MDMainClass.h"

#define super IOService

OSDefineMetaClassAndStructors(com_aqnichol_MemDisk_Driver, IOService);

#define kClassName "MDMainClass"

bool MDMainClass::init(OSDictionary * dictionary) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    if (!super::init(dictionary)) return false;
    devicesLock = IOLockAlloc();
    devices = OSArray::withCapacity(1);
    return true;
}

void MDMainClass::free() {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    // release all devices
    devices->release();
    IOLockFree(devicesLock);
    super::free();
}

bool MDMainClass::start(IOService * provider) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    if (!super::start(provider)) return false;
    registerService();
    return true;
}

void MDMainClass::stop(IOService * provider) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    // release all devices
    while (devices->getCount() > 0) {
        this->removeDevice(0);
    }
    super::stop(provider);
}

#pragma mark - Device Management -

bool MDMainClass::addDevice(OSString * name, int blockCount) {
    IOLog("%s::%s()\n", kClassName, __FUNCTION__);
    // allocate the memory
    long long allocSize = (long long)blockCount * kMDBlockSize;
    long long maxSize = 1L << 31L;
    IOLog("%s::%s() - alloc size %lld max=%lld\n", kClassName, __FUNCTION__, allocSize, maxSize);
    if (allocSize >= maxSize) return false;
    vm_size_t memSize = (vm_size_t)allocSize;
    char * buffer = (char *)IOMalloc(memSize);
    IOLog("%s::%s() - buffer %p\n", kClassName, __FUNCTION__, buffer);
    if (!buffer) return false;
    IOLog("%s::%s() - creating block device \n", kClassName, __FUNCTION__);
    // create and register block device
    MDBlockDevice * device = OSTypeAlloc(MDBlockDevice);
    IOLog("%s::%s() - iniatializing\n", kClassName, __FUNCTION__);
    if (!device->init(name, blockCount, buffer)) {
        device->release();
        IOFree(buffer, memSize);
        return false;
    }
    IOLog("%s::%s() - attaching\n", kClassName, __FUNCTION__);
    if (!device->attach(this)) {
        device->release();
        return false;
    }
    IOLog("%s::%s() - registering\n", kClassName, __FUNCTION__);
    device->registerService();
    
    // update our internal list
    IOLockLock(devicesLock);
    if (!devices->setObject(device)) {
        IOLockUnlock(devicesLock);
        device->release();
        return false;
    }
    device->release();
    IOLockUnlock(devicesLock);
    
    return true;
}

bool MDMainClass::removeDevice(int index) {
    IOLockLock(devicesLock);
    MDBlockDevice * device = OSDynamicCast(MDBlockDevice, devices->getObject(index));
    if (!device) {
        IOLockUnlock(devicesLock);
        return false;
    }
    if (!device->terminate()) {
        IOLog("%s::%s() - failed\n", kClassName, __FUNCTION__);
        IOLockUnlock(devicesLock);
        return false;
    } else {
        IOLog("%s::%s() - success\n", kClassName, __FUNCTION__);
    }
    devices->removeObject(index);
    IOLockUnlock(devicesLock);
    return true;
}

int MDMainClass::deviceCount() {
    IOLockLock(devicesLock);
    int count = (int)devices->getCount();
    IOLockUnlock(devicesLock);
    return count;
}

OSString * MDMainClass::getDeviceName(int index) {
    IOLockLock(devicesLock);
    MDBlockDevice * device = OSDynamicCast(MDBlockDevice, devices->getObject(index));
    if (!device) {
        IOLockUnlock(devicesLock);
        return NULL;
    }
    OSString * result = device->getDeviceName();
    result->retain();
    IOLockUnlock(devicesLock);
    return result;
}

#pragma mark Private

void MDMainClass::removeDevice(MDBlockDevice * device) {
    IOLockLock(devicesLock);
    for (unsigned int i = 0; i < devices->getCount(); i++) {
        MDBlockDevice * dev = OSDynamicCast(MDBlockDevice, devices->getObject(i));
        if (!dev) continue;
        if (dev == device) {
            if (!device->terminate()) {
                IOLog("%s::%s() - failed\n", kClassName, __FUNCTION__);
                IOLockUnlock(devicesLock);
                break;
            } else {
                IOLog("%s::%s() - success\n", kClassName, __FUNCTION__);
            }
            devices->removeObject(i);
            break;
        }
    }
    IOLockUnlock(devicesLock);
}
