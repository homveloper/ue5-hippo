#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
// #include "Engine/LocalPlayer.h" // Not needed for GameInstanceSubsystem
#include "Engine/GameViewportClient.h"
#include "HippocacheSubsystem.h"
#include "HippocacheBlueprintLibrary.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/DateTime.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Algo/MaxElement.h"
#include "Widgets/SOverlay.h"
#include "Tests/TestStructs.h"
#include "Tests/WeirdTestStructs.h"

#if WITH_DEV_AUTOMATION_TESTS

// ApplicationContextMask is deprecated in UE 5.6+, use conditional compilation for compatibility
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHippocacheBenchmarkTest, "Hippocache.Performance.Benchmark", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)
#else
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHippocacheBenchmarkTest, "Hippocache.Performance.Benchmark", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)
#endif

/**
 * Benchmark test context for performance measurements
 */
struct FHippocacheBenchmarkContext
{
    UWorld* TestWorld = nullptr;
    UHippocacheSubsystem* Subsystem = nullptr;
    FName TestCollection = TEXT("BenchmarkCollection");
    
    // Benchmark results
    double SetOperationsPerSecond = 0.0;
    double GetOperationsPerSecond = 0.0;
    double AverageSetTimeNanoseconds = 0.0;
    double AverageGetTimeNanoseconds = 0.0;
};

/**
 * Setup benchmark test environment
 */
bool SetupBenchmarkTest(FHippocacheBenchmarkContext& Context, FAutomationTestBase* TestSpec)
{
    // Create GameInstance with unique name
    static int32 UniqueInstanceID = 0;
    UniqueInstanceID++;
    
    FString GameInstanceName = FString::Printf(TEXT("BenchmarkGameInstance_%d"), UniqueInstanceID);
    UGameInstance* GameInstance = NewObject<UGameInstance>(GEngine, UGameInstance::StaticClass(), *GameInstanceName);
    
    if (!GameInstance)
    {
        TestSpec->AddError(TEXT("Failed to create GameInstance"));
        return false;
    }
    
    // Create a minimal world for testing - avoid complex initialization
    FString WorldName = FString::Printf(TEXT("BenchmarkWorld_%d"), UniqueInstanceID);
    Context.TestWorld = UWorld::CreateWorld(EWorldType::Game, false, *WorldName);
    if (!Context.TestWorld)
    {
        TestSpec->AddError(TEXT("Failed to create test world"));
        return false;
    }
    
    // Set GameInstance first before any other setup
    Context.TestWorld->SetGameInstance(GameInstance);
    
    // Create world context and properly link everything
    FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
    WorldContext.SetCurrentWorld(Context.TestWorld);
    WorldContext.OwningGameInstance = GameInstance;
    
    TestSpec->AddInfo(TEXT("World and GameInstance setup completed (minimal initialization)"));
    
    // Simplest approach: Just try to get the subsystem without complex initialization
    // The subsystem should be automatically available in most test scenarios
    UHippocacheSubsystem* HippocacheSubsystem = nullptr;
    
    // Method 1: Try via GameInstance (most reliable in test environment)
    HippocacheSubsystem = GameInstance->GetSubsystem<UHippocacheSubsystem>();
    if (HippocacheSubsystem)
    {
        TestSpec->AddInfo(TEXT("HippocacheSubsystem found via GameInstance"));
    }
    else
    {
        // Method 2: Try the standard Get method
        FHippocacheResult GetResult = UHippocacheSubsystem::Get(Context.TestWorld, HippocacheSubsystem);
        if (GetResult.IsSuccess() && HippocacheSubsystem)
        {
            TestSpec->AddInfo(TEXT("HippocacheSubsystem obtained via standard Get method"));
        }
        else
        {
            // Method 3: Create minimal standalone instance
            TestSpec->AddInfo(TEXT("Creating minimal HippocacheSubsystem for test"));
            HippocacheSubsystem = NewObject<UHippocacheSubsystem>(GameInstance);
            if (HippocacheSubsystem)
            {
                TestSpec->AddInfo(TEXT("Minimal HippocacheSubsystem created successfully"));
            }
            else
            {
                TestSpec->AddError(TEXT("Failed to create HippocacheSubsystem"));
                return false;
            }
        }
    }
    
    Context.Subsystem = HippocacheSubsystem;
    return true;
}

