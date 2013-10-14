//
//  ANMDConnection.h
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import "ANMDDevice.h"
#import "MDStructs.h"

@interface ANMDConnection : NSObject {
    io_connect_t connect;
}

@property (readonly) io_connect_t connect;

- (BOOL)open;
- (void)close;

- (NSArray *)allDevices;
- (ANMDDevice *)addDeviceWithName:(NSString *)name blocks:(uint32_t)blocks;
- (void)removeDevice:(ANMDDevice *)device;

@end
