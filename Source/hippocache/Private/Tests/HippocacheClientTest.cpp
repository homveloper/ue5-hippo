#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "HippocacheSubsystem.h"
#include "TestStructs.h"

//#if WITH_DEV_AUTOMATION_TESTS
//
//DEFINE_SPEC(FHippocacheClientSpec, "Hippocache.Client", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
//
//void FHippocacheClientSpec::Define()
//{
//	Describe("UHippocacheSubsystem with FHippocacheClient", [this]()
//	{
//		UHippocacheSubsystem* Subsystem = nullptr;
//		FHippocacheClientHandle ClientHandle;
//
//		BeforeEach([this, Subsystem, ClientHandle]()
//		{
//			// Setup for each test
//			Subsystem = UHippocacheSubsystem::Get(this);
//			TestNotNull("Subsystem should be valid", Subsystem);
//			ClientHandle = Subsystem->CreateOrGetClient(TEXT("TestClient"));
//			TestTrue("ClientHandle should be valid", ClientHandle.IsValid());
//		});
//
//		AfterEach([this]()
//		{
//			// Teardown for each test
//			Subsystem->RemoveClient(TEXT("TestClient"));
//		});
//
//		It("should store and retrieve a simple struct", [this]()
//		{
//			FTestStruct OriginalValue = { 123, TEXT("Hello Hippocache") };
//			ClientHandle.GetClient().SetStruct(TEXT("TestKey"), OriginalValue);
//
//			FTestStruct RetrievedValue;
//			bool bSuccess = ClientHandle.GetClient().GetStruct(TEXT("TestKey"), RetrievedValue);
//
//			TestTrue("Get should succeed", bSuccess);
//			TestTrue("Retrieved value should match original", OriginalValue == RetrievedValue);
//		});
//
//		It("should return false for a non-existent key", [this]()
//		{
//			FTestStruct RetrievedValue;
//			bool bSuccess = ClientHandle.GetClient().GetStruct(TEXT("NonExistentKey"), RetrievedValue);
//
//			TestFalse("Get should fail for non-existent key", bSuccess);
//		});
//
//		It("should store and retrieve an int32", [this]()
//		{
//			int32 OriginalValue = 42;
//			ClientHandle.GetClient().SetInt32(TEXT("IntKey"), OriginalValue);
//
//			int32 RetrievedValue = 0;
//			bool bSuccess = ClientHandle.GetClient().GetInt32(TEXT("IntKey"), RetrievedValue);
//
//			TestTrue("GetInt32 should succeed", bSuccess);
//			TestTrue("Retrieved int32 value should match original", OriginalValue == RetrievedValue);
//		});
//
//		It("should store and retrieve an int64", [this]()
//		{
//			int64 OriginalValue = 123456789012345LL;
//			ClientHandle.GetClient().SetInt64(TEXT("Int64Key"), OriginalValue);
//
//			int64 RetrievedValue = 0;
//			bool bSuccess = ClientHandle.GetClient().GetInt64(TEXT("Int64Key"), RetrievedValue);
//
//			TestTrue("GetInt64 should succeed", bSuccess);
//			TestTrue("Retrieved int64 value should match original", OriginalValue == RetrievedValue);
//		});
//
//		It("should store and retrieve a float", [this]()
//		{
//			float OriginalValue = 3.14f;
//			ClientHandle.GetClient().SetFloat(TEXT("FloatKey"), OriginalValue);
//
//			float RetrievedValue = 0.0f;
//			bool bSuccess = ClientHandle.GetClient().GetFloat(TEXT("FloatKey"), RetrievedValue);
//
//			TestTrue("GetFloat should succeed", bSuccess);
//			TestTrue("Retrieved float value should match original", FMath::IsNearlyEqual(OriginalValue, RetrievedValue));
//		});
//
//		It("should store and retrieve a double", [this]()
//		{
//			double OriginalValue = 2.71828;
//			ClientHandle.GetClient().SetDouble(TEXT("DoubleKey"), OriginalValue);
//
//			double RetrievedValue = 0.0;
//			bool bSuccess = ClientHandle.GetClient().GetDouble(TEXT("DoubleKey"), RetrievedValue);
//
//			TestTrue("GetDouble should succeed", bSuccess);
//			TestTrue("Retrieved double value should match original", FMath::IsNearlyEqual(OriginalValue, RetrievedValue));
//		});
//
//		It("should store and retrieve an FString", [this]()
//		{
//			FString OriginalValue = TEXT("Test String");
//			ClientHandle.GetClient().SetString(TEXT("StringKey"), OriginalValue);
//
//			FString RetrievedValue;
//			bool bSuccess = ClientHandle.GetClient().GetString(TEXT("StringKey"), RetrievedValue);
//
//			TestTrue("GetString should succeed", bSuccess);
//			TestTrue("Retrieved FString value should match original", OriginalValue == RetrievedValue);
//		});
//
//		It("should store and retrieve an FDateTime", [this]()
//		{
//			FDateTime OriginalValue = FDateTime::UtcNow();
//			ClientHandle.GetClient().SetDateTime(TEXT("DateTimeKey"), OriginalValue);
//
//			FDateTime RetrievedValue;
//			bool bSuccess = ClientHandle.GetClient().GetDateTime(TEXT("DateTimeKey"), RetrievedValue);
//
//			TestTrue("GetDateTime should succeed", bSuccess);
//			TestTrue("Retrieved FDateTime value should match original", OriginalValue == RetrievedValue);
//		});
//
//		It("should remove an item", [this]()
//		{
//			int32 ValueToRemove = 100;
//			ClientHandle.GetClient().SetInt32(TEXT("RemoveKey"), ValueToRemove);
//			TestTrue("Item should exist before removal", ClientHandle.GetClient().Num(TEXT("RemoveKey")) == 1);
//
//			bool bRemoved = ClientHandle.GetClient().Remove(TEXT("RemoveKey"));
//			TestTrue("Remove should succeed", bRemoved);
//			TestTrue("Item should not exist after removal", ClientHandle.GetClient().Num(TEXT("RemoveKey")) == 0);
//		});
//
//		It("should clear all items for a client", [this]()
//		{
//			ClientHandle.GetClient().SetInt32(TEXT("Key1"), 1);
//			ClientHandle.GetClient().SetInt32(TEXT("Key2"), 2);
//			TestTrue("Client should have items before clear", ClientHandle.GetClient().Num() == 2);
//
//			ClientHandle.GetClient().Clear();
//			TestTrue("Client should have no items after clear", ClientHandle.GetClient().Num() == 0);
//		});
//
//		It("should handle TTL for items", [this]()
//		{
//			int32 ValueWithTTL = 50;
//			ClientHandle.GetClient().SetInt32WithTTL(TEXT("TTLKey"), ValueWithTTL, FTimespan::FromSeconds(0.1)); // 0.1 second TTL
//
//			int32 RetrievedValue = 0;
//			bool bSuccess = ClientHandle.GetClient().GetInt32(TEXT("TTLKey"), RetrievedValue);
//			TestTrue("Get should succeed immediately after set", bSuccess);
//
//			FPlatformProcess::Sleep(0.2f); // Wait for TTL to expire
//
//			bSuccess = ClientHandle.GetClient().GetInt32(TEXT("TTLKey"), RetrievedValue);
//			TestFalse("Get should fail after TTL expires", bSuccess);
//		});
//	});
//}
//
//#endif //WITH_DEV_AUTOMATION_TESTS
