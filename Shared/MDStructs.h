//
//  MDStructs.h
//  MemDisk
//
//  Created by Alex Nichol on 10/13/13.
//  Copyright (c) 2013 Alex Nichol. All rights reserved.
//

#ifndef MemDisk_MDStructs_h
#define MemDisk_MDStructs_h

typedef struct {
    char name[96];
} MDInfoNameStruct;

typedef struct {
    char name[96];
    uint32_t size;
} MDRegisterStruct;

typedef enum {
    MDFunctionIndexOpen,
    MDFunctionIndexClose,
    MDFunctionIndexGetCount,
    MDFunctionIndexGetName,
    MDFunctionIndexRemove,
    MDFunctionIndexAdd
} MDFunctionIndex;

#endif
