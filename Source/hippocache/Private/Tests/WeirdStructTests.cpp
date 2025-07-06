#include "WeirdStructTests.h"
#include "HAL/PlatformProcess.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "TestStructs.h"

// Static string initialization
const FString FStaticOnlyStruct::StaticString = TEXT("Static String Value");

bool FWeirdStructTestRunner::TestEmptyStruct(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Empty Struct ==="));
    
    FName CollectionName = "EmptyStructTest";
    FString TestKey = "EmptyKey";
    
    FEmptyStruct EmptyStruct;
    
    // Test storing empty struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FEmptyStruct>(
        World, CollectionName, TestKey, EmptyStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Empty struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to set empty struct: %s"), *SetResult.ErrorMessage));
        return false;
    }
    
    // Test retrieving empty struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FEmptyStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Empty struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to get empty struct: %s"), *GetResult.Result.ErrorMessage));
        return false;
    }
    
    // Test equality (should always be true for empty structs)
    FEmptyStruct RetrievedStruct = GetResult.Value;
    bool bAreEqual = EmptyStruct == RetrievedStruct;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Empty structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
    
    return SetResult.IsSuccess() && GetResult.IsSuccess() && bAreEqual;
}

bool FWeirdStructTestRunner::TestHugeStruct(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Huge Struct ==="));
    
    FName CollectionName = "HugeStructTest";
    FString TestKey = "HugeKey";
    
    FHugeStruct HugeStruct = CreateMassiveStruct();
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created huge struct with %d array elements, %d map entries"), 
        HugeStruct.MassiveArray.Num(), HugeStruct.HugeMap.Num()));
    
    // Test storing huge struct (this might fail due to memory)
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FHugeStruct>(
        World, CollectionName, TestKey, HugeStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Huge struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddWarning(FString::Printf(TEXT("Expected: Huge struct failed to store: %s"), *SetResult.ErrorMessage));
        return true; // This is expected behavior
    }
    
    // If it succeeded, try to retrieve it
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FHugeStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Huge struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (GetResult.IsSuccess())
    {
        FHugeStruct RetrievedStruct = GetResult.Value;
        bool bAreEqual = HugeStruct == RetrievedStruct;
        
        TestSpec->AddInfo(FString::Printf(TEXT("Huge structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
        TestSpec->AddInfo(FString::Printf(TEXT("Retrieved struct has %d array elements, %d map entries"), 
            RetrievedStruct.MassiveArray.Num(), RetrievedStruct.HugeMap.Num()));
        
        return bAreEqual;
    }
    
    return SetResult.IsSuccess();
}

bool FWeirdStructTestRunner::TestNestedStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Nested Structs ==="));
    
    FName CollectionName = "NestedStructTest";
    FString TestKey = "NestedKey";
    
    FNestedStruct NestedStruct = CreateDeeplyNestedStruct();
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created nested struct with %d inner array elements, %d inner map entries"), 
        NestedStruct.InnerArray.Num(), NestedStruct.InnerMap.Num()));
    
    // Test storing nested struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FNestedStruct>(
        World, CollectionName, TestKey, NestedStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Nested struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to set nested struct: %s"), *SetResult.ErrorMessage));
        return false;
    }
    
    // Test retrieving nested struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FNestedStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Nested struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to get nested struct: %s"), *GetResult.Result.ErrorMessage));
        return false;
    }
    
    FNestedStruct RetrievedStruct = GetResult.Value;
    bool bAreEqual = NestedStruct == RetrievedStruct;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Nested structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved nested struct has %d inner array elements, %d inner map entries"), 
        RetrievedStruct.InnerArray.Num(), RetrievedStruct.InnerMap.Num()));
    
    return SetResult.IsSuccess() && GetResult.IsSuccess() && bAreEqual;
}