/**
 * Cleanup benchmark test environment
 */
void CleanupBenchmarkTest(FHippocacheBenchmarkContext& Context)
{
    if (Context.Subsystem)
    {
        Context.Subsystem->Clear(Context.TestCollection);
    }
    
    if (Context.TestWorld)
    {
        Context.TestWorld->DestroyWorld(false);
        Context.TestWorld = nullptr;
    }
}

/**
 * Benchmark Set operations with different data types
 */
template<typename T>
void BenchmarkSetOperations(FHippocacheBenchmarkContext& Context, const TArray<T>& TestValues, 
                           const FString& TypeName, FAutomationTestBase* TestSpec, 
                           TFunction<FHippocacheResult(UHippocacheSubsystem*, FName, const FString&, const T&)> SetFunction)
{
    const int32 NumIterations = TestValues.Num();
    const double BenchmarkDurationSeconds = 5.0; // Run for 5 seconds
    
    TArray<double> OperationTimes;
    int32 CompletedOperations = 0;
    
    double StartTime = FPlatformTime::Seconds();
    double CurrentTime = StartTime;
    
    // Run operations for specified duration
    while ((CurrentTime - StartTime) < BenchmarkDurationSeconds)
    {
        for (int32 i = 0; i < NumIterations && (FPlatformTime::Seconds() - StartTime) < BenchmarkDurationSeconds; ++i)
        {
            FString Key = FString::Printf(TEXT("%s_Key_%d_%d"), *TypeName, CompletedOperations, i);
            
            double OpStartTime = FPlatformTime::Seconds();
            FHippocacheResult Result = SetFunction(Context.Subsystem, Context.TestCollection, Key, TestValues[i]);
            double OpEndTime = FPlatformTime::Seconds();
            
            if (Result.IsError())
            {
                TestSpec->AddError(FString::Printf(TEXT("Set operation failed for %s: %s"), *TypeName, *Result.ErrorMessage));
                return;
            }
            
            OperationTimes.Add((OpEndTime - OpStartTime) * 1000000000.0); // Convert to nanoseconds
            CompletedOperations++;
        }
        CurrentTime = FPlatformTime::Seconds();
    }
    
    double TotalTime = CurrentTime - StartTime;
    double OperationsPerSecond = CompletedOperations / TotalTime;
    
    // Calculate average operation time
    double TotalNanoseconds = 0.0;
    for (double OpTime : OperationTimes)
    {
        TotalNanoseconds += OpTime;
    }
    double AverageNanoseconds = TotalNanoseconds / OperationTimes.Num();
    
    TestSpec->AddInfo(FString::Printf(TEXT("=== %s Set Operations Benchmark ==="), *TypeName));
    TestSpec->AddInfo(FString::Printf(TEXT("Total operations: %d"), CompletedOperations));
    TestSpec->AddInfo(FString::Printf(TEXT("Total time: %.3f seconds"), TotalTime));
    TestSpec->AddInfo(FString::Printf(TEXT("Operations per second: %.2f ops/sec"), OperationsPerSecond));
    TestSpec->AddInfo(FString::Printf(TEXT("Average time per operation: %.2f ns"), AverageNanoseconds));
    TestSpec->AddInfo(FString::Printf(TEXT("Min time per operation: %.2f ns"), *Algo::MinElement(OperationTimes)));
    // Find max element manually since Algo::MaxElement might not be available
    double MaxTime = 0.0;
    for (double OpTime : OperationTimes)
    {
        if (OpTime > MaxTime) MaxTime = OpTime;
    }
    TestSpec->AddInfo(FString::Printf(TEXT("Max time per operation: %.2f ns"), MaxTime));
}

