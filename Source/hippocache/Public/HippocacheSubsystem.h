#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HAL/CriticalSection.h"
#include "StructUtils/InstancedStruct.h"
#include "HippocacheResult.h"
#include "HippocacheVariantWrapper.h"
#include "HippocacheSubsystem.generated.h"

/**
 * @brief Represents a single cached item using unified FInstancedStruct storage.
 * All data types (primitives and structs) are stored as FInstancedStruct for consistency.
 */
USTRUCT()
struct FCachedItem
{
	GENERATED_BODY()

	/** The cached data stored as FInstancedStruct (unified storage for all types). */
	UPROPERTY()
	FInstancedStruct Value;

	/** Time to live in seconds. 0.0f means no expiration. */
	UPROPERTY()
	FTimespan TTL;

	/** Timestamp when this item was cached. */
	UPROPERTY()
	double CreationTime;

	// TODO: Memory features - disabled for now, implement later
	/** Last access time for LRU eviction */
	mutable double LastAccessTime;

	/** Estimated memory size in bytes */
	// int64 EstimatedSizeBytes;

	/** Default constructor. */
	FCachedItem()
		: TTL(FTimespan::Zero())
		, CreationTime(0.0)
		, LastAccessTime(0.0)
		// , EstimatedSizeBytes(0)
	{}

	/** Constructor for any type stored as FInstancedStruct. */
	FCachedItem(const FInstancedStruct& InValue, FTimespan InTTL)
		: Value(InValue)
		, TTL(InTTL)
		, CreationTime(FPlatformTime::Seconds())
		, LastAccessTime(FPlatformTime::Seconds())
	{}

	/** Checks if the item has expired. */
	bool HasExpired() const
	{
		return TTL > FTimespan::Zero() && (FPlatformTime::Seconds() - CreationTime) > TTL.GetTotalSeconds();
	}

	/** Updates last access time */
	void UpdateAccessTime() const
	{
		LastAccessTime = FPlatformTime::Seconds();
	}

	/** Get time since last access */
	double GetTimeSinceLastAccess() const
	{
		return FPlatformTime::Seconds() - LastAccessTime;
	}
};

// TODO: Memory limit features - disabled for now, implement later

/**
 * @brief Memory limit configuration for Hippocache
 */
/*
* 
USTRUCT(BlueprintType)
struct HIPPOCACHE_API FHippocacheMemoryConfig
{
	GENERATED_BODY()

	// Maximum memory usage in megabytes (0 = unlimited)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hippocache", meta = (ClampMin = "0"))
	int32 MaxMemoryUsageMB = 100;

	// Maximum items per collection (0 = unlimited)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hippocache", meta = (ClampMin = "0"))
	int32 MaxItemsPerCollection = 10000;

	// Maximum total items across all collections (0 = unlimited) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hippocache", meta = (ClampMin = "0"))
	int32 MaxTotalItems = 50000;

	// Enable automatic eviction when limits are reached 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hippocache")
	bool bEnableAutoEviction = true;

	FHippocacheMemoryConfig()
	{
		MaxMemoryUsageMB = 100;
		MaxItemsPerCollection = 10000;
		MaxTotalItems = 50000;
		bEnableAutoEviction = true;
	}
};
*/

/**
 * @brief Memory usage statistics for Hippocache
 */
/*
USTRUCT(BlueprintType)
struct HIPPOCACHE_API FHippocacheMemoryStats
{
	GENERATED_BODY()

	// Current estimated memory usage in bytes
	UPROPERTY(BlueprintReadOnly, Category = "Hippocache")
	int64 CurrentMemoryBytes = 0;

	// Current total number of items
	UPROPERTY(BlueprintReadOnly, Category = "Hippocache")
	int32 TotalItems = 0;

	// Number of collections 
	UPROPERTY(BlueprintReadOnly, Category = "Hippocache")
	int32 CollectionCount = 0;

	// Number of evictions performed 
	UPROPERTY(BlueprintReadOnly, Category = "Hippocache")
	int32 EvictionCount = 0;
};
*/

// Client-side cache interface has been disabled for now as it's not being used
// All functionality is available through UHippocacheBlueprintLibrary instead

/**
 * @brief Manages and provides access to named FHippocacheClient instances.
 * This is the central point for creating and retrieving cache client in Blueprints and C++.
 */
