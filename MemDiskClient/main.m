//
//  main.m
//  MemDiskClient
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <IOKit/IOKitLib.h>
#include "MDStructs.h"

void createSampleDisk(io_connect_t connect);

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // here we create a connection
        io_iterator_t iterator;
        kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                            IOServiceMatching("com_aqnichol_MemDisk_Driver"),
                                                            &iterator);
        if (result != KERN_SUCCESS) {
            NSLog(@"failure to get matching services");
            return 1;
        }
        
        io_service_t service;
        while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
            NSLog(@"found a match");
            // this is probably the one and only
            io_connect_t connect;
            result = IOServiceOpen(service, mach_task_self(), 0, &connect);
            if (result != KERN_SUCCESS) {
                NSLog(@"IOServiceOpen() failed");
            } else {
                NSLog(@"creating sample disk");
                // pass it off
                createSampleDisk(connect);
                IOServiceClose(connect);
            }
        }
        IOObjectRelease(iterator);
    }
    return 0;
}

void createSampleDisk(io_connect_t connect) {
    IOConnectCallMethod(connect, 0, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
}
