//
//  ANMDDevice.h
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ANMDDevice : NSObject {
    __weak id connection;
    int index;
}

@property (readonly) id connection;
@property (readonly) int index;

- (id)initWithConnection:(id)aConn index:(int)anIndex;
- (NSString *)getName;

@end
