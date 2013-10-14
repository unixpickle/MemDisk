//
//  ANAppDelegate.m
//  MemDiskController
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#import "ANAppDelegate.h"

@implementation ANAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    sizeStepper.integerValue = 10000;
    sizeInput.stringValue = @"10000";
    nameInput.stringValue = @"Unnamed";
    connection = [[ANMDConnection alloc] init];
    if (![connection open]) {
        NSRunAlertPanel(@"Connection failed", @"Make sure the MemDisk kernel extension is running",
                        @"OK", nil, nil);
    }
    [tableView reloadData];
}

- (void)applicationDidBecomeActive:(NSNotification *)notification {
    []
}

- (IBAction)addPressed:(id)sender {
    if (![connection addDeviceWithName:nameInput.stringValue blocks:(int)sizeStepper.integerValue]) {
        NSRunAlertPanel(@"Add failed", @"There was an error adding your disk. It may either be too small or too large...", @"OK", nil, nil);
        return;
    }
    [tableView reloadData];
}

- (IBAction)stepperChanged:(id)sender {
    sizeInput.stringValue = [NSString stringWithFormat:@"%llu", (unsigned long long)sizeStepper.integerValue];
}

- (IBAction)sizeChanged:(id)sender {
    sizeStepper.integerValue = sizeInput.integerValue;
}

- (IBAction)deletePressed:(id)sender {
    NSLog(@"delete pressed");
    if ([tableView selectedRow] < 0) return;
    ANMDDevice * device = [connection allDevices][[tableView selectedRow]];
    [connection removeDevice:device];
    [tableView reloadData];
}

- (void)controlTextDidChange:(NSNotification *)obj {
    [self sizeChanged:nil];
}

#pragma mark - Table View -

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    NSLog(@"getting number of rows");
    NSInteger count = [connection allDevices].count;
    NSLog(@"count %d", (int)count);
    return count;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
    NSLog(@"getting object value %d", (int)rowIndex);
    ANMDDevice * device = [connection allDevices][rowIndex];
    NSLog(@"device %@", device);
    NSLog(@"name %@", [device getName]);
    return [device getName];
}

@end