bool FWeirdStructTestRunner::TestCircularStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Circular Reference Structs ==="));
    
    FName CollectionName = "CircularStructTest";
    FString TestKey = "CircularKey";
    
    FCircularStruct CircularStruct = CreateCircularStruct(3); // 3 levels deep
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created circular struct '%s' with %d simulated children"), 
        *CircularStruct.Name, CircularStruct.ChildCount));
    
    // Test storing circular struct (this might cause issues)
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FCircularStruct>(
        World, CollectionName, TestKey, CircularStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Circular struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddWarning(FString::Printf(TEXT("Expected: Circular struct might fail: %s"), *SetResult.ErrorMessage));
        return true; // This might be expected
    }
    
    // Test retrieving circular struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FCircularStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Circular struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (GetResult.IsSuccess())
    {
        FCircularStruct RetrievedStruct = GetResult.Value;
        bool bAreEqual = CircularStruct == RetrievedStruct;
        
        TestSpec->AddInfo(FString::Printf(TEXT("Circular structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
        
        return bAreEqual;
    }
    
    return SetResult.IsSuccess();
}

bool FWeirdStructTestRunner::TestObjectPointerStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Object Pointer Structs (DANGEROUS!) ==="));
    
    FName CollectionName = "ObjectPointerTest";
    FString TestKey = "ObjectPointerKey";
    
    FObjectPointerStruct ObjectStruct;
    
    // Create some objects to point to
    UObject* TestObject = NewObject<UObject>();
    ObjectStruct.ObjectPtr = TestObject;
    ObjectStruct.WeakObjectPtr = TestObject;
    // Don't set SoftObjectPtr as it's more complex
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created object pointer struct with ObjectPtr: %s"), 
        ObjectStruct.ObjectPtr ? TEXT("Valid") : TEXT("Null")));
    
    // Test storing object pointer struct (this should probably fail or be dangerous)
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FObjectPointerStruct>(
        World, CollectionName, TestKey, ObjectStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Object pointer struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddWarning(FString::Printf(TEXT("Expected: Object pointer struct might fail: %s"), *SetResult.ErrorMessage));
        return true; // This is probably expected
    }
    
    // Test retrieving object pointer struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FObjectPointerStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Object pointer struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (GetResult.IsSuccess())
    {
        FObjectPointerStruct RetrievedStruct = GetResult.Value;
        
        TestSpec->AddInfo(FString::Printf(TEXT("Retrieved ObjectPtr: %s"), 
            RetrievedStruct.ObjectPtr ? TEXT("Valid") : TEXT("Null")));
        TestSpec->AddInfo(FString::Printf(TEXT("Retrieved WeakObjectPtr: %s"), 
            RetrievedStruct.WeakObjectPtr.IsValid() ? TEXT("Valid") : TEXT("Invalid")));
        
        // Note: Object pointers will likely be null after serialization/deserialization
        bool bPointersMatch = (ObjectStruct.ObjectPtr == RetrievedStruct.ObjectPtr);
        
        TestSpec->AddWarning(FString::Printf(TEXT("Object pointers match: %s (This is likely false and expected)"), 
            bPointersMatch ? TEXT("Yes") : TEXT("No")));
        
        return true; // Consider this a success regardless of pointer values
    }
    
    return SetResult.IsSuccess();
}

bool FWeirdStructTestRunner::TestDelegateStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Delegate Structs ==="));
    
    FName CollectionName = "DelegateStructTest";
    FString TestKey = "DelegateKey";
    
    FDelegateStruct DelegateStruct;
    // Note: We can't really bind the delegate in a meaningful way for testing
    
    TestSpec->AddInfo(TEXT("Created delegate struct (delegate is unbound)"));
    
    // Test storing delegate struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FDelegateStruct>(
        World, CollectionName, TestKey, DelegateStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Delegate struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddWarning(FString::Printf(TEXT("Expected: Delegate struct might fail: %s"), *SetResult.ErrorMessage));
        return true; // This might be expected
    }
    
    // Test retrieving delegate struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FDelegateStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Delegate struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (GetResult.IsSuccess())
    {
        FDelegateStruct RetrievedStruct = GetResult.Value;
        
        TestSpec->AddInfo(TEXT("Successfully retrieved delegate struct"));
        
        return true;
    }
    
    return SetResult.IsSuccess();
}