/**
 * Benchmark Get operations with different data types
 */
template<typename T>
void BenchmarkGetOperations(FHippocacheBenchmarkContext& Context, const TArray<FString>& TestKeys, 
                           const FString& TypeName, FAutomationTestBase* TestSpec,
                           TFunction<FHippocacheResult(UHippocacheSubsystem*, FName, const FString&, T&)> GetFunction)
{
    const int32 NumKeys = TestKeys.Num();
    const double BenchmarkDurationSeconds = 5.0; // Run for 5 seconds
    
    TArray<double> OperationTimes;
    int32 CompletedOperations = 0;
    
    double StartTime = FPlatformTime::Seconds();
    double CurrentTime = StartTime;
    
    // Run operations for specified duration
    while ((CurrentTime - StartTime) < BenchmarkDurationSeconds)
    {
        for (int32 i = 0; i < NumKeys && (FPlatformTime::Seconds() - StartTime) < BenchmarkDurationSeconds; ++i)
        {
            T OutValue;
            
            double OpStartTime = FPlatformTime::Seconds();
            FHippocacheResult Result = GetFunction(Context.Subsystem, Context.TestCollection, TestKeys[i], OutValue);
            double OpEndTime = FPlatformTime::Seconds();
            
            if (Result.IsError())
            {
                // Skip missing keys - they might have been cleaned up
                continue;
            }
            
            OperationTimes.Add((OpEndTime - OpStartTime) * 1000000000.0); // Convert to nanoseconds
            CompletedOperations++;
        }
        CurrentTime = FPlatformTime::Seconds();
    }
    
    double TotalTime = CurrentTime - StartTime;
    double OperationsPerSecond = CompletedOperations / TotalTime;
    
    // Calculate average operation time
    double TotalNanoseconds = 0.0;
    for (double OpTime : OperationTimes)
    {
        TotalNanoseconds += OpTime;
    }
    double AverageNanoseconds = OperationTimes.Num() > 0 ? TotalNanoseconds / OperationTimes.Num() : 0.0;
    
    TestSpec->AddInfo(FString::Printf(TEXT("=== %s Get Operations Benchmark ==="), *TypeName));
    TestSpec->AddInfo(FString::Printf(TEXT("Total operations: %d"), CompletedOperations));
    TestSpec->AddInfo(FString::Printf(TEXT("Total time: %.3f seconds"), TotalTime));
    TestSpec->AddInfo(FString::Printf(TEXT("Operations per second: %.2f ops/sec"), OperationsPerSecond));
    TestSpec->AddInfo(FString::Printf(TEXT("Average time per operation: %.2f ns"), AverageNanoseconds));
    if (OperationTimes.Num() > 0)
    {
        TestSpec->AddInfo(FString::Printf(TEXT("Min time per operation: %.2f ns"), *Algo::MinElement(OperationTimes)));
        // Find max element manually since Algo::MaxElement might not be available
    double MaxTime = 0.0;
    for (double OpTime : OperationTimes)
    {
        if (OpTime > MaxTime) MaxTime = OpTime;
    }
    TestSpec->AddInfo(FString::Printf(TEXT("Max time per operation: %.2f ns"), MaxTime));
    }
}

