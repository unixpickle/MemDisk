//
//  BlockDevice.h
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#ifndef __MemDisk__BlockDevice__
#define __MemDisk__BlockDevice__

#include <IOKit/storage/IOBlockStorageDevice.h>

#define MDBlockDevice com_aqnichol_MemDisk_BlockDevice
#define kMDBlockSize 512

class com_aqnichol_MemDisk_Driver;

class MDBlockDevice : public IOBlockStorageDevice {
    OSDeclareDefaultStructors(com_aqnichol_MemDisk_BlockDevice)
    
private:
    UInt64 blockCount;
    OSString * deviceName;
    char * buffer;
    
    com_aqnichol_MemDisk_Driver * mainProvider;
    
public:
    virtual bool init(OSString * name, UInt64 _blockCount, char * buffer, OSDictionary * dictionary = NULL);
	virtual void free();
	
    virtual void release();
    virtual void retain();
    
    virtual bool attach(IOService * provider);
    virtual void detach(IOService * provider);
    
    virtual bool start(IOService * provider);
    virtual void stop(IOService * provider);
        
    virtual OSString * getDeviceName();
    virtual OSString * getBSDName();
    
    // overloaded
    virtual IOReturn doEjectMedia(void);
    virtual IOReturn doFormatMedia(UInt64 byteCapacity);
    virtual UInt32 doGetFormatCapacities(UInt64 * capacities, UInt32 capacitiesMaxCount) const;
    virtual IOReturn doLockUnlockMedia(bool doLock);
    virtual IOReturn doSynchronizeCache(void);
    virtual char * getVendorString(void);
    virtual char * getProductString(void);
    virtual char * getRevisionString(void);
    virtual char * getAdditionalDeviceInfoString(void);
    virtual IOReturn reportBlockSize(UInt64 * blockSize);
    virtual IOReturn reportEjectability(bool * isEjectable);
    virtual IOReturn reportLockability(bool * isLockable);
    virtual IOReturn reportMaxValidBlock(UInt64 * maxBlock);
    virtual IOReturn reportMediaState(bool * mediaPresent, bool * changedState = 0);
    virtual IOReturn reportPollRequirements(bool * pollRequired, bool * pollIsExpensive);
    virtual IOReturn reportRemovability(bool * isRemovable);
    virtual IOReturn reportWriteProtection(bool * isWriteProtected);
    virtual IOReturn getWriteCacheState(bool * enabled);
    virtual IOReturn setWriteCacheState(bool enabled);
    virtual IOReturn doAsyncReadWrite(IOMemoryDescriptor * buffer,
                                      UInt64 block, UInt64 nblks,
                                      IOStorageAttributes * attributes,
                                      IOStorageCompletion * completion);
    virtual IOReturn requestIdle(void);
    virtual IOReturn doDiscard(UInt64 block, UInt64 nblks);
    virtual IOReturn doUnmap(IOBlockStorageDeviceExtent * extents,
                             UInt32 extentsCount,
                             UInt32 options = 0);
};

#endif /* defined(__MemDisk__BlockDevice__) */
