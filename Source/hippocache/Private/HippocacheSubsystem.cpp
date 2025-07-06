#include "HippocacheSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "HippocacheVariantWrapper.h"

// Macros for read-write lock patterns
#define HIPPOCACHE_READ_LOCK() FReadScopeLock ReadLock(CacheRWLock)
#define HIPPOCACHE_WRITE_LOCK() FWriteScopeLock WriteLock(CacheRWLock)
// Legacy macro for compatibility (uses write lock for safety)
#define HIPPOCACHE_SCOPED_LOCK() FWriteScopeLock WriteLock(CacheRWLock)


// FHippocacheClientHandle implementation
/*
FHippocacheResult FHippocacheClientHandle::GetClientSafe(FHippocacheClient*& OutClient)
{
	if (!ClientPtr.IsValid())
	{
		OutClient = nullptr;
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidClient, 
			TEXT("Client handle is invalid"), 
			FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}

	OutClient = ClientPtr.Get();
	return FHippocacheResult::Success();
}
*/

// UHippocacheSubsystem implementation
FHippocacheResult UHippocacheSubsystem::Get(const UObject* WorldContextObject, UHippocacheSubsystem*& OutSubsystem)
{
	OutSubsystem = nullptr;

	if (!WorldContextObject)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidValue, 
			TEXT("WorldContextObject is null"), 
			TEXT("Cannot get world context from null object"));
	}

	if (!GEngine)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::UnknownError, 
			TEXT("GEngine is null"), 
			TEXT("Engine not initialized"));
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::WorldNotFound, 
			TEXT("Failed to get world from context object"), 
			FString::Printf(TEXT("WorldContextObject: %s"), *WorldContextObject->GetClass()->GetName()));
	}

	// Get GameInstance from World
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidSubsystem, 
			TEXT("No GameInstance found in world"), 
			FString::Printf(TEXT("World: %s"), *World->GetName()));
	}

	// Get HippocacheSubsystem from GameInstance
	OutSubsystem = GameInstance->GetSubsystem<UHippocacheSubsystem>();
	if (!OutSubsystem)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidSubsystem, 
			TEXT("Failed to get HippocacheSubsystem from GameInstance"), 
			FString::Printf(TEXT("GameInstance: %s"), *GameInstance->GetName()));
	}

	return FHippocacheResult::Success();
}


void UHippocacheSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Start periodic cleanup timer with error handling
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(CleanupTimerHandle, this, &UHippocacheSubsystem::PerformCleanup, 60.0f, true);
		
		UE_LOG(LogTemp, Log, TEXT("HippocacheSubsystem: Cleanup timer started (60 second interval)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HippocacheSubsystem: Failed to start cleanup timer - World is null"));
	}
}

void UHippocacheSubsystem::Deinitialize()
{
	// Clear cleanup timer with error handling
	UWorld* World = GetWorld();
	if (World && CleanupTimerHandle.IsValid())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(CleanupTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("HippocacheSubsystem: Cleanup timer cleared"));
	}

	// Clear all data
	// const int32 ClientCount = ActiveClients.Num();
	const int32 DataCount = AllClientData.Num();
	
	// ActiveClients.Empty();
	AllClientData.Empty();

	UE_LOG(LogTemp, Log, TEXT("HippocacheSubsystem: Cleared %d data collections"), DataCount);

	Super::Deinitialize();
}

