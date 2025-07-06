#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "HippocacheBlueprintLibrary.h"
#include "HippocacheSubsystem.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
// #include "Engine/LocalPlayer.h" // Not needed for GameInstanceSubsystem
#include "Engine/GameViewportClient.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Widgets/SOverlay.h"
#include "Tests/TestStructs.h"
#include "Tests/WeirdTestStructs.h"
#include "Tests/WeirdStructTests.h"
#include "InstancedStruct.h"

#if WITH_DEV_AUTOMATION_TESTS

// Helper function to get world safely for testing
inline UWorld* GetWorldForTestSafe()
{
#if WITH_EDITOR
    if (GEditor && GEditor->GetWorldContexts().Num() && GEditor->GetWorldContexts()[0].World())
    {
        UE_LOG(LogTemp, Verbose, TEXT("Getting world from editor"));
        return GEditor->GetWorldContexts()[0].World();
    }
#endif
    if (GEngine && GEngine->GetWorldContexts().Num() && GEngine->GetWorldContexts()[0].World())
    {
        UE_LOG(LogTemp, Verbose, TEXT("Getting world from engine"));
        return GEngine->GetWorldContexts()[0].World();
    }
#if WITH_EDITOR
    if (GEditor)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Creating new world for editor"));
        return FAutomationEditorCommonUtils::CreateNewMap();
    }
#endif
    UE_LOG(LogTemp, Warning, TEXT("GEditor was not present, could not create World (map)"));
    return nullptr;
}

// Test context for Blueprint Library testing
struct FHippocacheBlueprintTestContext
{
    UWorld* TestWorld = nullptr;
    
    bool IsValid() const
    {
        return TestWorld != nullptr;
    }
    
    void Clear()
    {
        TestWorld = nullptr;
    }
};

// Helper class for Blueprint Library test setup
class FHippocacheBlueprintTestHelper
{
public:
    bool SetupBlueprintTest(FHippocacheBlueprintTestContext& Context, FAutomationSpecBase* TestSpec)
    {
        // Create a proper test world with all required components
        TestSpec->AddInfo(TEXT("Creating safe test world for automation"));
        
        // Create GameInstance with unique name to avoid conflicts
        static int32 UniqueInstanceID = 0;
        UniqueInstanceID++;
        
        FString GameInstanceName = FString::Printf(TEXT("TestGameInstance_%d"), UniqueInstanceID);
        UGameInstance* GameInstance = NewObject<UGameInstance>(GEngine, UGameInstance::StaticClass(), *GameInstanceName);
        if (!GameInstance)
        {
            TestSpec->AddError(TEXT("Failed to create GameInstance"));
            return false;
        }
        
        // Create World with unique name and set GameInstance
        FString WorldName = FString::Printf(TEXT("TestWorld_%d"), UniqueInstanceID);
        Context.TestWorld = UWorld::CreateWorld(EWorldType::Game, false, *WorldName);
        if (!Context.TestWorld)
        {
            TestSpec->AddError(TEXT("Failed to create test world"));
            return false;
        }
        
        Context.TestWorld->SetGameInstance(GameInstance);
        
        // This triggers the creation of a game mode which is required for some things
        Context.TestWorld->SetGameMode(FURL());
        
        // Set up the world context which is sometimes required to find the player
        FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
        WorldContext.SetCurrentWorld(Context.TestWorld);
        WorldContext.GameViewport = NewObject<UGameViewportClient>(GEngine);
        WorldContext.OwningGameInstance = Context.TestWorld->GetGameInstance();
        
        // Initialize actors for play
        Context.TestWorld->InitializeActorsForPlay(FURL());
        
        // Setup the viewport which is required for some things as well
        TSharedRef<SOverlay> DummyOverlay = SNew(SOverlay);
        WorldContext.GameViewport->SetViewportOverlayWidget(nullptr, DummyOverlay);
        
        // Note: LocalPlayer is not needed for GameInstanceSubsystem tests
        // Hippocache now uses GameInstanceSubsystem which is accessible through GameInstance
        /* 
        // Try to create LocalPlayer with a unique ID - expect ensure warning in editor, but it should still work
        FString Error;
        TestSpec->AddInfo(TEXT("Attempting to create LocalPlayer (expect editor warning)"));
        
        // Use a unique player ID to avoid conflicts between test runs
        static int32 UniquePlayerID = 0;
        UniquePlayerID++;
        
        ULocalPlayer* LocalPlayer = GameInstance->CreateLocalPlayer(UniquePlayerID, Error, false);
        if (LocalPlayer)
        {
            TestSpec->AddInfo(TEXT("Successfully created LocalPlayer for test (despite editor ensure warning)"));
        }
        else
        {
            TestSpec->AddWarning(FString::Printf(TEXT("Failed to create LocalPlayer: %s - some tests may fail"), *Error));
        }
        */
        
        // Manually initialize GameInstanceSubsystem for test environment
        UHippocacheSubsystem* HippocacheSubsystem = GameInstance->GetSubsystem<UHippocacheSubsystem>();
        if (!HippocacheSubsystem)
        {
            TestSpec->AddWarning(TEXT("HippocacheSubsystem not found in GameInstance - trying to initialize manually"));
            // Force subsystem initialization by calling Initialize if needed
            GameInstance->Init();
            HippocacheSubsystem = GameInstance->GetSubsystem<UHippocacheSubsystem>();
            if (HippocacheSubsystem)
            {
                TestSpec->AddInfo(TEXT("Successfully initialized HippocacheSubsystem manually"));
            }
            else
            {
                TestSpec->AddError(TEXT("Failed to initialize HippocacheSubsystem even manually"));
                return false;
            }
        }
        else
        {
            TestSpec->AddInfo(TEXT("HippocacheSubsystem found in GameInstance"));
        }
        
        TestSpec->AddInfo(TEXT("Safe test world setup completed"));
        return true;
    }
    