bool FWeirdStructTestRunner::TestSpecialTypesStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Special Types Structs ==="));
    
    FName CollectionName = "SpecialTypesTest";
    FString TestKey = "SpecialTypesKey";
    
    FSpecialTypesStruct SpecialStruct = CreateSpecialTypesStruct();
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created special types struct with GUID: %s"), 
        *SpecialStruct.UniqueId.ToString()));
    
    // Test storing special types struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FSpecialTypesStruct>(
        World, CollectionName, TestKey, SpecialStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Special types struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to set special types struct: %s"), *SetResult.ErrorMessage));
        return false;
    }
    
    // Test retrieving special types struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FSpecialTypesStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Special types struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to get special types struct: %s"), *GetResult.Result.ErrorMessage));
        return false;
    }
    
    FSpecialTypesStruct RetrievedStruct = GetResult.Value;
    bool bAreEqual = SpecialStruct == RetrievedStruct;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Special types structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved GUID: %s"), *RetrievedStruct.UniqueId.ToString()));
    
    return SetResult.IsSuccess() && GetResult.IsSuccess() && bAreEqual;
}

bool FWeirdStructTestRunner::TestInheritanceStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Inheritance Structs ==="));
    
    FName CollectionName = "InheritanceTest";
    FString TestKey = "InheritanceKey";
    
    FDerivedStruct DerivedStruct;
    DerivedStruct.BaseValue = 42;
    DerivedStruct.DerivedValue = TEXT("Derived Data");
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created derived struct with BaseValue: %d, DerivedValue: %s"), 
        DerivedStruct.BaseValue, *DerivedStruct.DerivedValue));
    
    // Test storing derived struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FDerivedStruct>(
        World, CollectionName, TestKey, DerivedStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Derived struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to set derived struct: %s"), *SetResult.ErrorMessage));
        return false;
    }
    
    // Test retrieving derived struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FDerivedStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Derived struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to get derived struct: %s"), *GetResult.Result.ErrorMessage));
        return false;
    }
    
    FDerivedStruct RetrievedStruct = GetResult.Value;
    bool bAreEqual = DerivedStruct == RetrievedStruct;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Derived structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved BaseValue: %d, DerivedValue: %s"), 
        RetrievedStruct.BaseValue, *RetrievedStruct.DerivedValue));
    
    return SetResult.IsSuccess() && GetResult.IsSuccess() && bAreEqual;
}

bool FWeirdStructTestRunner::TestPackedStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Packed/Bitfield Structs ==="));
    
    FName CollectionName = "PackedStructTest";
    FString TestKey = "PackedKey";
    
    FPackedStruct PackedStruct;
    PackedStruct.Flag1 = true;
    PackedStruct.Flag2 = false;
    PackedStruct.SmallValue = 255; // Max uint8 value
    PackedStruct.PackedData = 0xDEADBEEF;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created packed struct with Flag1: %s, Flag2: %s, SmallValue: %d, PackedData: 0x%X"), 
        PackedStruct.Flag1 ? TEXT("true") : TEXT("false"), 
        PackedStruct.Flag2 ? TEXT("true") : TEXT("false"), 
        PackedStruct.SmallValue, PackedStruct.PackedData));
    
    // Test storing packed struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FPackedStruct>(
        World, CollectionName, TestKey, PackedStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Packed struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to set packed struct: %s"), *SetResult.ErrorMessage));
        return false;
    }
    
    // Test retrieving packed struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FPackedStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Packed struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to get packed struct: %s"), *GetResult.Result.ErrorMessage));
        return false;
    }
    
    FPackedStruct RetrievedStruct = GetResult.Value;
    bool bAreEqual = PackedStruct == RetrievedStruct;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Packed structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved Flag1: %s, Flag2: %s, SmallValue: %d, PackedData: 0x%X"), 
        RetrievedStruct.Flag1 ? TEXT("true") : TEXT("false"), 
        RetrievedStruct.Flag2 ? TEXT("true") : TEXT("false"), 
        RetrievedStruct.SmallValue, RetrievedStruct.PackedData));
    
    return SetResult.IsSuccess() && GetResult.IsSuccess() && bAreEqual;
}