/*
// Disabled client functions - keeping stub for compilation
FHippocacheResult UHippocacheSubsystem::CreateOrGetClient(FName Collection, FHippocacheClientHandle& OutClientHandle)
{
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, 
			TEXT("Collection name cannot be None"), 
			TEXT("Valid collection name required"));
	}

	HIPPOCACHE_SCOPED_LOCK();

	TSharedPtr<FHippocacheClient>* FoundClient = ActiveClients.Find(Collection);
	if (FoundClient && FoundClient->IsValid())
	{
		OutClientHandle.ClientPtr = *FoundClient;
		OutClientHandle.Collection = Collection;
		return FHippocacheResult::Success();
	}

	// Create new client
	TSharedPtr<FHippocacheClient> NewClient = MakeShared<FHippocacheClient>(this, Collection);
	if (!NewClient.IsValid())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::MemoryAllocationError, 
			TEXT("Failed to create new client"), 
			FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}

	ActiveClients.Add(Collection, NewClient);
	AllClientData.FindOrAdd(Collection);

	OutClientHandle.ClientPtr = NewClient;
	OutClientHandle.Collection = Collection;

	UE_LOG(LogTemp, Log, TEXT("HippocacheSubsystem: Created new client for collection '%s'"), *Collection.ToString());
	return FHippocacheResult::Success();
}

FHippocacheResult UHippocacheSubsystem::GetClient(FName Collection, FHippocacheClientHandle& OutClientHandle) const
{
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, 
			TEXT("Collection name cannot be None"), 
			TEXT("Valid collection name required"));
	}

	HIPPOCACHE_SCOPED_LOCK();
	
	const TSharedPtr<FHippocacheClient>* FoundClient = ActiveClients.Find(Collection);
	if (!FoundClient || !FoundClient->IsValid())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, 
			TEXT("Client not found"), 
			FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}

	OutClientHandle.ClientPtr = *FoundClient;
	OutClientHandle.Collection = Collection;
	return FHippocacheResult::Success();
}

FHippocacheResult UHippocacheSubsystem::RemoveClient(FName Collection)
{
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, 
			TEXT("Collection name cannot be None"), 
			TEXT("Valid collection name required"));
	}

	HIPPOCACHE_SCOPED_LOCK();
	
	const bool bClientRemoved = ActiveClients.Remove(Collection) > 0;
	const bool bDataRemoved = AllClientData.Remove(Collection) > 0;

	if (!bClientRemoved && !bDataRemoved)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, 
			TEXT("Client not found"), 
			FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}

	UE_LOG(LogTemp, Log, TEXT("HippocacheSubsystem: Removed client for collection '%s'"), *Collection.ToString());
	return FHippocacheResult::Success();
}

FHippocacheResult UHippocacheSubsystem::ClearAllClients()
{
	HIPPOCACHE_SCOPED_LOCK();
	
	const int32 ClientCount = ActiveClients.Num();
	const int32 DataCount = AllClientData.Num();

	ActiveClients.Empty();
	AllClientData.Empty();

	UE_LOG(LogTemp, Log, TEXT("HippocacheSubsystem: Cleared all clients (%d clients, %d data collections)"), ClientCount, DataCount);
	return FHippocacheResult::Success();
}
*/


FHippocacheResult UHippocacheSubsystem::Remove(FName Collection, const FString& Key)
{
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, TEXT("Collection name cannot be None"), TEXT("Valid collection name required"));
	}
	if (Key.IsEmpty())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidKey, TEXT("Key cannot be empty"), FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}
	
	HIPPOCACHE_SCOPED_LOCK();
	
	TMap<FString, FCachedItem>* ClientData = AllClientData.Find(Collection);
	if (!ClientData)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, TEXT("Collection not found"), FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}
	if (ClientData->Remove(Key) == 0)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, TEXT("Item not found"), FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
	}
	return FHippocacheResult::Success();
}

FHippocacheResult UHippocacheSubsystem::Clear(FName Collection)
{
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, TEXT("Collection name cannot be None"), TEXT("Valid collection name required"));
	}
	
	HIPPOCACHE_SCOPED_LOCK();
	
	TMap<FString, FCachedItem>* ClientData = AllClientData.Find(Collection);
	if (!ClientData)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, TEXT("Collection not found"), FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}
	const int32 ClearedCount = ClientData->Num();
	ClientData->Empty();
	UE_LOG(LogTemp, Log, TEXT("HippocacheSubsystem: Cleared %d items from collection '%s'"), ClearedCount, *Collection.ToString());
	return FHippocacheResult::Success();
}

FHippocacheResult UHippocacheSubsystem::Num(FName Collection, int32& OutCount) const
{
	OutCount = 0;
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, TEXT("Collection name cannot be None"), TEXT("Valid collection name required"));
	}
	
	HIPPOCACHE_READ_LOCK();
	
	const TMap<FString, FCachedItem>* ClientData = AllClientData.Find(Collection);
	if (!ClientData)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, TEXT("Collection not found"), FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}
	OutCount = ClientData->Num();
	return FHippocacheResult::Success();
}