UCLASS()
class HIPPOCACHE_API UHippocacheSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * @brief Gets the Hippocache Subsystem with error handling.
	 * @param WorldContextObject The object to get the world context from.
	 * @param OutSubsystem The retrieved subsystem instance.
	 * @return Result indicating success or failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext = "true"))
	static FHippocacheResult Get(const UObject* WorldContextObject, UHippocacheSubsystem*& OutSubsystem);

	// USubsystem implementation
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Client-side functions disabled for now
	// All functionality is available through UHippocacheBlueprintLibrary instead

	// Data access methods

	/**
	 * @brief Stores a USTRUCT value in the cache using FInstancedStruct.
	 * @param Collection The name of the client.
	 * @param Key The key to associate with the value.
	 * @param Value The struct value to store (any USTRUCT).
	 * @param TTL Time to live. FTimespan::Zero() means no expiration.
	 * @return Result indicating success or failure.
	 */
	FHippocacheResult SetStructWithTTL(FName Collection, const FString& Key, const FInstancedStruct& Value, FTimespan TTL);
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Client")
	FHippocacheResult SetStruct(FName Collection, const FString& Key, const FInstancedStruct& Value);
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Client")
	FHippocacheResult GetStruct(FName Collection, const FString& Key, FInstancedStruct& OutValue);

	/**
	 * @brief Template convenience functions for direct struct access (C++ only).
	 */
	template<typename T>
	FHippocacheResult SetStructWithTTL(FName Collection, const FString& Key, const T& Value, FTimespan TTL)
	{
		// No static_assert needed - FInstancedStruct::Make will handle the type checking
		FInstancedStruct InstancedStruct = FInstancedStruct::Make<T>(Value);
		return SetStructWithTTL(Collection, Key, InstancedStruct, TTL);
	}

	template<typename T>
	FHippocacheResult SetStruct(FName Collection, const FString& Key, const T& Value)
	{
		return SetStructWithTTL<T>(Collection, Key, Value, FTimespan::Zero());
	}

	template<typename T>
	THippocacheResult<T> GetStructTyped(FName Collection, const FString& Key)
	{
		// No static_assert needed - T::StaticStruct() will fail at compile time if T is not a USTRUCT
		FInstancedStruct OutValue;
		auto Result = GetStruct(Collection, Key, OutValue);
		
		if (Result.IsError())
		{
			return THippocacheResult<T>::Error(Result.ErrorCode, Result.ErrorMessage, Result.ErrorContext);
		}

		if (OutValue.IsValid() && OutValue.GetScriptStruct() == T::StaticStruct())
		{
			return THippocacheResult<T>::Success(*OutValue.GetPtr<T>());
		}

		return THippocacheResult<T>::Error(EHippocacheErrorCode::TypeMismatch, 
			TEXT("Struct type mismatch"), 
			FString::Printf(TEXT("Expected %s"), *T::StaticStruct()->GetName()));
	}

	/**
	 * @brief Removes an item from the cache for a specific client.
	 * @param Collection The name of the client.
	 * @param Key The key of the item to remove.
	 * @return Result indicating success or failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Client")
	FHippocacheResult Remove(FName Collection, const FString& Key);

	/**
	 * @brief Clears all items from the cache for a specific client.
	 * @param Collection The name of the client.
	 * @return Result indicating success or failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Client")
	FHippocacheResult Clear(FName Collection);

	/**
	 * @brief Gets the number of items currently in the cache for a specific client.
	 * @param Collection The name of the client.
	 * @param OutCount The number of items in the cache.
	 * @return Result indicating success or failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Client")
	FHippocacheResult Num(FName Collection, int32& OutCount) const;

	// TODO: Memory management functions - disabled for now, implement later
	//
	// Sets memory configuration for the cache.
	// @param Config The memory configuration to apply.
	// @return Result indicating success or failure.
	//
	// UFUNCTION(BlueprintCallable, Category = "Hippocache|Memory")
	// FHippocacheResult SetMemoryConfig(const FHippocacheMemoryConfig& Config);

	// Gets current memory configuration.
	// @return Current memory configuration.
	//
	// UFUNCTION(BlueprintCallable, Category = "Hippocache|Memory")
	// FHippocacheMemoryConfig GetMemoryConfig() const;

	// Gets current memory usage statistics.
	// @return Current memory statistics.
	//
	// UFUNCTION(BlueprintCallable, Category = "Hippocache|Memory")
	// FHippocacheMemoryStats GetMemoryStats() const;

private:
	/** Map of active named Hippocache client instances. */
	// TMap<FName, TSharedPtr<FHippocacheClient>> ActiveClients;

	/** Central storage for all cached data, organized by client name. */
	TMap<FName, TMap<FString, FCachedItem>> AllClientData;

	/** Timer handle for periodic cleanup of expired items. */
	FTimerHandle CleanupTimerHandle;

	/** Read-write lock for thread safety - allows concurrent reads */
	mutable FRWLock CacheRWLock;

	// TODO: Memory features - disabled for now, implement later
	//
	// Memory configuration
	// FHippocacheMemoryConfig MemoryConfig;

	// Current memory statistics  
	// mutable FHippocacheMemoryStats MemoryStats;

	/** Periodically cleans up expired items from all active clients. */
	void PerformCleanup();

	/** Helper to get a client's data map, creating it if it doesn't exist. */
	TMap<FString, FCachedItem>& GetClientData(FName Collection);


	/** Helper to get a client's data map (const version). */
	const TMap<FString, FCachedItem>& GetClientData(FName Collection) const;

	// TODO: Memory management methods - disabled for now, implement later
	//
	// Check if adding an item would exceed memory limits
	// FHippocacheResult CheckMemoryLimits(FName Collection, int64 NewItemSize) const;

	// Evict least recently used items to make space
	// FHippocacheResult EvictLRU(int64 RequiredSpace);

	// Update memory statistics
	// void UpdateMemoryStats();

	// Estimate total memory usage
	// int64 CalculateTotalMemoryUsage() const;

	// Get total item count
	// int32 GetTotalItemCount() const;
};

// ============================================================================
// FVariant-based type support - all EVariantTypes automatically supported
// ============================================================================