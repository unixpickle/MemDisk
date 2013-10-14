//
//  ANMDDevice.m
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#import "ANMDDevice.h"
#import "ANMDConnection.h"

@implementation ANMDDevice

@synthesize connection;
@synthesize index;

- (id)initWithConnection:(id)aConn index:(int)anIndex {
    if ((self = [super init])) {
        connection = aConn;
        index = anIndex;
    }
    return self;
}

- (NSString *)getName {
    MDInfoNameStruct info;
    size_t sizeOut = sizeof(info);
    io_connect_t connect = [(ANMDConnection *)connection connect];
    uint64_t indexValue = (uint64_t)index;
    if (IOConnectCallMethod(connect, MDFunctionIndexGetName, &indexValue, 1, NULL, 0, NULL, NULL, (void *)&info, &sizeOut) != KERN_SUCCESS) {
        return nil;
    }
    return [NSString stringWithUTF8String:info.name];
}

@end