// FInstancedStruct methods
FHippocacheResult UHippocacheSubsystem::SetStructWithTTL(FName Collection, const FString& Key, const FInstancedStruct& Value, FTimespan TTL)
{
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, TEXT("Collection name cannot be None"), TEXT("Valid collection name required"));
	}
	if (Key.IsEmpty())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidKey, TEXT("Key cannot be empty"), FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}
	if (!Value.IsValid())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidValue, TEXT("Struct value is invalid"), FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
	}
	
	HIPPOCACHE_SCOPED_LOCK();
	
	TMap<FString, FCachedItem>& ClientData = GetClientData(Collection);
	ClientData.Add(Key, FCachedItem(Value, TTL));
	return FHippocacheResult::Success();
}

FHippocacheResult UHippocacheSubsystem::SetStruct(FName Collection, const FString& Key, const FInstancedStruct& Value)
{
	return SetStructWithTTL(Collection, Key, Value, FTimespan::Zero());
}

FHippocacheResult UHippocacheSubsystem::GetStruct(FName Collection, const FString& Key, FInstancedStruct& OutValue)
{
	if (Collection.IsNone())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidCollection, TEXT("Collection name cannot be None"), TEXT("Valid collection name required"));
	}
	if (Key.IsEmpty())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidKey, TEXT("Key cannot be empty"), FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}
	
	HIPPOCACHE_READ_LOCK();
	
	const TMap<FString, FCachedItem>* ClientDataPtr = AllClientData.Find(Collection);
	if (!ClientDataPtr)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, TEXT("Collection not found"), FString::Printf(TEXT("Collection: %s"), *Collection.ToString()));
	}
	const TMap<FString, FCachedItem>& ClientData = *ClientDataPtr;
	const FCachedItem* FoundItem = ClientData.Find(Key);
	if (!FoundItem)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemNotFound, TEXT("Item not found"), FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
	}
	
	// Check expiration but don't remove - let cleanup process handle expired items
	if (FoundItem->HasExpired())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::ItemExpired, TEXT("Item has expired"), FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
	}
	
	if (!FoundItem->Value.IsValid())
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, TEXT("Type mismatch - expected Struct"), FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
	}
	
	OutValue = FoundItem->Value;
	
	// Update access time for LRU tracking (future use) - disabled for performance
	// FoundItem->UpdateAccessTime();
	
	return FHippocacheResult::Success();
}

void UHippocacheSubsystem::PerformCleanup()
{
	HIPPOCACHE_SCOPED_LOCK();
	
	// Clean up expired items in all collections
	for (auto& CollectionPair : AllClientData)
	{
		TMap<FString, FCachedItem>& ClientData = CollectionPair.Value;
		for (auto ItemIt = ClientData.CreateIterator(); ItemIt; ++ItemIt)
		{
			if (ItemIt->Value.HasExpired())
			{
				ItemIt.RemoveCurrent();
			}
		}
	}
	
	/*
	// Original implementation using ActiveClients - disabled
	for (auto It = ActiveClients.CreateIterator(); It; ++It)
	{
		FName Collection = It->Key;
		TSharedPtr<FHippocacheClient> ClientPtr = It->Value;

		// If the shared pointer is no longer valid, it means all external references are gone.
		// We can safely remove this client's data.
		if (!ClientPtr.IsValid())
		{
			AllClientData.Remove(Collection);
			It.RemoveCurrent();
		}
		else
		{
			// Clean up expired items within this client's data
			TMap<FString, FCachedItem>& ClientData = AllClientData.FindOrAdd(Collection);
			for (auto ItemIt = ClientData.CreateIterator(); ItemIt; ++ItemIt)
			{
				if (ItemIt->Value.HasExpired())
				{
					ItemIt.RemoveCurrent();
				}
			}
		}
	}
	*/
}

TMap<FString, FCachedItem>& UHippocacheSubsystem::GetClientData(FName Collection)
{
	return AllClientData.FindOrAdd(Collection);
}

const TMap<FString, FCachedItem>& UHippocacheSubsystem::GetClientData(FName Collection) const
{
	const TMap<FString, FCachedItem>* FoundData = AllClientData.Find(Collection);
	if (FoundData)
	{
		return *FoundData;
	}
	// Return an empty static map if not found (for const correctness)
	static const TMap<FString, FCachedItem> EmptyMap;
	return EmptyMap;
}