bool FHippocacheBenchmarkTest::RunTest(const FString& Parameters)
{
    FHippocacheBenchmarkContext Context;
    
    // Setup test environment
    if (!SetupBenchmarkTest(Context, this))
    {
        return false;
    }
    
    AddInfo(TEXT("=== Hippocache Performance Benchmark Test ==="));
    AddInfo(FString::Printf(TEXT("Starting benchmark at: %s"), *FDateTime::Now().ToString()));
    
    // Prepare test data
    TArray<int32> IntValues;
    TArray<float> FloatValues;
    TArray<FString> StringValues;
    TArray<FString> SetKeys;
    
    const int32 NumTestValues = 1000;
    
    for (int32 i = 0; i < NumTestValues; ++i)
    {
        IntValues.Add(FMath::RandRange(1, 100000));
        FloatValues.Add(FMath::FRandRange(0.0f, 1000.0f));
        StringValues.Add(FString::Printf(TEXT("TestString_%d_%s"), i, *FGuid::NewGuid().ToString()));
    }
    
    // Benchmark Set operations for different types
    BenchmarkSetOperations<int32>(Context, IntValues, TEXT("Int32"), this,
        [&Context](UHippocacheSubsystem* Subsys, FName Collection, const FString& Key, const int32& Value) {
            return UHippocacheBlueprintLibrary::SetInt32(Context.TestWorld, Collection, Key, Value);
        });
    
    BenchmarkSetOperations<float>(Context, FloatValues, TEXT("Float"), this,
        [&Context](UHippocacheSubsystem* Subsys, FName Collection, const FString& Key, const float& Value) {
            return UHippocacheBlueprintLibrary::SetFloat(Context.TestWorld, Collection, Key, Value);
        });
    
    BenchmarkSetOperations<FString>(Context, StringValues, TEXT("String"), this,
        [&Context](UHippocacheSubsystem* Subsys, FName Collection, const FString& Key, const FString& Value) {
            return UHippocacheBlueprintLibrary::SetString(Context.TestWorld, Collection, Key, Value);
        });
    
    // Collect keys that were set for Get benchmarks
    // Note: This is a simplification - in reality Set benchmarks generate many different keys
    // For Get benchmarks, we'll set some known keys first
    for (int32 i = 0; i < 100; ++i) // Use subset for Get tests
    {
        FString Key = FString::Printf(TEXT("GetBench_Key_%d"), i);
        SetKeys.Add(Key);
        // Pre-set these keys for Get benchmark
        UHippocacheBlueprintLibrary::SetInt32(Context.TestWorld, Context.TestCollection, Key, 42);
    }
    
    // Benchmark Get operations
    BenchmarkGetOperations<int32>(Context, SetKeys, TEXT("Int32"), this,
        [&Context](UHippocacheSubsystem* Subsys, FName Collection, const FString& Key, int32& OutValue) {
            return UHippocacheBlueprintLibrary::GetInt32(Context.TestWorld, Collection, Key, OutValue);
        });
    
    // Memory usage benchmark
    int32 ItemCount = 0;
    FHippocacheResult CountResult = Context.Subsystem->Num(Context.TestCollection, ItemCount);
    if (CountResult.IsSuccess())
    {
        AddInfo(FString::Printf(TEXT("=== Memory Usage ==="), ItemCount));
        AddInfo(FString::Printf(TEXT("Total items in cache: %d"), ItemCount));
        
        // Estimate memory usage (rough calculation)
        double EstimatedMemoryMB = (ItemCount * (sizeof(FCachedItem) + 50)) / (1024.0 * 1024.0); // Rough estimate
        AddInfo(FString::Printf(TEXT("Estimated memory usage: %.2f MB"), EstimatedMemoryMB));
    }
    
    AddInfo(FString::Printf(TEXT("Benchmark completed at: %s"), *FDateTime::Now().ToString()));
    
    // Cleanup
    CleanupBenchmarkTest(Context);
    
    return true;
}

// ApplicationContextMask is deprecated in UE 5.6+, use conditional compilation for compatibility
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHippocacheRValueBenchmarkTest, "Hippocache.Performance.RValueBenchmark", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)
#else
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHippocacheRValueBenchmarkTest, "Hippocache.Performance.RValueBenchmark", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)
#endif

// Use existing large struct from WeirdTestStructs.h
// FHugeStruct serves as our large benchmark struct

/**
 * Benchmark lvalue vs rvalue performance for SetStruct operations
 */
