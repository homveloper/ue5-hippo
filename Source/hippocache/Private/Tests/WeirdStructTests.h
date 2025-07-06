#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "HippocacheBlueprintLibrary.h"
#include "WeirdTestStructs.h"

/**
 * Collection of really weird struct test cases that users might try
 */
class FWeirdStructTestRunner
{
public:
    // Test empty struct behavior
    static bool TestEmptyStruct(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test massive struct with lots of data
    static bool TestHugeStruct(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test deeply nested structures
    static bool TestNestedStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test circular reference structures
    static bool TestCircularStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test structs with UObject pointers (should be dangerous)
    static bool TestObjectPointerStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test structs with delegates
    static bool TestDelegateStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test structs with special Unreal types
    static bool TestSpecialTypesStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test inheritance hierarchies
    static bool TestInheritanceStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test packed/bitfield structs
    static bool TestPackedStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test DataTable row structs
    static bool TestDataTableRowStructs(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test concurrent access to same struct
    static bool TestConcurrentStructAccess(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test struct modification after storage
    static bool TestStructModificationAfterStorage(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test struct with invalid memory patterns
    static bool TestStructMemoryPatterns(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test struct serialization edge cases
    static bool TestStructSerializationEdgeCases(UWorld* World, FAutomationSpecBase* TestSpec);
    
    // Test null/uninitialized struct data
    static bool TestUninitializedStructData(UWorld* World, FAutomationSpecBase* TestSpec);
    
private:
    // Helper functions
    static FHugeStruct CreateMassiveStruct();
    static FNestedStruct CreateDeeplyNestedStruct();
    static FCircularStruct CreateCircularStruct(int32 Depth);
    static FSpecialTypesStruct CreateSpecialTypesStruct();
};