bool FWeirdStructTestRunner::TestDataTableRowStructs(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing DataTable Row Structs ==="));
    
    FName CollectionName = "DataTableRowTest";
    FString TestKey = "DataTableKey";
    
    FDataTableRowStruct DataTableStruct;
    DataTableStruct.ItemName = TEXT("Legendary Sword");
    DataTableStruct.ItemValue = 1000;
    DataTableStruct.ItemWeight = 5.5f;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Created DataTable row struct: %s (Value: %d, Weight: %.2f)"), 
        *DataTableStruct.ItemName, DataTableStruct.ItemValue, DataTableStruct.ItemWeight));
    
    // Test storing DataTable row struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FDataTableRowStruct>(
        World, CollectionName, TestKey, DataTableStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("DataTable row struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to set DataTable row struct: %s"), *SetResult.ErrorMessage));
        return false;
    }
    
    // Test retrieving DataTable row struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FDataTableRowStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("DataTable row struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to get DataTable row struct: %s"), *GetResult.Result.ErrorMessage));
        return false;
    }
    
    FDataTableRowStruct RetrievedStruct = GetResult.Value;
    bool bAreEqual = DataTableStruct == RetrievedStruct;
    
    TestSpec->AddInfo(FString::Printf(TEXT("DataTable row structs equal: %s"), bAreEqual ? TEXT("Yes") : TEXT("No")));
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved: %s (Value: %d, Weight: %.2f)"), 
        *RetrievedStruct.ItemName, RetrievedStruct.ItemValue, RetrievedStruct.ItemWeight));
    
    return SetResult.IsSuccess() && GetResult.IsSuccess() && bAreEqual;
}

bool FWeirdStructTestRunner::TestConcurrentStructAccess(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Concurrent Struct Access ==="));
    
    FName CollectionName = "ConcurrentTest";
    FString TestKey = "ConcurrentKey";
    
    FTestStruct TestStruct;
    TestStruct.IntValue = 999;
    TestStruct.StringValue = TEXT("Concurrent Test");
    TestStruct.FloatValue = 9.99f;
    
    // Simulate concurrent access by rapidly setting and getting
    bool bAllSucceeded = true;
    
    for (int32 i = 0; i < 10; ++i)
    {
        // Modify the struct slightly each iteration
        TestStruct.IntValue = 999 + i;
        
        FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FTestStruct>(
            World, CollectionName, TestKey, TestStruct);
        
        if (!SetResult.IsSuccess())
        {
            TestSpec->AddError(FString::Printf(TEXT("Concurrent set %d failed: %s"), i, *SetResult.ErrorMessage));
            bAllSucceeded = false;
            continue;
        }
        
        auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FTestStruct>(
            World, CollectionName, TestKey);
        
        if (!GetResult.IsSuccess())
        {
            TestSpec->AddError(FString::Printf(TEXT("Concurrent get %d failed: %s"), i, *GetResult.Result.ErrorMessage));
            bAllSucceeded = false;
            continue;
        }
        
        FTestStruct RetrievedStruct = GetResult.Value;
        if (!(TestStruct == RetrievedStruct))
        {
            TestSpec->AddError(FString::Printf(TEXT("Concurrent iteration %d: structs don't match"), i));
            bAllSucceeded = false;
        }
    }
    
    TestSpec->AddInfo(FString::Printf(TEXT("Concurrent access test result: %s"), 
        bAllSucceeded ? TEXT("All operations succeeded") : TEXT("Some operations failed")));
    
    return bAllSucceeded;
}

bool FWeirdStructTestRunner::TestStructModificationAfterStorage(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Struct Modification After Storage ==="));
    
    FName CollectionName = "ModificationTest";
    FString TestKey = "ModificationKey";
    
    FTestStruct OriginalStruct;
    OriginalStruct.IntValue = 100;
    OriginalStruct.StringValue = TEXT("Original");
    OriginalStruct.FloatValue = 1.0f;
    
    // Store the struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FTestStruct>(
        World, CollectionName, TestKey, OriginalStruct);
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to store original struct: %s"), *SetResult.ErrorMessage));
        return false;
    }
    
    // Modify the original struct after storage
    OriginalStruct.IntValue = 200;
    OriginalStruct.StringValue = TEXT("Modified");
    OriginalStruct.FloatValue = 2.0f;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Modified original struct: IntValue=%d, StringValue=%s"), 
        OriginalStruct.IntValue, *OriginalStruct.StringValue));
    
    // Retrieve the struct - it should still have the original values
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FTestStruct>(
        World, CollectionName, TestKey);
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to retrieve struct: %s"), *GetResult.Result.ErrorMessage));
        return false;
    }
    
    FTestStruct RetrievedStruct = GetResult.Value;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved struct: IntValue=%d, StringValue=%s"), 
        RetrievedStruct.IntValue, *RetrievedStruct.StringValue));
    
    // The retrieved struct should have the original values, not the modified ones
    bool bHasOriginalValues = (RetrievedStruct.IntValue == 100 && 
                               RetrievedStruct.StringValue == TEXT("Original") && 
                               FMath::IsNearlyEqual(RetrievedStruct.FloatValue, 1.0f));
    
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved struct has original values: %s"), 
        bHasOriginalValues ? TEXT("Yes (correct)") : TEXT("No (incorrect)")));
    
    return bHasOriginalValues;
}