template<typename T>
void BenchmarkLValueRValueComparison(FHippocacheBenchmarkContext& Context, const FString& TypeName, FAutomationTestBase* TestSpec)
{
    const int32 NumIterations = 1000;
    const FName LValueCollection = TEXT("LValueCollection");
    const FName RValueCollection = TEXT("RValueCollection");

    // Generate test data
    TArray<T> TestData;
    TestData.Reserve(NumIterations);
    for (int32 i = 0; i < NumIterations; ++i)
    {
        TestData.Add(T());
    }

    TestSpec->AddInfo(FString::Printf(TEXT("=== %s LValue vs RValue Performance Comparison ==="), *TypeName));

    // Benchmark 1: LValue operations (copy semantics)
    TArray<double> LValueTimes;
    double LValueStartTime = FPlatformTime::Seconds();
    
    for (int32 i = 0; i < NumIterations; ++i)
    {
        FString Key = FString::Printf(TEXT("LValue_Key_%d"), i);
        
        double OpStartTime = FPlatformTime::Seconds();
        const T& LValueRef = TestData[i];  // LValue reference
        FHippocacheResult Result = Context.Subsystem->SetStruct(LValueCollection, Key, LValueRef);
        double OpEndTime = FPlatformTime::Seconds();
        
        if (Result.IsError())
        {
            TestSpec->AddError(FString::Printf(TEXT("LValue SetStruct failed for %s: %s"), *TypeName, *Result.ErrorMessage));
            return;
        }
        
        LValueTimes.Add((OpEndTime - OpStartTime) * 1000000000.0); // Convert to nanoseconds
    }
    
    double LValueTotalTime = FPlatformTime::Seconds() - LValueStartTime;

    // Benchmark 2: RValue operations (move semantics potential)
    TArray<double> RValueTimes;
    double RValueStartTime = FPlatformTime::Seconds();
    
    for (int32 i = 0; i < NumIterations; ++i)
    {
        FString Key = FString::Printf(TEXT("RValue_Key_%d"), i);
        
        double OpStartTime = FPlatformTime::Seconds();
        T TempValue = TestData[i];  // Create copy for move
        FHippocacheResult Result = Context.Subsystem->SetStruct(RValueCollection, Key, MoveTemp(TempValue));
        double OpEndTime = FPlatformTime::Seconds();
        
        if (Result.IsError())
        {
            TestSpec->AddError(FString::Printf(TEXT("RValue SetStruct failed for %s: %s"), *TypeName, *Result.ErrorMessage));
            return;
        }
        
        RValueTimes.Add((OpEndTime - OpStartTime) * 1000000000.0); // Convert to nanoseconds
    }
    
    double RValueTotalTime = FPlatformTime::Seconds() - RValueStartTime;

    // Calculate statistics
    double LValueAverage = 0.0;
    double RValueAverage = 0.0;
    
    for (int32 i = 0; i < NumIterations; ++i)
    {
        LValueAverage += LValueTimes[i];
        RValueAverage += RValueTimes[i];
    }
    
    LValueAverage /= NumIterations;
    RValueAverage /= NumIterations;

    // Find min/max times
    double LValueMin = *Algo::MinElement(LValueTimes);
    double RValueMin = *Algo::MinElement(RValueTimes);
    
    double LValueMax = 0.0;
    double RValueMax = 0.0;
    for (int32 i = 0; i < NumIterations; ++i)
    {
        if (LValueTimes[i] > LValueMax) LValueMax = LValueTimes[i];
        if (RValueTimes[i] > RValueMax) RValueMax = RValueTimes[i];
    }

    // Report results
    TestSpec->AddInfo(FString::Printf(TEXT("LValue Results:")));
    TestSpec->AddInfo(FString::Printf(TEXT("  Total time: %.3f seconds"), LValueTotalTime));
    TestSpec->AddInfo(FString::Printf(TEXT("  Operations per second: %.2f ops/sec"), NumIterations / LValueTotalTime));
    TestSpec->AddInfo(FString::Printf(TEXT("  Average time per operation: %.2f ns"), LValueAverage));
    TestSpec->AddInfo(FString::Printf(TEXT("  Min/Max time per operation: %.2f / %.2f ns"), LValueMin, LValueMax));

    TestSpec->AddInfo(FString::Printf(TEXT("RValue Results:")));
    TestSpec->AddInfo(FString::Printf(TEXT("  Total time: %.3f seconds"), RValueTotalTime));
    TestSpec->AddInfo(FString::Printf(TEXT("  Operations per second: %.2f ops/sec"), NumIterations / RValueTotalTime));
    TestSpec->AddInfo(FString::Printf(TEXT("  Average time per operation: %.2f ns"), RValueAverage));
    TestSpec->AddInfo(FString::Printf(TEXT("  Min/Max time per operation: %.2f / %.2f ns"), RValueMin, RValueMax));

    // Performance comparison
    double PerformanceImprovement = ((LValueAverage - RValueAverage) / LValueAverage) * 100.0;
    TestSpec->AddInfo(FString::Printf(TEXT("Performance Comparison:")));
    TestSpec->AddInfo(FString::Printf(TEXT("  RValue is %.2f%% %s than LValue"), 
                                     FMath::Abs(PerformanceImprovement), 
                                     PerformanceImprovement > 0 ? TEXT("FASTER") : TEXT("SLOWER")));
    
    if (PerformanceImprovement > 5.0)
    {
        TestSpec->AddInfo(FString::Printf(TEXT("  → SIGNIFICANT PERFORMANCE IMPROVEMENT with RValue semantics!")));
    }
    else if (PerformanceImprovement < -5.0)
    {
        TestSpec->AddWarning(FString::Printf(TEXT("  → RValue semantics are slower - possible optimization issue")));
    }
    else
    {
        TestSpec->AddInfo(FString::Printf(TEXT("  → Performance difference is negligible")));
    }

    // Cleanup
    Context.Subsystem->Clear(LValueCollection);
    Context.Subsystem->Clear(RValueCollection);
}

