//
//  ANAppDelegate.h
//  MemDiskController
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ANMDConnection.h"

@interface ANAppDelegate : NSObject <NSApplicationDelegate, NSTextFieldDelegate, NSTableViewDataSource> {
    IBOutlet NSTextField * sizeInput;
    IBOutlet NSStepper * sizeStepper;
    IBOutlet NSTextField * nameInput;
    IBOutlet NSTableView * tableView;
    ANMDConnection * connection;
}

@property (assign) IBOutlet NSWindow * window;

- (IBAction)addPressed:(id)sender;
- (IBAction)stepperChanged:(id)sender;
- (IBAction)sizeChanged:(id)sender;
- (IBAction)deletePressed:(id)sender;

@end