    void CleanupBlueprintTest(FHippocacheBlueprintTestContext& Context)
    {
        if (Context.TestWorld)
        {
            // Note: Skip Hippocache cleanup to avoid potential memory issues
            // Let garbage collection handle cleanup automatically
            /*
            // Clear any cached data from Hippocache first
            UGameInstance* GameInstance = Context.TestWorld->GetGameInstance();
            if (GameInstance)
            {
                UHippocacheSubsystem* HippocacheSubsystem = GameInstance->GetSubsystem<UHippocacheSubsystem>();
                if (HippocacheSubsystem)
                {
                    // Clear test collection data to prevent dangling references
                    HippocacheSubsystem->Clear(TEXT("BlueprintTestCollection"));
                }
            }
            */
            
            // Note: LocalPlayer cleanup not needed for GameInstanceSubsystem tests
            /*
            // Clean up LocalPlayers first
            if (GameInstance)
            {
                // Remove all local players from this game instance
                TArray<ULocalPlayer*> LocalPlayersToRemove = GameInstance->GetLocalPlayers();
                for (ULocalPlayer* Player : LocalPlayersToRemove)
                {
                    if (Player)
                    {
                        GameInstance->RemoveLocalPlayer(Player);
                    }
                }
            }
            */
            
            // Clean up actors before destroying world
            Context.TestWorld->CleanupWorld();
            
            // Note: Simplified cleanup to avoid memory access issues
            // Skip manual WorldContext cleanup - let garbage collection handle it
            /*
            // Clean up the world context we created (safer to do this after CleanupWorld)
            if (GEngine)
            {
                for (int32 i = GEngine->GetWorldContexts().Num() - 1; i >= 0; --i)
                {
                    const FWorldContext& WorldContext = GEngine->GetWorldContexts()[i];
                    if (WorldContext.World() == Context.TestWorld)
                    {
                        GEngine->DestroyWorldContext(Context.TestWorld);
                        break;
                    }
                }
            }
            */
            
            // Destroy the test world
            Context.TestWorld->DestroyWorld(false);
            Context.TestWorld = nullptr;
        }
        
        Context.Clear();
        
        // Force garbage collection to clean up any remaining references
        CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    }
};