bool FHippocacheRValueBenchmarkTest::RunTest(const FString& Parameters)
{
    FHippocacheBenchmarkContext Context;
    
    // Setup test environment
    if (!SetupBenchmarkTest(Context, this))
    {
        return false;
    }
    
    AddInfo(TEXT("=== Hippocache RValue vs LValue Performance Benchmark ==="));
    AddInfo(FString::Printf(TEXT("Starting benchmark at: %s"), *FDateTime::Now().ToString()));
    AddInfo(TEXT("This test compares performance between LValue (copy) and RValue (move) operations"));
    AddInfo(TEXT("Current implementation uses const T& (always copy) - we're testing potential improvement"));
    
    // Test with different struct sizes
    
    // Small struct test
    BenchmarkLValueRValueComparison<FSimpleTestStruct>(Context, TEXT("SmallStruct"), this);
    
    // Large struct test  
    BenchmarkLValueRValueComparison<FHugeStruct>(Context, TEXT("LargeStruct"), this);
    
    AddInfo(TEXT(""));
    AddInfo(TEXT("=== Analysis ==="));
    AddInfo(TEXT("Current SetStruct implementation: template<typename T> SetStruct(const T& Value)"));
    AddInfo(TEXT("Proposed optimization: template<typename T> SetStruct(T&& Value) with std::forward<T>"));
    AddInfo(TEXT(""));
    AddInfo(TEXT("If RValue shows significant performance improvement, especially for large structs,"));
    AddInfo(TEXT("implementing universal reference would be beneficial."));
    
    AddInfo(FString::Printf(TEXT("Benchmark completed at: %s"), *FDateTime::Now().ToString()));
    
    // Cleanup
    CleanupBenchmarkTest(Context);
    
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS