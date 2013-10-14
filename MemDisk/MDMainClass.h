/* add your code here */

#ifndef __MemDisk_MemDisk__
#define __MemDisk_MemDisk__

#define MDMainClass com_aqnichol_MemDisk_Driver
#include <IOKit/IOService.h>
#include <IOKit/IOBSD.h>
#include <IOKit/IOLib.h>
#include "MDBlockDevice.h"

class MDMainClass : public IOService {
    OSDeclareDefaultStructors(com_aqnichol_MemDisk_Driver)
private:
    OSArray * devices;
    IOLock * devicesLock;
    virtual void removeDevice(MDBlockDevice * device);
public:
    friend class MDBlockDevice;
    
    virtual bool init(OSDictionary * dictionary = NULL);
	virtual void free();
	
	virtual bool start(IOService * provider);
	virtual void stop(IOService * provider);
    
    // device management
    virtual bool addDevice(OSString * name, int blockCount);
    virtual bool removeDevice(int index);
    virtual int deviceCount();
    virtual OSString * getDeviceName(int index);
};

#endif
