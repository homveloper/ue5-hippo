#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "HippocacheSubsystem.h"
#include "HippocacheBlueprintLibrary.h"
#include "HippocacheClient.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
// #include "Engine/LocalPlayer.h" // Not needed for GameInstanceSubsystem
#include "Misc/Timespan.h"
#include "HAL/PlatformProcess.h"
#include "InstancedStruct.h"
#include "Tests/TestStructs.h"
#include "Tests/AutomationCommon.h"
#include "Runtime/Launch/Resources/Version.h"

#if 0 // Disabled - Blueprint Library tests cover all functionality
#if WITH_DEV_AUTOMATION_TESTS

// Test context for managing test dependencies
struct FHippocacheTestContext
{
	UHippocacheSubsystem* Subsystem = nullptr;
	
	bool IsValid() const
	{
		return Subsystem != nullptr;
	}
	
	void Clear()
	{
		Subsystem = nullptr;
	}
};

// Helper class for test setup - create new instance for each test
class FHippocacheTestHelper
{
public:
	bool SetupSubsystemTest(FHippocacheTestContext& Context, FAutomationSpecBase* TestSpec)
	{
		// Create a mock subsystem for testing
		Context.Subsystem = NewObject<UHippocacheSubsystem>();
		if (!Context.Subsystem)
		{
			TestSpec->AddError(TEXT("Failed to create Hippocache subsystem"));
			return false;
		}
		
		// Clear all clients to start fresh
		Context.Subsystem->ClearAllClients();
		return true;
	}
	
	void CleanupSubsystemTest(FHippocacheTestContext& Context)
	{
		if (Context.Subsystem)
		{
			Context.Subsystem->ClearAllClients();
		}
		
		Context.Clear();
	}
};

// ApplicationContextMask is deprecated in UE 5.6+, use conditional compilation for compatibility
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6
DEFINE_SPEC(FHippocacheSubsystemSpec, "Hippocache.Subsystem", 
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
#else
DEFINE_SPEC(FHippocacheSubsystemSpec, "Hippocache.Subsystem", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
#endif

void FHippocacheSubsystemSpec::Define()
{

	Describe("Error Handling - Subsystem Access", [this]()
	{
		// These tests don't need subsystem setup since they test static functions
		It("should return error for null WorldContextObject", [this]()
		{
			UHippocacheSubsystem* OutSubsystem = nullptr;
			FHippocacheResult Result = UHippocacheSubsystem::Get(static_cast<const UObject*>(nullptr), OutSubsystem);
			TestTrue("Should return error for null context", Result.IsError());
			TestEqual("Should return InvalidValue error", Result.ErrorCode, EHippocacheErrorCode::InvalidValue);
			TestNull("OutSubsystem should be null", OutSubsystem);
		});

		It("should handle invalid world context gracefully", [this]()
		{
			// Create an object that doesn't have valid world context
			UObject* InvalidContext = NewObject<UObject>();
			
			UHippocacheSubsystem* OutSubsystem = nullptr;
			FHippocacheResult Result = UHippocacheSubsystem::Get(InvalidContext, OutSubsystem);
			TestTrue("Should return error for invalid context", Result.IsError());
		});
	});

	Describe("Client Management with Error Handling", [this]()
	{
		It("should create client successfully", [this]()
		{
			FHippocacheTestContext TestContext;
			FHippocacheTestHelper TestHelper;
			if (!TestHelper.SetupSubsystemTest(TestContext, this))
			{
				return; // Error already logged by helper
			}
			
			FName CollectionName = "TestCollection";
			FHippocacheClientHandle Handle;
			FHippocacheResult Result = TestContext.Subsystem->CreateOrGetClient(CollectionName, Handle);
			
			TestTrue("CreateOrGetClient should succeed", Result.IsSuccess());
			TestTrue("Handle should be valid", Handle.IsValid());
			
			TestHelper.CleanupSubsystemTest(TestContext);
		});

		It("should return error for invalid collection name", [this]()
		{
			FHippocacheTestContext TestContext;
			FHippocacheTestHelper TestHelper;
			if (!TestHelper.SetupSubsystemTest(TestContext, this))
			{
				return;
			}
			
			FHippocacheClientHandle Handle;
			FHippocacheResult Result = TestContext.Subsystem->CreateOrGetClient(NAME_None, Handle);
			
			TestTrue("Should return error for None collection", Result.IsError());
			TestEqual("Should return InvalidCollection error", Result.ErrorCode, EHippocacheErrorCode::InvalidCollection);
			
			TestHelper.CleanupSubsystemTest(TestContext);
		});

		It("should remove client successfully", [this]()
		{
			FHippocacheTestContext TestContext;
			FHippocacheTestHelper TestHelper;
			if (!TestHelper.SetupSubsystemTest(TestContext, this))
			{
				return;
			}
			
			FName CollectionName = "TestCollection";
			FHippocacheClientHandle Handle;
			TestContext.Subsystem->CreateOrGetClient(CollectionName, Handle);
			
			FHippocacheResult Result = TestContext.Subsystem->RemoveClient(CollectionName);
			TestTrue("RemoveClient should succeed", Result.IsSuccess());
			
			TestHelper.CleanupSubsystemTest(TestContext);
		});

		It("should return error when removing non-existent client", [this]()
		{
			FHippocacheTestContext TestContext;
			FHippocacheTestHelper TestHelper;
			if (!TestHelper.SetupSubsystemTest(TestContext, this))
			{
				return;
			}
			
			FHippocacheResult Result = TestContext.Subsystem->RemoveClient("NonExistentCollection");
			TestTrue("Should return error for non-existent client", Result.IsError());
			TestEqual("Should return ItemNotFound error", Result.ErrorCode, EHippocacheErrorCode::ItemNotFound);
			
			TestHelper.CleanupSubsystemTest(TestContext);
		});
	});

}


#endif // WITH_DEV_AUTOMATION_TESTS
#endif // Disabled subsystem tests