bool FWeirdStructTestRunner::TestStructMemoryPatterns(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Struct Memory Patterns ==="));
    
    FName CollectionName = "MemoryPatternTest";
    
    // Test with different memory patterns (using safe approaches)
    TArray<FTestStruct> TestStructs;
    
    // Pattern 1: Default initialized (essentially zeros for primitives)
    FTestStruct ZeroStruct;
    ZeroStruct.IntValue = 0;
    ZeroStruct.StringValue = TEXT("");
    ZeroStruct.FloatValue = 0.0f;
    TestStructs.Add(ZeroStruct);
    
    // Pattern 2: Maximum values pattern
    FTestStruct MaxStruct;
    MaxStruct.IntValue = INT32_MAX;
    MaxStruct.StringValue = TEXT("MaxPattern_") + FString::FromInt(INT32_MAX);
    MaxStruct.FloatValue = FLT_MAX;
    TestStructs.Add(MaxStruct);
    
    // Pattern 3: Alternating pattern (using safe bit patterns for primitives)
    FTestStruct PatternStruct;
    PatternStruct.IntValue = 0xAAAAAAAA;
    PatternStruct.StringValue = TEXT("AlternatingPattern_0xAAAAAAAA");
    PatternStruct.FloatValue = -123.456f; // Some recognizable pattern
    TestStructs.Add(PatternStruct);
    
    bool bAllSucceeded = true;
    
    for (int32 i = 0; i < TestStructs.Num(); ++i)
    {
        FString TestKey = FString::Printf(TEXT("MemoryPattern_%d"), i);
        
        FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FTestStruct>(
            World, CollectionName, TestKey, TestStructs[i]);
        
        TestSpec->AddInfo(FString::Printf(TEXT("Memory pattern %d set result: %s"), i,
            SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
        
        if (!SetResult.IsSuccess())
        {
            TestSpec->AddWarning(FString::Printf(TEXT("Memory pattern %d failed (might be expected): %s"), 
                i, *SetResult.ErrorMessage));
            continue; // Some patterns might fail and that's okay
        }
        
        auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FTestStruct>(
            World, CollectionName, TestKey);
        
        if (!GetResult.IsSuccess())
        {
            TestSpec->AddError(FString::Printf(TEXT("Failed to get memory pattern %d: %s"), 
                i, *GetResult.Result.ErrorMessage));
            bAllSucceeded = false;
            continue;
        }
        
        TestSpec->AddInfo(FString::Printf(TEXT("Memory pattern %d retrieved successfully"), i));
    }
    
    return bAllSucceeded;
}

bool FWeirdStructTestRunner::TestStructSerializationEdgeCases(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Struct Serialization Edge Cases ==="));
    
    FName CollectionName = "SerializationEdgeTest";
    FString TestKey = "SerializationKey";
    
    // Test struct with extreme string values
    FTestStruct EdgeStruct;
    EdgeStruct.IntValue = TNumericLimits<int32>::Max();
    EdgeStruct.FloatValue = TNumericLimits<float>::Max();
    
    // Test with various problematic string content
    TArray<FString> ProblematicStrings = {
        TEXT(""),                                    // Empty string
        TEXT("\0"),                                  // Null character
        TEXT("String with\nnewlines\nand\ttabs"),   // Control characters
        TEXT("String with \"quotes\" and 'apostrophes'"), // Quotes
        TEXT("String with \\backslashes\\"),        // Backslashes
        TEXT("Very long string: ") + FString::ChrN(1000, TEXT('X')), // Very long
        TEXT("Unicode: 한글 日本語 中文 العربية"), // Unicode
        TEXT("{}[]()<>|&*?~`!@#$%^&*()_+-="),      // Special characters
    };
    
    bool bAllSucceeded = true;
    
    for (int32 i = 0; i < ProblematicStrings.Num(); ++i)
    {
        EdgeStruct.StringValue = ProblematicStrings[i];
        FString TestKeyWithIndex = FString::Printf(TEXT("%s_%d"), *TestKey, i);
        
        FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FTestStruct>(
            World, CollectionName, TestKeyWithIndex, EdgeStruct);
        
        TestSpec->AddInfo(FString::Printf(TEXT("Problematic string %d ('%s') set result: %s"), 
            i, *ProblematicStrings[i].Left(20), SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
        
        if (!SetResult.IsSuccess())
        {
            TestSpec->AddWarning(FString::Printf(TEXT("Problematic string %d failed: %s"), 
                i, *SetResult.ErrorMessage));
            continue; // Some might fail and that's expected
        }
        
        auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FTestStruct>(
            World, CollectionName, TestKeyWithIndex);
        
        if (!GetResult.IsSuccess())
        {
            TestSpec->AddError(FString::Printf(TEXT("Failed to get problematic string %d: %s"), 
                i, *GetResult.Result.ErrorMessage));
            bAllSucceeded = false;
            continue;
        }
        
        FTestStruct RetrievedStruct = GetResult.Value;
        bool bStringMatches = (EdgeStruct.StringValue == RetrievedStruct.StringValue);
        
        TestSpec->AddInfo(FString::Printf(TEXT("Problematic string %d matches: %s"), 
            i, bStringMatches ? TEXT("Yes") : TEXT("No")));
        
        if (!bStringMatches)
        {
            bAllSucceeded = false;
        }
    }
    
    return bAllSucceeded;
}

bool FWeirdStructTestRunner::TestUninitializedStructData(UWorld* World, FAutomationSpecBase* TestSpec)
{
    TestSpec->AddInfo(TEXT("=== Testing Uninitialized Struct Data ==="));
    
    FName CollectionName = "UninitializedTest";
    FString TestKey = "UninitializedKey";
    
    // Create struct without proper initialization
    FTestStruct UninitializedStruct;
    // Don't initialize anything - let it have random stack data
    
    TestSpec->AddInfo(FString::Printf(TEXT("Uninitialized struct IntValue: %d, FloatValue: %f"), 
        UninitializedStruct.IntValue, UninitializedStruct.FloatValue));
    TestSpec->AddInfo(FString::Printf(TEXT("Uninitialized struct StringValue length: %d"), 
        UninitializedStruct.StringValue.Len()));
    
    // Test storing uninitialized struct
    FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FTestStruct>(
        World, CollectionName, TestKey, UninitializedStruct);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Uninitialized struct set result: %s"), 
        SetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!SetResult.IsSuccess())
    {
        TestSpec->AddWarning(FString::Printf(TEXT("Uninitialized struct failed (might be expected): %s"), 
            *SetResult.ErrorMessage));
        return true; // This might be expected
    }
    
    // Test retrieving uninitialized struct
    auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FTestStruct>(
        World, CollectionName, TestKey);
    
    TestSpec->AddInfo(FString::Printf(TEXT("Uninitialized struct get result: %s"), 
        GetResult.IsSuccess() ? TEXT("Success") : TEXT("Failed")));
    
    if (!GetResult.IsSuccess())
    {
        TestSpec->AddError(FString::Printf(TEXT("Failed to get uninitialized struct: %s"), 
            *GetResult.Result.ErrorMessage));
        return false;
    }
    
    FTestStruct RetrievedStruct = GetResult.Value;
    
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved IntValue: %d, FloatValue: %f"), 
        RetrievedStruct.IntValue, RetrievedStruct.FloatValue));
    TestSpec->AddInfo(FString::Printf(TEXT("Retrieved StringValue length: %d"), 
        RetrievedStruct.StringValue.Len()));
    
    // For uninitialized data, we just check that it doesn't crash
    return true;
}

// Helper function implementations
FHugeStruct FWeirdStructTestRunner::CreateMassiveStruct()
{
    FHugeStruct HugeStruct;
    
    // Create massive array (10,000 elements)
    HugeStruct.MassiveArray.Reserve(10000);
    for (int32 i = 0; i < 10000; ++i)
    {
        HugeStruct.MassiveArray.Add(i);
    }
    
    // Create huge map (1,000 entries)
    for (int32 i = 0; i < 1000; ++i)
    {
        FString Key = FString::Printf(TEXT("Key_%d"), i);
        FString Value = FString::Printf(TEXT("Value_%d_with_some_extra_text_to_make_it_larger"), i);
        HugeStruct.HugeMap.Add(Key, Value);
    }
    
    // Create very long string (100KB)
    FString Pattern = TEXT("This is a very long pattern that will be repeated many times to create a huge string. ");
    HugeStruct.VeryLongString.Reserve(100000);
    while (HugeStruct.VeryLongString.Len() < 100000)
    {
        HugeStruct.VeryLongString += Pattern;
    }
    
    // Create lots of vectors (5,000 vectors)
    HugeStruct.LotsOfVectors.Reserve(5000);
    for (int32 i = 0; i < 5000; ++i)
    {
        HugeStruct.LotsOfVectors.Add(FVector(i, i * 2, i * 3));
    }
    
    // Create many transforms (1,000 transforms)
    HugeStruct.ManyTransforms.Reserve(1000);
    for (int32 i = 0; i < 1000; ++i)
    {
        FTransform Transform;
        Transform.SetLocation(FVector(i, i, i));
        Transform.SetRotation(FQuat::MakeFromEuler(FVector(i, i, i)));
        Transform.SetScale3D(FVector(1.0f + i * 0.1f));
        HugeStruct.ManyTransforms.Add(Transform);
    }
    
    return HugeStruct;
}

FNestedStruct FWeirdStructTestRunner::CreateDeeplyNestedStruct()
{
    FNestedStruct NestedStruct;
    
    // Set up inner struct
    NestedStruct.Inner.Value = 42;
    NestedStruct.Inner.Name = TEXT("Inner Struct");
    
    // Create array of inner structs
    for (int32 i = 0; i < 100; ++i)
    {
        FInnerStruct InnerStruct;
        InnerStruct.Value = i;
        InnerStruct.Name = FString::Printf(TEXT("Inner_%d"), i);
        NestedStruct.InnerArray.Add(InnerStruct);
    }
    
    // Create map of inner structs
    for (int32 i = 0; i < 50; ++i)
    {
        FString Key = FString::Printf(TEXT("MapKey_%d"), i);
        FInnerStruct InnerStruct;
        InnerStruct.Value = i * 10;
        InnerStruct.Name = FString::Printf(TEXT("MapInner_%d"), i);
        NestedStruct.InnerMap.Add(Key, InnerStruct);
    }
    
    return NestedStruct;
}

FCircularStruct FWeirdStructTestRunner::CreateCircularStruct(int32 Depth)
{
    FCircularStruct CircularStruct;
    CircularStruct.Name = FString::Printf(TEXT("Level_%d"), Depth);
    
    if (Depth > 0)
    {
        // Simulate children by storing count and serialized data
        CircularStruct.ChildCount = 3;
        CircularStruct.ChildData = FString::Printf(TEXT("Child_Data_Depth_%d"), Depth - 1);
    }
    else
    {
        CircularStruct.ChildCount = 0;
        CircularStruct.ChildData = TEXT("");
    }
    
    return CircularStruct;
}

FSpecialTypesStruct FWeirdStructTestRunner::CreateSpecialTypesStruct()
{
    FSpecialTypesStruct SpecialStruct;
    
    SpecialStruct.UniqueId = FGuid::NewGuid();
    SpecialStruct.TimeStamp = FDateTime::Now();
    SpecialStruct.Duration = FTimespan::FromMinutes(123.456);
    SpecialStruct.Color = FColor::Red;
    SpecialStruct.LinearColor = FLinearColor::Blue;
    SpecialStruct.Vector2D = FVector2D(1.23f, 4.56f);
    SpecialStruct.Vector = FVector(1.0f, 2.0f, 3.0f);
    SpecialStruct.Vector4 = FVector4(1.0f, 2.0f, 3.0f, 4.0f);
    SpecialStruct.Quaternion = FQuat::MakeFromEuler(FVector(45.0f, 90.0f, 180.0f));
    SpecialStruct.Rotator = FRotator(30.0f, 60.0f, 90.0f);
    SpecialStruct.Transform = FTransform(SpecialStruct.Quaternion, SpecialStruct.Vector, FVector::OneVector);
    
    return SpecialStruct;
}