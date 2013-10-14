//
//  ANMDConnection.m
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#import "ANMDConnection.h"

@implementation ANMDConnection

@synthesize connect;

- (id)init {
    if ((self = [super init])) {
        connect = IO_OBJECT_NULL;
    }
    return self;
}

- (BOOL)open {
    io_iterator_t iterator;
    kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                        IOServiceMatching("com_aqnichol_MemDisk_Driver"),
                                                        &iterator);
    if (result != KERN_SUCCESS) return NO;
    
    io_service_t service;
    while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
        result = IOServiceOpen(service, mach_task_self(), 0, &connect);
        if (result != KERN_SUCCESS) {
            return NO;
        } else {
            IOObjectRelease(iterator);
            if (IOConnectCallMethod(connect, MDFunctionIndexOpen, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL)) {
                IOServiceClose(connect);
                return NO;
            }
            return YES;
        }
    }
    IOObjectRelease(iterator);
    return NO;
}

- (void)close {
    IOConnectCallMethod(connect, MDFunctionIndexClose, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
    IOServiceClose(connect);
    connect = IO_OBJECT_NULL;
}

- (NSArray *)allDevices {
    if (connect == IO_OBJECT_NULL) return nil;
    uint64_t count;
    uint32_t outCount = 1;
    if (IOConnectCallScalarMethod(connect, MDFunctionIndexGetCount, NULL, 0, &count, &outCount) != KERN_SUCCESS) {
        return nil;
    }
    NSLog(@"count out is %llu, %d fields", count, outCount);
    NSMutableArray * array = [NSMutableArray array];
    for (int i = 0; i < (int)count; i++) {
        ANMDDevice * dev = [[ANMDDevice alloc] initWithConnection:self index:i];
        [array addObject:dev];
    }
    return [array copy];
}

- (ANMDDevice *)addDeviceWithName:(NSString *)name blocks:(uint32_t)blocks {
    if (connect == IO_OBJECT_NULL) return nil;
    MDRegisterStruct info;
    strncpy(info.name, [name UTF8String], sizeof(info.name));
    info.size = blocks;
    uint64_t outputNum = 0;
    uint32_t outputCount = 1;
    if (IOConnectCallMethod(connect, MDFunctionIndexAdd, NULL, 0, &info, sizeof(info), &outputNum, &outputCount, NULL, NULL) != KERN_SUCCESS) {
        return nil;
    }
    return [[ANMDDevice alloc] initWithConnection:self index:((int)outputNum - 1)];
}

- (void)removeDevice:(ANMDDevice *)device {
    if (connect == IO_OBJECT_NULL) return;
    uint64_t inputNum = (uint64_t)device.index;
    IOConnectCallScalarMethod(connect, MDFunctionIndexRemove, &inputNum, 1, NULL, 0);
}

@end