// ApplicationContextMask is deprecated in UE 5.6+, use conditional compilation for compatibility
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6
DEFINE_SPEC(FHippocacheBlueprintLibrarySpec, "Hippocache.BlueprintLibrary", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
#else
DEFINE_SPEC(FHippocacheBlueprintLibrarySpec, "Hippocache.BlueprintLibrary", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
#endif

void FHippocacheBlueprintLibrarySpec::Define()
{
    Describe("Blueprint Library Basic Operations", [this]()
    {
        It("should set and get Int32 value successfully", [this]()
        {
            AddInfo(TEXT("=== Starting Int32 test ==="));
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                AddError(TEXT("Failed to setup blueprint test"));
                return;
            }
            AddInfo(TEXT("Blueprint test setup completed successfully"));
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "Int32Key";
            int32 SetValue = 12345;
            
            // Test Set operation
            AddInfo(FString::Printf(TEXT("Testing SetInt32 with value: %d"), SetValue));
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            AddInfo(FString::Printf(TEXT("SetInt32 result: Success=%s, Error=%s"), 
                SetResult.IsSuccess() ? TEXT("true") : TEXT("false"),
                *SetResult.ErrorMessage));
            TestTrue("SetInt32 should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            AddInfo(TEXT("Testing GetInt32"));
            int32 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            AddInfo(FString::Printf(TEXT("GetInt32 result: Success=%s, Value=%d"), 
                GetResult.IsSuccess() ? TEXT("true") : TEXT("false"), GetValue));
            TestTrue("GetInt32 should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            AddInfo(TEXT("=== Int32 test completed successfully ==="));
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Int64 value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "Int64Key";
            int64 SetValue = 9223372036854775807LL;
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt64(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetInt64 should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            int64 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt64(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetInt64 should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Float value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "FloatKey";
            float SetValue = 3.14159f;
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetFloat(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetFloat should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            float GetValue = 0.0f;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetFloat(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetFloat should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Double value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "DoubleKey";
            double SetValue = 2.718281828459045;
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetDouble(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetDouble should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            double GetValue = 0.0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetDouble(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetDouble should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get String value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "StringKey";
            FString SetValue = TEXT("Hello, Blueprint Cache!");
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetString(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetString should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FString GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetString(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetString should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get DateTime value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "DateTimeKey";
            FDateTime SetValue = FDateTime::Now();
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetDateTime(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetDateTime should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FDateTime GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetDateTime(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetDateTime should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Bool value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "BoolKey";
            bool SetValue = true;
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetBool(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetBool should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            bool GetValue = false;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetBool(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetBool should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Byte value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "ByteKey";
            uint8 SetValue = 255;
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetByte(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetByte should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            uint8 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetByte(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetByte should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Name value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "NameKey";
            FName SetValue = TEXT("TestNameValue");
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetName(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetName should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FName GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetName(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetName should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Vector value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "VectorKey";
            FVector SetValue = FVector(1.0f, 2.0f, 3.0f);
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetVector(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetVector should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FVector GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetVector(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetVector should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Vector2D value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "Vector2DKey";
            FVector2D SetValue = FVector2D(10.5f, 20.5f);
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetVector2D(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetVector2D should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FVector2D GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetVector2D(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetVector2D should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Vector4 value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "Vector4Key";
            FVector4 SetValue = FVector4(1.0f, 2.0f, 3.0f, 4.0f);
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetVector4(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetVector4 should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FVector4 GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetVector4(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetVector4 should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Rotator value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "RotatorKey";
            FRotator SetValue = FRotator(30.0f, 45.0f, 60.0f);
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetRotator(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetRotator should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FRotator GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetRotator(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetRotator should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Transform value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "TransformKey";
            FTransform SetValue = FTransform(FRotator(0, 45, 0), FVector(100, 200, 300), FVector(2.0f, 2.0f, 2.0f));
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetTransform(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetTransform should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FTransform GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetTransform(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetTransform should succeed", GetResult.IsSuccess());
            TestTrue("Values should match", GetValue.Equals(SetValue));
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Color value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "ColorKey";
            FColor SetValue = FColor(255, 128, 64, 255);
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetColor(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetColor should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FColor GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetColor(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetColor should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get LinearColor value successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTestCollection";
            FString TestKey = "LinearColorKey";
            FLinearColor SetValue = FLinearColor(1.0f, 0.5f, 0.25f, 1.0f);
            
            // Test Set operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetLinearColor(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetLinearColor should succeed", SetResult.IsSuccess());
            
            // Test Get operation
            FLinearColor GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetLinearColor(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetLinearColor should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });

    Describe("Blueprint Library Struct Operations", [this]()
    {
        It("should set and get struct using FInstancedStruct successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "StructTestCollection";
            FString TestKey = "StructKey";
            
            // Create test struct
            FTestStruct TestStruct;
            TestStruct.IntValue = 12345;
            TestStruct.StringValue = TEXT("Hello Struct Cache!");
            TestStruct.FloatValue = 3.14159f;
            
            // Convert to FInstancedStruct
            FInstancedStruct InstancedStruct = FInstancedStruct::Make<FTestStruct>(TestStruct);
            
            // Test SetStruct operation using Subsystem directly (since Blueprint Library doesn't expose FInstancedStruct)
            UHippocacheSubsystem* Subsystem = nullptr;
            FHippocacheResult SubsystemResult = UHippocacheSubsystem::Get(TestContext.TestWorld, Subsystem);
            TestTrue("Should get subsystem successfully", SubsystemResult.IsSuccess());
            
            if (Subsystem)
            {
                FHippocacheResult SetResult = Subsystem->SetStruct(CollectionName, TestKey, InstancedStruct);
                AddInfo(FString::Printf(TEXT("SetStruct result: Success=%s"), 
                    SetResult.IsSuccess() ? TEXT("true") : TEXT("false")));
                TestTrue("SetStruct should succeed", SetResult.IsSuccess());
                
                // Test GetStruct operation
                FInstancedStruct GetStruct;
                FHippocacheResult GetResult = Subsystem->GetStruct(CollectionName, TestKey, GetStruct);
                AddInfo(FString::Printf(TEXT("GetStruct result: Success=%s"), 
                    GetResult.IsSuccess() ? TEXT("true") : TEXT("false")));
                TestTrue("GetStruct should succeed", GetResult.IsSuccess());
                
                // Verify struct content
                if (GetResult.IsSuccess() && GetStruct.IsValid())
                {
                    const FTestStruct* RetrievedStruct = GetStruct.GetPtr<FTestStruct>();
                    TestNotNull("Retrieved struct should not be null", RetrievedStruct);
                    
                    if (RetrievedStruct)
                    {
                        TestEqual("IntValue should match", RetrievedStruct->IntValue, TestStruct.IntValue);
                        TestEqual("StringValue should match", RetrievedStruct->StringValue, TestStruct.StringValue);
                        TestEqual("FloatValue should match", RetrievedStruct->FloatValue, TestStruct.FloatValue);
                        TestTrue("Structs should be equal", *RetrievedStruct == TestStruct);
                    }
                }
            }
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get struct with TTL using C++ template functions", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "StructTTLTestCollection";
            FString TestKey = "StructTTLKey";
            
            // Create test struct
            FTestStruct TestStruct;
            TestStruct.IntValue = 99999;
            TestStruct.StringValue = TEXT("TTL Struct Test");
            TestStruct.FloatValue = 2.71828f;
            
            // Test C++ template functions via Blueprint Library
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStruct<FTestStruct>(
                TestContext.TestWorld, CollectionName, TestKey, TestStruct);
            AddInfo(FString::Printf(TEXT("SetStruct template result: Success=%s"), 
                SetResult.IsSuccess() ? TEXT("true") : TEXT("false")));
            TestTrue("SetStruct template should succeed", SetResult.IsSuccess());
            
            // Test C++ template GetStruct
            auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FTestStruct>(
                TestContext.TestWorld, CollectionName, TestKey);
            AddInfo(FString::Printf(TEXT("GetStruct template result: Success=%s"), 
                GetResult.IsSuccess() ? TEXT("true") : TEXT("false")));
            TestTrue("GetStruct template should succeed", GetResult.IsSuccess());
            
            if (GetResult.IsSuccess())
            {
                FTestStruct RetrievedStruct = GetResult.Value;
                TestEqual("IntValue should match", RetrievedStruct.IntValue, TestStruct.IntValue);
                TestEqual("StringValue should match", RetrievedStruct.StringValue, TestStruct.StringValue);
                TestEqual("FloatValue should match", RetrievedStruct.FloatValue, TestStruct.FloatValue);
                TestTrue("Structs should be equal", RetrievedStruct == TestStruct);
            }
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle struct with TTL expiration correctly", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "StructTTLExpiryTest";
            FString TestKey = "ExpiringStructKey";
            
            FTestStruct TestStruct;
            TestStruct.IntValue = 555;
            TestStruct.StringValue = TEXT("Expiring Struct");
            TestStruct.FloatValue = 1.41421f;
            
            // Set struct with very short TTL (0.1 seconds)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStructWithTTL<FTestStruct>(
                TestContext.TestWorld, CollectionName, TestKey, TestStruct, 0.1f);
            TestTrue("SetStructWithTTL should succeed", SetResult.IsSuccess());
            
            // Wait for expiration
            FPlatformProcess::Sleep(0.2f);
            
            // Try to get expired struct
            auto GetResult = UHippocacheBlueprintLibrary::GetStruct<FTestStruct>(
                TestContext.TestWorld, CollectionName, TestKey);
            TestTrue("GetStruct should fail for expired item", GetResult.IsError());
            TestEqual("Should return ItemExpired error", GetResult.Result.ErrorCode, EHippocacheErrorCode::ItemExpired);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });

    Describe("Blueprint Library Edge Cases and Error Handling", [this]()
    {
        It("should handle empty string keys appropriately", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "EdgeCaseTest";
            FString EmptyKey = TEXT("");
            int32 TestValue = 123;
            
            // Test with empty key
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                TestContext.TestWorld, CollectionName, EmptyKey, TestValue);
            TestTrue("Should return error for empty key", SetResult.IsError());
            TestEqual("Should return InvalidKey error", SetResult.ErrorCode, EHippocacheErrorCode::InvalidKey);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle very long string keys", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "LongKeyTest";
            
            // Create a very long key (1000 characters)
            FString LongKey;
            for (int32 i = 0; i < 1000; ++i)
            {
                LongKey += TEXT("A");
            }
            
            int32 TestValue = 456;
            
            // Test with very long key
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                TestContext.TestWorld, CollectionName, LongKey, TestValue);
            TestTrue("Should handle very long keys", SetResult.IsSuccess());
            
            if (SetResult.IsSuccess())
            {
                int32 GetValue = 0;
                FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                    TestContext.TestWorld, CollectionName, LongKey, GetValue);
                TestTrue("Should retrieve value with long key", GetResult.IsSuccess());
                TestEqual("Value should match", GetValue, TestValue);
            }
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle Unicode and special characters in keys", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "UnicodeKeyTest";
            
            TArray<FString> TestKeys = {
                TEXT("한글키"),                    // Korean
                TEXT("日本語キー"),               // Japanese
                TEXT("中文键"),                   // Chinese
                TEXT("ключ"),                     // Russian
                TEXT("🎮🚀💾"),                   // Emojis
                TEXT("key@#$%^&*()_+-={}[]|\\:;\"'<>,.?/"), // Special chars
                TEXT("key with spaces"),          // Spaces
                TEXT("Key.With.Dots"),           // Dots
                TEXT("key-with-dashes")          // Dashes
            };
            
            for (int32 i = 0; i < TestKeys.Num(); ++i)
            {
                const FString& TestKey = TestKeys[i];
                int32 TestValue = i * 100;
                
                FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                    TestContext.TestWorld, CollectionName, TestKey, TestValue);
                AddInfo(FString::Printf(TEXT("Testing key: %s"), *TestKey));
                TestTrue(FString::Printf(TEXT("Should handle Unicode key: %s"), *TestKey), SetResult.IsSuccess());
                
                if (SetResult.IsSuccess())
                {
                    int32 GetValue = 0;
                    FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                        TestContext.TestWorld, CollectionName, TestKey, GetValue);
                    TestTrue(FString::Printf(TEXT("Should retrieve Unicode key: %s"), *TestKey), GetResult.IsSuccess());
                    TestEqual(FString::Printf(TEXT("Value should match for key: %s"), *TestKey), GetValue, TestValue);
                }
            }
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle extreme numeric values", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "ExtremeValuesTest";
            
            // Test Int32 extremes
            TArray<int32> Int32Values = {
                TNumericLimits<int32>::Min(),
                TNumericLimits<int32>::Max(),
                0,
                -1,
                1
            };
            
            for (int32 i = 0; i < Int32Values.Num(); ++i)
            {
                FString TestKey = FString::Printf(TEXT("Int32Key_%d"), i);
                int32 TestValue = Int32Values[i];
                
                FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                    TestContext.TestWorld, CollectionName, TestKey, TestValue);
                TestTrue(FString::Printf(TEXT("Should handle Int32 value: %d"), TestValue), SetResult.IsSuccess());
                
                if (SetResult.IsSuccess())
                {
                    int32 GetValue = 0;
                    FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                        TestContext.TestWorld, CollectionName, TestKey, GetValue);
                    TestTrue(FString::Printf(TEXT("Should retrieve Int32 value: %d"), TestValue), GetResult.IsSuccess());
                    TestEqual(FString::Printf(TEXT("Int32 value should match: %d"), TestValue), GetValue, TestValue);
                }
            }
            
            // Test Float extremes
            TArray<float> FloatValues = {
                TNumericLimits<float>::Min(),
                TNumericLimits<float>::Max(),
                TNumericLimits<float>::Lowest(),
                0.0f,
                -0.0f,
                1.0f,
                -1.0f,
                FLT_EPSILON,
                -FLT_EPSILON
            };
            
            for (int32 i = 0; i < FloatValues.Num(); ++i)
            {
                FString TestKey = FString::Printf(TEXT("FloatKey_%d"), i);
                float TestValue = FloatValues[i];
                
                FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetFloat(
                    TestContext.TestWorld, CollectionName, TestKey, TestValue);
                TestTrue(FString::Printf(TEXT("Should handle Float value: %f"), TestValue), SetResult.IsSuccess());
                
                if (SetResult.IsSuccess())
                {
                    float GetValue = 0.0f;
                    FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetFloat(
                        TestContext.TestWorld, CollectionName, TestKey, GetValue);
                    TestTrue(FString::Printf(TEXT("Should retrieve Float value: %f"), TestValue), GetResult.IsSuccess());
                    TestEqual(FString::Printf(TEXT("Float value should match: %f"), TestValue), GetValue, TestValue);
                }
            }
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle very large strings", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "LargeStringTest";
            FString TestKey = "LargeStringKey";
            
            // Create a very large string (10KB)
            FString LargeString;
            const FString Pattern = TEXT("This is a test pattern that will be repeated many times to create a large string. ");
            const int32 RepeatCount = 10000 / Pattern.Len() + 1;
            
            for (int32 i = 0; i < RepeatCount; ++i)
            {
                LargeString += Pattern;
            }
            
            AddInfo(FString::Printf(TEXT("Testing with string of length: %d"), LargeString.Len()));
            
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetString(
                TestContext.TestWorld, CollectionName, TestKey, LargeString);
            TestTrue("Should handle very large strings", SetResult.IsSuccess());
            
            if (SetResult.IsSuccess())
            {
                FString GetValue;
                FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetString(
                    TestContext.TestWorld, CollectionName, TestKey, GetValue);
                TestTrue("Should retrieve large string", GetResult.IsSuccess());
                TestEqual("Large string should match", GetValue, LargeString);
            }
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle rapid successive operations", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "RapidOperationsTest";
            const int32 OperationCount = 100;
            
            // Rapid set operations
            for (int32 i = 0; i < OperationCount; ++i)
            {
                FString TestKey = FString::Printf(TEXT("RapidKey_%d"), i);
                int32 TestValue = i * 2;
                
                FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                    TestContext.TestWorld, CollectionName, TestKey, TestValue);
                TestTrue(FString::Printf(TEXT("Rapid set operation %d should succeed"), i), SetResult.IsSuccess());
            }
            
            // Rapid get operations
            for (int32 i = 0; i < OperationCount; ++i)
            {
                FString TestKey = FString::Printf(TEXT("RapidKey_%d"), i);
                int32 ExpectedValue = i * 2;
                int32 GetValue = 0;
                
                FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                    TestContext.TestWorld, CollectionName, TestKey, GetValue);
                TestTrue(FString::Printf(TEXT("Rapid get operation %d should succeed"), i), GetResult.IsSuccess());
                TestEqual(FString::Printf(TEXT("Rapid operation %d value should match"), i), GetValue, ExpectedValue);
            }
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle mixed data type operations in same collection", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "MixedTypeTest";
            
            // Store different types with similar keys (including new variant types)
            FHippocacheResult SetInt = UHippocacheBlueprintLibrary::SetInt32(
                TestContext.TestWorld, CollectionName, "Value1", 12345);
            FHippocacheResult SetFloat = UHippocacheBlueprintLibrary::SetFloat(
                TestContext.TestWorld, CollectionName, "Value2", 3.14159f);
            FHippocacheResult SetString = UHippocacheBlueprintLibrary::SetString(
                TestContext.TestWorld, CollectionName, "Value3", TEXT("Mixed Types Test"));
            FHippocacheResult SetDateTime = UHippocacheBlueprintLibrary::SetDateTime(
                TestContext.TestWorld, CollectionName, "Value4", FDateTime::Now());
            FHippocacheResult SetBool = UHippocacheBlueprintLibrary::SetBool(
                TestContext.TestWorld, CollectionName, "Value5", true);
            FHippocacheResult SetVector = UHippocacheBlueprintLibrary::SetVector(
                TestContext.TestWorld, CollectionName, "Value6", FVector(1.0f, 2.0f, 3.0f));
            FHippocacheResult SetColor = UHippocacheBlueprintLibrary::SetColor(
                TestContext.TestWorld, CollectionName, "Value7", FColor(255, 128, 64, 255));
            FHippocacheResult SetName = UHippocacheBlueprintLibrary::SetName(
                TestContext.TestWorld, CollectionName, "Value8", FName("TestName"));
            FHippocacheResult SetByte = UHippocacheBlueprintLibrary::SetByte(
                TestContext.TestWorld, CollectionName, "Value9", uint8(200));
            
            TestTrue("Mixed type Int32 should succeed", SetInt.IsSuccess());
            TestTrue("Mixed type Float should succeed", SetFloat.IsSuccess());
            TestTrue("Mixed type String should succeed", SetString.IsSuccess());
            TestTrue("Mixed type DateTime should succeed", SetDateTime.IsSuccess());
            TestTrue("Mixed type Bool should succeed", SetBool.IsSuccess());
            TestTrue("Mixed type Vector should succeed", SetVector.IsSuccess());
            TestTrue("Mixed type Color should succeed", SetColor.IsSuccess());
            TestTrue("Mixed type Name should succeed", SetName.IsSuccess());
            TestTrue("Mixed type Byte should succeed", SetByte.IsSuccess());
            
            // Retrieve and verify each type
            int32 GetIntValue = 0;
            float GetFloatValue = 0.0f;
            FString GetStringValue;
            FDateTime GetDateTimeValue;
            bool GetBoolValue = false;
            FVector GetVectorValue;
            FColor GetColorValue;
            FName GetNameValue;
            uint8 GetByteValue = 0;
            
            FHippocacheResult GetInt = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, "Value1", GetIntValue);
            FHippocacheResult GetFloat = UHippocacheBlueprintLibrary::GetFloat(
                TestContext.TestWorld, CollectionName, "Value2", GetFloatValue);
            FHippocacheResult GetString = UHippocacheBlueprintLibrary::GetString(
                TestContext.TestWorld, CollectionName, "Value3", GetStringValue);
            FHippocacheResult GetDateTime = UHippocacheBlueprintLibrary::GetDateTime(
                TestContext.TestWorld, CollectionName, "Value4", GetDateTimeValue);
            FHippocacheResult GetBool = UHippocacheBlueprintLibrary::GetBool(
                TestContext.TestWorld, CollectionName, "Value5", GetBoolValue);
            FHippocacheResult GetVector = UHippocacheBlueprintLibrary::GetVector(
                TestContext.TestWorld, CollectionName, "Value6", GetVectorValue);
            FHippocacheResult GetColor = UHippocacheBlueprintLibrary::GetColor(
                TestContext.TestWorld, CollectionName, "Value7", GetColorValue);
            FHippocacheResult GetName = UHippocacheBlueprintLibrary::GetName(
                TestContext.TestWorld, CollectionName, "Value8", GetNameValue);
            FHippocacheResult GetByte = UHippocacheBlueprintLibrary::GetByte(
                TestContext.TestWorld, CollectionName, "Value9", GetByteValue);
            
            TestTrue("Mixed type Int32 retrieval should succeed", GetInt.IsSuccess());
            TestTrue("Mixed type Float retrieval should succeed", GetFloat.IsSuccess());
            TestTrue("Mixed type String retrieval should succeed", GetString.IsSuccess());
            TestTrue("Mixed type DateTime retrieval should succeed", GetDateTime.IsSuccess());
            TestTrue("Mixed type Bool retrieval should succeed", GetBool.IsSuccess());
            TestTrue("Mixed type Vector retrieval should succeed", GetVector.IsSuccess());
            TestTrue("Mixed type Color retrieval should succeed", GetColor.IsSuccess());
            TestTrue("Mixed type Name retrieval should succeed", GetName.IsSuccess());
            TestTrue("Mixed type Byte retrieval should succeed", GetByte.IsSuccess());
            
            TestEqual("Mixed type Int32 value should match", GetIntValue, 12345);
            TestEqual("Mixed type Float value should match", GetFloatValue, 3.14159f);
            TestEqual("Mixed type String value should match", GetStringValue, TEXT("Mixed Types Test"));
            TestEqual("Mixed type Bool value should match", GetBoolValue, true);
            TestEqual("Mixed type Vector value should match", GetVectorValue, FVector(1.0f, 2.0f, 3.0f));
            TestEqual("Mixed type Color value should match", GetColorValue, FColor(255, 128, 64, 255));
            TestEqual("Mixed type Name value should match", GetNameValue, FName("TestName"));
            TestEqual("Mixed type Byte value should match", GetByteValue, uint8(200));
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });

    Describe("Blueprint Library Weird Struct Edge Cases", [this]()
    {
        It("should handle empty structs", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestEmptyStruct(TestContext.TestWorld, this);
            TestTrue("Empty struct test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle massive structs with lots of data", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestHugeStruct(TestContext.TestWorld, this);
            TestTrue("Huge struct test should handle gracefully", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle deeply nested structures", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestNestedStructs(TestContext.TestWorld, this);
            TestTrue("Nested struct test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle circular reference structures", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestCircularStructs(TestContext.TestWorld, this);
            TestTrue("Circular struct test should handle gracefully", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle structs with UObject pointers (dangerous)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestObjectPointerStructs(TestContext.TestWorld, this);
            TestTrue("Object pointer struct test should handle gracefully", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle structs with delegates", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestDelegateStructs(TestContext.TestWorld, this);
            TestTrue("Delegate struct test should handle gracefully", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle structs with special Unreal types", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestSpecialTypesStructs(TestContext.TestWorld, this);
            TestTrue("Special types struct test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle inheritance hierarchies", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestInheritanceStructs(TestContext.TestWorld, this);
            TestTrue("Inheritance struct test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle packed/bitfield structs", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestPackedStructs(TestContext.TestWorld, this);
            TestTrue("Packed struct test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle DataTable row structs", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestDataTableRowStructs(TestContext.TestWorld, this);
            TestTrue("DataTable row struct test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle concurrent struct access", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestConcurrentStructAccess(TestContext.TestWorld, this);
            TestTrue("Concurrent struct access test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle struct modification after storage", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestStructModificationAfterStorage(TestContext.TestWorld, this);
            TestTrue("Struct modification after storage test should succeed", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle structs with weird memory patterns", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestStructMemoryPatterns(TestContext.TestWorld, this);
            TestTrue("Struct memory patterns test should handle gracefully", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle struct serialization edge cases", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestStructSerializationEdgeCases(TestContext.TestWorld, this);
            TestTrue("Struct serialization edge cases test should handle gracefully", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle uninitialized struct data", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            bool bResult = FWeirdStructTestRunner::TestUninitializedStructData(TestContext.TestWorld, this);
            TestTrue("Uninitialized struct data test should handle gracefully", bResult);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });


    Describe("Blueprint Library TTL Operations", [this]()
    {
        It("should set value with TTL and retrieve before expiration", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTTLTest";
            FString TestKey = "TTLKey";
            int32 SetValue = 777;
            float TTLSeconds = 5.0f;
            
            // Test SetInt32WithTTL operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32WithTTL(
                TestContext.TestWorld, CollectionName, TestKey, SetValue, TTLSeconds);
            TestTrue("SetInt32WithTTL should succeed", SetResult.IsSuccess());
            
            // Test Get operation before expiration
            int32 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetInt32 should succeed before expiration", GetResult.IsSuccess());
            TestEqual("Value should be correct", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set String with TTL and retrieve before expiration", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTTLTest";
            FString TestKey = "StringTTLKey";
            FString SetValue = TEXT("TTL Test String");
            float TTLSeconds = 5.0f;
            
            // Test SetStringWithTTL operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetStringWithTTL(
                TestContext.TestWorld, CollectionName, TestKey, SetValue, TTLSeconds);
            TestTrue("SetStringWithTTL should succeed", SetResult.IsSuccess());
            
            // Test Get operation before expiration
            FString GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetString(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetString should succeed before expiration", GetResult.IsSuccess());
            TestEqual("Value should be correct", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set Vector with TTL and retrieve before expiration", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTTLTest";
            FString TestKey = "VectorTTLKey";
            FVector SetValue = FVector(10.0f, 20.0f, 30.0f);
            float TTLSeconds = 5.0f;
            
            // Test SetVectorWithTTL operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetVectorWithTTL(
                TestContext.TestWorld, CollectionName, TestKey, SetValue, TTLSeconds);
            TestTrue("SetVectorWithTTL should succeed", SetResult.IsSuccess());
            
            // Test Get operation before expiration
            FVector GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetVector(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetVector should succeed before expiration", GetResult.IsSuccess());
            TestEqual("Value should be correct", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set Bool with TTL and retrieve before expiration", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTTLTest";
            FString TestKey = "BoolTTLKey";
            bool SetValue = true;
            float TTLSeconds = 5.0f;
            
            // Test SetBoolWithTTL operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetBoolWithTTL(
                TestContext.TestWorld, CollectionName, TestKey, SetValue, TTLSeconds);
            TestTrue("SetBoolWithTTL should succeed", SetResult.IsSuccess());
            
            // Test Get operation before expiration
            bool GetValue = false;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetBool(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetBool should succeed before expiration", GetResult.IsSuccess());
            TestEqual("Value should be correct", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set Color with TTL and retrieve before expiration", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "BlueprintTTLTest";
            FString TestKey = "ColorTTLKey";
            FColor SetValue = FColor(200, 100, 50, 255);
            float TTLSeconds = 5.0f;
            
            // Test SetColorWithTTL operation
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetColorWithTTL(
                TestContext.TestWorld, CollectionName, TestKey, SetValue, TTLSeconds);
            TestTrue("SetColorWithTTL should succeed", SetResult.IsSuccess());
            
            // Test Get operation before expiration
            FColor GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetColor(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetColor should succeed before expiration", GetResult.IsSuccess());
            TestEqual("Value should be correct", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });

    Describe("Blueprint Library C++ Only Types", [this]()
    {
        It("should set and get Int8 value successfully (C++ only)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "CppOnlyTestCollection";
            FString TestKey = "Int8Key";
            int8 SetValue = -128;
            
            // Test Set operation (C++ only)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt8(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetInt8 should succeed", SetResult.IsSuccess());
            
            // Test Get operation (C++ only)
            int8 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt8(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetInt8 should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Quat value successfully (C++ only)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "CppOnlyTestCollection";
            FString TestKey = "QuatKey";
            FQuat SetValue = FQuat::MakeFromEuler(FVector(45.0f, 90.0f, 180.0f));
            
            // Test Set operation (C++ only)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetQuat(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetQuat should succeed", SetResult.IsSuccess());
            
            // Test Get operation (C++ only)
            FQuat GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetQuat(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetQuat should succeed", GetResult.IsSuccess());
            TestTrue("Values should match", GetValue.Equals(SetValue));
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Timespan value successfully (C++ only)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "CppOnlyTestCollection";
            FString TestKey = "TimespanKey";
            FTimespan SetValue = FTimespan::FromMinutes(30);
            
            // Test Set operation (C++ only)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetTimespan(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetTimespan should succeed", SetResult.IsSuccess());
            
            // Test Get operation (C++ only)
            FTimespan GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetTimespan(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetTimespan should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set and get Guid value successfully (C++ only)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "CppOnlyTestCollection";
            FString TestKey = "GuidKey";
            FGuid SetValue = FGuid::NewGuid();
            
            // Test Set operation (C++ only)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetGuid(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetGuid should succeed", SetResult.IsSuccess());
            
            // Test Get operation (C++ only)
            FGuid GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetGuid(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetGuid should succeed", GetResult.IsSuccess());
            TestEqual("Values should match", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set Int8 with TTL successfully (C++ only)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "CppOnlyTTLTest";
            FString TestKey = "Int8TTLKey";
            int8 SetValue = 127;
            float TTLSeconds = 5.0f;
            
            // Test SetInt8WithTTL operation (C++ only)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt8WithTTL(
                TestContext.TestWorld, CollectionName, TestKey, SetValue, TTLSeconds);
            TestTrue("SetInt8WithTTL should succeed", SetResult.IsSuccess());
            
            // Test Get operation before expiration (C++ only)
            int8 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt8(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetInt8 should succeed before expiration", GetResult.IsSuccess());
            TestEqual("Value should be correct", GetValue, SetValue);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should set Quat with TTL successfully (C++ only)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "CppOnlyTTLTest";
            FString TestKey = "QuatTTLKey";
            FQuat SetValue = FQuat::Identity;
            float TTLSeconds = 5.0f;
            
            // Test SetQuatWithTTL operation (C++ only)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetQuatWithTTL(
                TestContext.TestWorld, CollectionName, TestKey, SetValue, TTLSeconds);
            TestTrue("SetQuatWithTTL should succeed", SetResult.IsSuccess());
            
            // Test Get operation before expiration (C++ only)
            FQuat GetValue;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetQuat(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetQuat should succeed before expiration", GetResult.IsSuccess());
            TestTrue("Value should be correct", GetValue.Equals(SetValue));
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });

    Describe("Universal Hippoo & Hippop Functions", [this]()
    {
        It("should store and retrieve primitive types automatically with Hippoo/Hippop", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "HippooTestCollection";
            
            // Test various primitive types
            AddInfo(TEXT("Testing Hippoo/Hippop with primitive types"));
            
            // Int32
            FHippocacheResult SetResult1 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_int32", 12345);
            TestTrue("Hippoo Int32 should succeed", SetResult1.IsSuccess());
            
            int32 GetInt32 = 0;
            FHippocacheResult GetResult1 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_int32", GetInt32);
            TestTrue("Hippop Int32 should succeed", GetResult1.IsSuccess());
            TestEqual("Int32 values should match", GetInt32, 12345);
            
            // FString
            FHippocacheResult SetResult2 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_string", FString("Hello Hippoo!"));
            TestTrue("Hippoo FString should succeed", SetResult2.IsSuccess());
            
            FString GetString;
            FHippocacheResult GetResult2 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_string", GetString);
            TestTrue("Hippop FString should succeed", GetResult2.IsSuccess());
            TestEqual("FString values should match", GetString, FString("Hello Hippoo!"));
            
            // Bool
            FHippocacheResult SetResult3 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_bool", true);
            TestTrue("Hippoo Bool should succeed", SetResult3.IsSuccess());
            
            bool GetBool = false;
            FHippocacheResult GetResult3 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_bool", GetBool);
            TestTrue("Hippop Bool should succeed", GetResult3.IsSuccess());
            TestEqual("Bool values should match", GetBool, true);
            
            // Float
            FHippocacheResult SetResult4 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_float", 3.14159f);
            TestTrue("Hippoo Float should succeed", SetResult4.IsSuccess());
            
            float GetFloat = 0.0f;
            FHippocacheResult GetResult4 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_float", GetFloat);
            TestTrue("Hippop Float should succeed", GetResult4.IsSuccess());
            TestEqual("Float values should match", GetFloat, 3.14159f);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should store and retrieve struct types automatically with Hippoo/Hippop", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "HippooStructTestCollection";
            
            AddInfo(TEXT("Testing Hippoo/Hippop with struct types"));
            
            // FVector (UE struct)
            FVector TestVector(100.0f, 200.0f, 300.0f);
            FHippocacheResult SetResult1 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_vector", TestVector);
            TestTrue("Hippoo FVector should succeed", SetResult1.IsSuccess());
            
            FVector GetVector;
            FHippocacheResult GetResult1 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_vector", GetVector);
            TestTrue("Hippop FVector should succeed", GetResult1.IsSuccess());
            TestEqual("FVector values should match", GetVector, TestVector);
            
            // FRotator (UE struct)
            FRotator TestRotator(45.0f, 90.0f, 180.0f);
            FHippocacheResult SetResult2 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_rotator", TestRotator);
            TestTrue("Hippoo FRotator should succeed", SetResult2.IsSuccess());
            
            FRotator GetRotator;
            FHippocacheResult GetResult2 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_rotator", GetRotator);
            TestTrue("Hippop FRotator should succeed", GetResult2.IsSuccess());
            TestEqual("FRotator values should match", GetRotator, TestRotator);
            
            // FColor (UE struct)
            FColor TestColor(255, 128, 64, 255);
            FHippocacheResult SetResult3 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_color", TestColor);
            TestTrue("Hippoo FColor should succeed", SetResult3.IsSuccess());
            
            FColor GetColor;
            FHippocacheResult GetResult3 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_color", GetColor);
            TestTrue("Hippop FColor should succeed", GetResult3.IsSuccess());
            TestEqual("FColor values should match", GetColor, TestColor);
            
            // Custom struct
            FTestStruct TestStruct;
            TestStruct.IntValue = 99999;
            TestStruct.StringValue = TEXT("Hippoo Struct Test");
            TestStruct.FloatValue = 2.71828f;
            
            FHippocacheResult SetResult4 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "test_custom_struct", TestStruct);
            TestTrue("Hippoo FTestStruct should succeed", SetResult4.IsSuccess());
            
            FTestStruct GetStruct;
            FHippocacheResult GetResult4 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "test_custom_struct", GetStruct);
            TestTrue("Hippop FTestStruct should succeed", GetResult4.IsSuccess());
            TestEqual("FTestStruct IntValue should match", GetStruct.IntValue, TestStruct.IntValue);
            TestEqual("FTestStruct StringValue should match", GetStruct.StringValue, TestStruct.StringValue);
            TestEqual("FTestStruct FloatValue should match", GetStruct.FloatValue, TestStruct.FloatValue);
            TestTrue("FTestStruct should be equal", GetStruct == TestStruct);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle TTL correctly with Hippoo (overloaded version)", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "HippooTTLTest";
            
            AddInfo(TEXT("Testing Hippoo TTL functionality"));
            
            // Test primitive with TTL
            FHippocacheResult SetResult1 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "ttl_int", 42, 10.0f); // 10 seconds TTL
            TestTrue("Hippoo TTL Int32 should succeed", SetResult1.IsSuccess());
            
            // Should be retrievable immediately
            int32 GetInt = 0;
            FHippocacheResult GetResult1 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "ttl_int", GetInt);
            TestTrue("Hippop TTL Int32 should succeed before expiration", GetResult1.IsSuccess());
            TestEqual("TTL Int32 value should match", GetInt, 42);
            
            // Test struct with TTL
            FVector TestVector(1.0f, 2.0f, 3.0f);
            FHippocacheResult SetResult2 = UHippocacheBlueprintLibrary::Hippoo(
                TestContext.TestWorld, CollectionName, "ttl_vector", TestVector, 10.0f); // 10 seconds TTL
            TestTrue("Hippoo TTL FVector should succeed", SetResult2.IsSuccess());
            
            // Should be retrievable immediately
            FVector GetVector;
            FHippocacheResult GetResult2 = UHippocacheBlueprintLibrary::Hippop(
                TestContext.TestWorld, CollectionName, "ttl_vector", GetVector);
            TestTrue("Hippop TTL FVector should succeed before expiration", GetResult2.IsSuccess());
            TestEqual("TTL FVector value should match", GetVector, TestVector);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle mixed types in same collection with Hippoo/Hippop", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "HippooMixedTest";
            
            AddInfo(TEXT("Testing Hippoo/Hippop with mixed types in same collection"));
            
            // Store different types with Hippoo
            UHippocacheBlueprintLibrary::Hippoo(TestContext.TestWorld, CollectionName, "data1", 123);
            UHippocacheBlueprintLibrary::Hippoo(TestContext.TestWorld, CollectionName, "data2", FString("Mixed"));
            UHippocacheBlueprintLibrary::Hippoo(TestContext.TestWorld, CollectionName, "data3", true);
            UHippocacheBlueprintLibrary::Hippoo(TestContext.TestWorld, CollectionName, "data4", FVector(1, 2, 3));
            UHippocacheBlueprintLibrary::Hippoo(TestContext.TestWorld, CollectionName, "data5", FColor::Red);
            
            // Retrieve and verify each type with Hippop
            int32 GetInt = 0;
            FString GetString;
            bool GetBool = false;
            FVector GetVector;
            FColor GetColor;
            
            FHippocacheResult Result1 = UHippocacheBlueprintLibrary::Hippop(TestContext.TestWorld, CollectionName, "data1", GetInt);
            FHippocacheResult Result2 = UHippocacheBlueprintLibrary::Hippop(TestContext.TestWorld, CollectionName, "data2", GetString);
            FHippocacheResult Result3 = UHippocacheBlueprintLibrary::Hippop(TestContext.TestWorld, CollectionName, "data3", GetBool);
            FHippocacheResult Result4 = UHippocacheBlueprintLibrary::Hippop(TestContext.TestWorld, CollectionName, "data4", GetVector);
            FHippocacheResult Result5 = UHippocacheBlueprintLibrary::Hippop(TestContext.TestWorld, CollectionName, "data5", GetColor);
            
            TestTrue("Mixed type Int32 retrieval should succeed", Result1.IsSuccess());
            TestTrue("Mixed type String retrieval should succeed", Result2.IsSuccess());
            TestTrue("Mixed type Bool retrieval should succeed", Result3.IsSuccess());
            TestTrue("Mixed type Vector retrieval should succeed", Result4.IsSuccess());
            TestTrue("Mixed type Color retrieval should succeed", Result5.IsSuccess());
            
            TestEqual("Mixed type Int32 value should match", GetInt, 123);
            TestEqual("Mixed type String value should match", GetString, FString("Mixed"));
            TestEqual("Mixed type Bool value should match", GetBool, true);
            TestEqual("Mixed type Vector value should match", GetVector, FVector(1, 2, 3));
            TestEqual("Mixed type Color value should match", GetColor, FColor::Red);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should handle type mismatches gracefully with Hippop", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "HippooTypeMismatchTest";
            
            AddInfo(TEXT("Testing Hippoo/Hippop type mismatch handling"));
            
            // Store an int32
            UHippocacheBlueprintLibrary::Hippoo(TestContext.TestWorld, CollectionName, "test_key", 12345);
            
            // Try to retrieve as wrong type
            FString WrongString;
            FHippocacheResult Result1 = UHippocacheBlueprintLibrary::Hippop(TestContext.TestWorld, CollectionName, "test_key", WrongString);
            TestTrue("Type mismatch should return error", Result1.IsError());
            TestEqual("Should return TypeMismatch error", Result1.ErrorCode, EHippocacheErrorCode::TypeMismatch);
            
            // Store a struct
            FVector TestVector(1, 2, 3);
            UHippocacheBlueprintLibrary::Hippoo(TestContext.TestWorld, CollectionName, "vector_key", TestVector);
            
            // Try to retrieve struct as primitive
            int32 WrongInt = 0;
            FHippocacheResult Result2 = UHippocacheBlueprintLibrary::Hippop(TestContext.TestWorld, CollectionName, "vector_key", WrongInt);
            TestTrue("Struct-to-primitive mismatch should return error", Result2.IsError());
            TestEqual("Should return TypeMismatch error", Result2.ErrorCode, EHippocacheErrorCode::TypeMismatch);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });

    Describe("Blueprint Library Error Handling", [this]()
    {
        It("should return error for null world context", [this]()
        {
            FName CollectionName = "ErrorTest";
            FString TestKey = "TestKey";
            int32 SetValue = 123;
            
            // Test with null world context
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                nullptr, CollectionName, TestKey, SetValue);
            TestTrue("Should return error for null context", SetResult.IsError());
            
            // Test Get with null world context
            int32 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                nullptr, CollectionName, TestKey, GetValue);
            TestTrue("Should return error for null context", GetResult.IsError());
        });

        It("should return error for invalid collection name", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FString TestKey = "TestKey";
            int32 SetValue = 123;
            
            // Test with invalid collection name (NAME_None)
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                TestContext.TestWorld, NAME_None, TestKey, SetValue);
            TestTrue("Should return error for invalid collection", SetResult.IsError());
            TestEqual("Should return InvalidCollection error", SetResult.ErrorCode, EHippocacheErrorCode::InvalidCollection);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should return error for empty key", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "ErrorTest";
            FString EmptyKey = "";
            int32 SetValue = 123;
            
            // Test with empty key
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                TestContext.TestWorld, CollectionName, EmptyKey, SetValue);
            TestTrue("Should return error for empty key", SetResult.IsError());
            TestEqual("Should return InvalidKey error", SetResult.ErrorCode, EHippocacheErrorCode::InvalidKey);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should return error for non-existent key", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "ErrorTest";
            FString NonExistentKey = "NonExistentKey";
            
            // Test Get with non-existent key
            int32 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, NonExistentKey, GetValue);
            TestTrue("Should return error for non-existent key", GetResult.IsError());
            TestEqual("Should return ItemNotFound error", GetResult.ErrorCode, EHippocacheErrorCode::ItemNotFound);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });

    Describe("Blueprint Library Management Operations", [this]()
    {
        It("should remove items successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "ManagementTest";
            FString TestKey = "RemoveTestKey";
            int32 SetValue = 999;
            
            // Set a value first
            FHippocacheResult SetResult = UHippocacheBlueprintLibrary::SetInt32(
                TestContext.TestWorld, CollectionName, TestKey, SetValue);
            TestTrue("SetInt32 should succeed", SetResult.IsSuccess());
            
            // Verify it exists
            int32 GetValue = 0;
            FHippocacheResult GetResult = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("GetInt32 should succeed", GetResult.IsSuccess());
            TestEqual("Value should match", GetValue, SetValue);
            
            // Remove the item
            FHippocacheResult RemoveResult = UHippocacheBlueprintLibrary::Remove(
                TestContext.TestWorld, CollectionName, TestKey);
            TestTrue("Remove should succeed", RemoveResult.IsSuccess());
            
            // Verify it's gone
            FHippocacheResult GetAfterRemove = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, TestKey, GetValue);
            TestTrue("Should return error after removal", GetAfterRemove.IsError());
            TestEqual("Should return ItemNotFound error", GetAfterRemove.ErrorCode, EHippocacheErrorCode::ItemNotFound);
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });

        It("should clear collection successfully", [this]()
        {
            FHippocacheBlueprintTestContext TestContext;
            FHippocacheBlueprintTestHelper TestHelper;
            if (!TestHelper.SetupBlueprintTest(TestContext, this))
            {
                return;
            }
            
            FName CollectionName = "ClearTest";
            
            // Set multiple values
            UHippocacheBlueprintLibrary::SetInt32(TestContext.TestWorld, CollectionName, "Key1", 1);
            UHippocacheBlueprintLibrary::SetInt32(TestContext.TestWorld, CollectionName, "Key2", 2);
            UHippocacheBlueprintLibrary::SetString(TestContext.TestWorld, CollectionName, "Key3", TEXT("Test"));
            
            // Clear the collection
            FHippocacheResult ClearResult = UHippocacheBlueprintLibrary::Clear(
                TestContext.TestWorld, CollectionName);
            TestTrue("Clear should succeed", ClearResult.IsSuccess());
            
            // Verify all items are gone
            int32 GetValue = 0;
            FHippocacheResult GetResult1 = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, "Key1", GetValue);
            TestTrue("Key1 should not exist after clear", GetResult1.IsError());
            
            FHippocacheResult GetResult2 = UHippocacheBlueprintLibrary::GetInt32(
                TestContext.TestWorld, CollectionName, "Key2", GetValue);
            TestTrue("Key2 should not exist after clear", GetResult2.IsError());
            
            FString GetString;
            FHippocacheResult GetResult3 = UHippocacheBlueprintLibrary::GetString(
                TestContext.TestWorld, CollectionName, "Key3", GetString);
            TestTrue("Key3 should not exist after clear", GetResult3.IsError());
            
            TestHelper.CleanupBlueprintTest(TestContext);
        });
    });
}

#endif // WITH_DEV_AUTOMATION_TESTS