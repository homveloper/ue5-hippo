#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HippocacheSubsystem.h"
#include "HippocacheResult.h"
#include "HippocacheVariantWrapper.h"
#include "HippocacheBlueprintLibrary.generated.h"

/**
 * @brief Blueprint Function Library for Hippocache.
 * Provides simple static functions to interact with the Hippocache Subsystem using Collection names.
 */
UCLASS()
class HIPPOCACHE_API UHippocacheBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Helper function to get subsystem with error handling
	static FHippocacheResult GetSubsystemSafe(const UObject* WorldContextObject, UHippocacheSubsystem*& OutSubsystem);


	// C++ only template functions for direct struct access
	template<typename T>
	static FHippocacheResult SetStructWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value, float TTLSeconds)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		FTimespan TTL = FTimespan::FromSeconds(TTLSeconds);
		return Subsystem->SetStructWithTTL<T>(Collection, Key, Value, TTL);
	}

	// Template specialization for FInstancedStruct to avoid recursive wrapping
	template<>
	FHippocacheResult SetStructWithTTL<FInstancedStruct>(const UObject* WorldContextObject, FName Collection, const FString& Key, const FInstancedStruct& Value, float TTLSeconds)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		FTimespan TTL = FTimespan::FromSeconds(TTLSeconds);
		return Subsystem->SetStructWithTTL(Collection, Key, Value, TTL);
	}

	template<typename T>
	static FHippocacheResult SetStruct(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		return Subsystem->SetStruct<T>(Collection, Key, Value);
	}

	template<typename T>
	static THippocacheResult<T> GetStruct(const UObject* WorldContextObject, FName Collection, const FString& Key)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return THippocacheResult<T>::Error(Result.ErrorCode, Result.ErrorMessage, Result.ErrorContext);
		}

		return Subsystem->GetStructTyped<T>(Collection, Key);
	}

	// Template specialization for FInstancedStruct to avoid static_assert
	template<>
	THippocacheResult<FInstancedStruct> GetStruct<FInstancedStruct>(const UObject* WorldContextObject, FName Collection, const FString& Key)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return THippocacheResult<FInstancedStruct>::Error(Result.ErrorCode, Result.ErrorMessage, Result.ErrorContext);
		}

		FInstancedStruct OutValue;
		Result = Subsystem->GetStruct(Collection, Key, OutValue);
		if (Result.IsError())
		{
			return THippocacheResult<FInstancedStruct>::Error(Result.ErrorCode, Result.ErrorMessage, Result.ErrorContext);
		}

		return THippocacheResult<FInstancedStruct>::Success(OutValue);
	}

	// C++ only template functions for primitive type access via FVariant
	template<typename T>
	static FHippocacheResult SetPrimitiveWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value, float TTLSeconds)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		// Wrap primitive value in FVariantWrapper and store as FInstancedStruct
		FVariantWrapper Wrapper((FVariant(Value)));
		FInstancedStruct InstancedStruct = FInstancedStruct::Make<FVariantWrapper>(Wrapper);
		
		FTimespan TTL = FTimespan::FromSeconds(TTLSeconds);
		return Subsystem->SetStructWithTTL(Collection, Key, InstancedStruct, TTL);
	}

	template<typename T>
	static FHippocacheResult SetPrimitive(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		// Wrap primitive value in FVariantWrapper and store as FInstancedStruct
		FVariantWrapper Wrapper((FVariant(Value)));
		FInstancedStruct InstancedStruct = FInstancedStruct::Make<FVariantWrapper>(Wrapper);
		
		return Subsystem->SetStruct(Collection, Key, InstancedStruct);
	}

	template<typename T>
	static FHippocacheResult GetPrimitive(const UObject* WorldContextObject, FName Collection, const FString& Key, T& OutValue)
	{
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		FInstancedStruct OutStruct;
		Result = Subsystem->GetStruct(Collection, Key, OutStruct);
		if (Result.IsError())
		{
			return Result;
		}

		// Check if it's a FVariantWrapper
		if (!OutStruct.IsValid() || OutStruct.GetScriptStruct() != FVariantWrapper::StaticStruct())
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, 
				TEXT("Expected FVariantWrapper"), 
				FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
		}

		const FVariantWrapper* Wrapper = OutStruct.GetPtr<FVariantWrapper>();
		if (!Wrapper)
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, 
				TEXT("Failed to extract FVariantWrapper"), 
				FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
		}

		// Check if the FVariant holds the expected type
		if (!Wrapper->IsType<T>())
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, 
				FString::Printf(TEXT("FVariant type mismatch - got %s"), 
					*Wrapper->GetTypeName()), 
				FString::Printf(TEXT("Collection: %s, Key: %s"), *Collection.ToString(), *Key));
		}

		OutValue = Wrapper->GetValue<T>();
		return FHippocacheResult::Success();
	}

	// Int32 operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Int32 With TTL"))
	static FHippocacheResult SetInt32WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, int32 Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Int32"))
	static FHippocacheResult SetInt32(const UObject* WorldContextObject, FName Collection, const FString& Key, int32 Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Int32"))
	static FHippocacheResult GetInt32(const UObject* WorldContextObject, FName Collection, const FString& Key, int32& OutValue);

	// Int64 operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Int64 With TTL"))
	static FHippocacheResult SetInt64WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, int64 Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Int64"))
	static FHippocacheResult SetInt64(const UObject* WorldContextObject, FName Collection, const FString& Key, int64 Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Int64"))
	static FHippocacheResult GetInt64(const UObject* WorldContextObject, FName Collection, const FString& Key, int64& OutValue);

	// Float operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Float With TTL"))
	static FHippocacheResult SetFloatWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, float Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Float"))
	static FHippocacheResult SetFloat(const UObject* WorldContextObject, FName Collection, const FString& Key, float Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Float"))
	static FHippocacheResult GetFloat(const UObject* WorldContextObject, FName Collection, const FString& Key, float& OutValue);

	// Double operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Double With TTL"))
	static FHippocacheResult SetDoubleWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, double Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Double"))
	static FHippocacheResult SetDouble(const UObject* WorldContextObject, FName Collection, const FString& Key, double Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Double"))
	static FHippocacheResult GetDouble(const UObject* WorldContextObject, FName Collection, const FString& Key, double& OutValue);

	// String operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set String With TTL"))
	static FHippocacheResult SetStringWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FString& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set String"))
	static FHippocacheResult SetString(const UObject* WorldContextObject, FName Collection, const FString& Key, const FString& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get String"))
	static FHippocacheResult GetString(const UObject* WorldContextObject, FName Collection, const FString& Key, FString& OutValue);

	// DateTime operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set DateTime With TTL"))
	static FHippocacheResult SetDateTimeWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FDateTime& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set DateTime"))
	static FHippocacheResult SetDateTime(const UObject* WorldContextObject, FName Collection, const FString& Key, const FDateTime& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get DateTime"))
	static FHippocacheResult GetDateTime(const UObject* WorldContextObject, FName Collection, const FString& Key, FDateTime& OutValue);

	// Bool operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Bool With TTL"))
	static FHippocacheResult SetBoolWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, bool Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Bool"))
	static FHippocacheResult SetBool(const UObject* WorldContextObject, FName Collection, const FString& Key, bool Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Bool"))
	static FHippocacheResult GetBool(const UObject* WorldContextObject, FName Collection, const FString& Key, bool& OutValue);

	// Int8 operations (C++ only - not Blueprint compatible)
	static FHippocacheResult SetInt8WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, int8 Value, float TTLSeconds);
	static FHippocacheResult SetInt8(const UObject* WorldContextObject, FName Collection, const FString& Key, int8 Value);
	static FHippocacheResult GetInt8(const UObject* WorldContextObject, FName Collection, const FString& Key, int8& OutValue);

	// UInt8 operations (Byte)
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Byte With TTL"))
	static FHippocacheResult SetByteWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, uint8 Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Byte"))
	static FHippocacheResult SetByte(const UObject* WorldContextObject, FName Collection, const FString& Key, uint8 Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Byte"))
	static FHippocacheResult GetByte(const UObject* WorldContextObject, FName Collection, const FString& Key, uint8& OutValue);

	// Name operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Name With TTL"))
	static FHippocacheResult SetNameWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, FName Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Name"))
	static FHippocacheResult SetName(const UObject* WorldContextObject, FName Collection, const FString& Key, FName Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Name"))
	static FHippocacheResult GetName(const UObject* WorldContextObject, FName Collection, const FString& Key, FName& OutValue);

	// Vector operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Vector With TTL"))
	static FHippocacheResult SetVectorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Vector"))
	static FHippocacheResult SetVector(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Vector"))
	static FHippocacheResult GetVector(const UObject* WorldContextObject, FName Collection, const FString& Key, FVector& OutValue);

	// Vector2d operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Vector2D With TTL"))
	static FHippocacheResult SetVector2DWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector2D& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Vector2D"))
	static FHippocacheResult SetVector2D(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector2D& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Vector2D"))
	static FHippocacheResult GetVector2D(const UObject* WorldContextObject, FName Collection, const FString& Key, FVector2D& OutValue);

	// Vector4 operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Vector4 With TTL"))
	static FHippocacheResult SetVector4WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector4& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Vector4"))
	static FHippocacheResult SetVector4(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector4& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Vector4"))
	static FHippocacheResult GetVector4(const UObject* WorldContextObject, FName Collection, const FString& Key, FVector4& OutValue);

	// Rotator operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Rotator With TTL"))
	static FHippocacheResult SetRotatorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FRotator& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Rotator"))
	static FHippocacheResult SetRotator(const UObject* WorldContextObject, FName Collection, const FString& Key, const FRotator& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Rotator"))
	static FHippocacheResult GetRotator(const UObject* WorldContextObject, FName Collection, const FString& Key, FRotator& OutValue);

	// Transform operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Transform With TTL"))
	static FHippocacheResult SetTransformWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTransform& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Transform"))
	static FHippocacheResult SetTransform(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTransform& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Transform"))
	static FHippocacheResult GetTransform(const UObject* WorldContextObject, FName Collection, const FString& Key, FTransform& OutValue);

	// Color operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Color With TTL"))
	static FHippocacheResult SetColorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FColor& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Color"))
	static FHippocacheResult SetColor(const UObject* WorldContextObject, FName Collection, const FString& Key, const FColor& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Color"))
	static FHippocacheResult GetColor(const UObject* WorldContextObject, FName Collection, const FString& Key, FColor& OutValue);

	// LinearColor operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set LinearColor With TTL"))
	static FHippocacheResult SetLinearColorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FLinearColor& Value, float TTLSeconds);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Set LinearColor"))
	static FHippocacheResult SetLinearColor(const UObject* WorldContextObject, FName Collection, const FString& Key, const FLinearColor& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get LinearColor"))
	static FHippocacheResult GetLinearColor(const UObject* WorldContextObject, FName Collection, const FString& Key, FLinearColor& OutValue);

	// Quat operations (C++ only - not Blueprint compatible)
	static FHippocacheResult SetQuatWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FQuat& Value, float TTLSeconds);
	static FHippocacheResult SetQuat(const UObject* WorldContextObject, FName Collection, const FString& Key, const FQuat& Value);
	static FHippocacheResult GetQuat(const UObject* WorldContextObject, FName Collection, const FString& Key, FQuat& OutValue);

	// Timespan operations (C++ only - not Blueprint compatible)
	static FHippocacheResult SetTimespanWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTimespan& Value, float TTLSeconds);
	static FHippocacheResult SetTimespan(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTimespan& Value);
	static FHippocacheResult GetTimespan(const UObject* WorldContextObject, FName Collection, const FString& Key, FTimespan& OutValue);

	// Guid operations (C++ only - not Blueprint compatible)
	static FHippocacheResult SetGuidWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FGuid& Value, float TTLSeconds);
	static FHippocacheResult SetGuid(const UObject* WorldContextObject, FName Collection, const FString& Key, const FGuid& Value);
	static FHippocacheResult GetGuid(const UObject* WorldContextObject, FName Collection, const FString& Key, FGuid& OutValue);

	// ============================================================================
	// Universal Setter/Getter - Hippoo & Hippop (C++ Templates)
	// ============================================================================
	
	/**
	 * Universal setter function - automatically detects type and stores any supported value (primitives and structs)
	 * @param WorldContextObject - World context for subsystem access
	 * @param Collection - Collection name to store in
	 * @param Key - Key to store the value under
	 * @param Value - Value to store (type automatically detected at runtime)
	 * @return Result indicating success or failure
	 */
	template<typename T>
	static FHippocacheResult Hippoo(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value)
	{
		return HippooImpl<T>(WorldContextObject, Collection, Key, Value);
	}

	/**
	 * Universal setter function with TTL - automatically detects type and stores any supported value with expiration
	 * @param WorldContextObject - World context for subsystem access
	 * @param Collection - Collection name to store in
	 * @param Key - Key to store the value under
	 * @param Value - Value to store (type automatically detected at runtime)
	 * @param TTLSeconds - Time to live in seconds
	 * @return Result indicating success or failure
	 */
	template<typename T>
	static FHippocacheResult Hippoo(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value, float TTLSeconds)
	{
		return HippooImplWithTTL<T>(WorldContextObject, Collection, Key, Value, TTLSeconds);
	}

	/**
	 * Universal getter function - automatically detects type and retrieves value (primitives and structs)
	 * @param WorldContextObject - World context for subsystem access
	 * @param Collection - Collection name to retrieve from
	 * @param Key - Key to retrieve the value from
	 * @param OutValue - Reference to store the retrieved value (type automatically detected at runtime)
	 * @return Result indicating success or failure
	 */
	template<typename T>
	static FHippocacheResult Hippop(const UObject* WorldContextObject, FName Collection, const FString& Key, T& OutValue)
	{
		return HippopImpl<T>(WorldContextObject, Collection, Key, OutValue);
	}

	// Cache management operations
	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Remove Item"))
	static FHippocacheResult Remove(const UObject* WorldContextObject, FName Collection, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Clear Collection"))
	static FHippocacheResult Clear(const UObject* WorldContextObject, FName Collection);

	UFUNCTION(BlueprintCallable, Category = "Hippocache", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Collection Size"))
	static FHippocacheResult Num(const UObject* WorldContextObject, FName Collection, int32& OutCount);

	// ============================================================================
	// Result Helper Functions for Blueprint
	// ============================================================================
	
	/**
	 * Checks if the cache operation was successful
	 * @param Result - The result to check
	 * @return True if the operation succeeded
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Is Success"))
	static bool IsSuccess(const FHippocacheResult& Result);

	/**
	 * Checks if the cache operation failed
	 * @param Result - The result to check
	 * @return True if the operation failed
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Is Error"))
	static bool IsError(const FHippocacheResult& Result);

	/**
	 * Checks if the item was not found (cache miss)
	 * @param Result - The result to check
	 * @return True if the item was not found
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Is Not Found"))
	static bool IsNotFound(const FHippocacheResult& Result);

	/**
	 * Checks if the item was expired
	 * @param Result - The result to check
	 * @return True if the item was expired
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Is Expired"))
	static bool IsExpired(const FHippocacheResult& Result);

	/**
	 * Checks if there was a type mismatch error
	 * @param Result - The result to check
	 * @return True if there was a type mismatch
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Is Type Mismatch"))
	static bool IsTypeMismatch(const FHippocacheResult& Result);

	/**
	 * Gets the error code from the result
	 * @param Result - The result to check
	 * @return The error code
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Get Error Code"))
	static EHippocacheErrorCode GetErrorCode(const FHippocacheResult& Result);

	/**
	 * Gets the error message from the result
	 * @param Result - The result to check
	 * @return The error message
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Get Error Message"))
	static FString GetErrorMessage(const FHippocacheResult& Result);

	/**
	 * Gets the error context from the result
	 * @param Result - The result to check
	 * @return The error context
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Get Error Context"))
	static FString GetErrorContext(const FHippocacheResult& Result);

	/**
	 * Creates a human-readable description of the result
	 * @param Result - The result to describe
	 * @return A formatted string describing the result
	 */
	UFUNCTION(BlueprintPure, Category = "Hippocache|Result", meta = (DisplayName = "Describe Result"))
	static FString DescribeResult(const FHippocacheResult& Result);

	// ============================================================================
	// CustomThunk Blueprint Universal Functions - Hippoo & Hippop
	// ============================================================================
	
	/**
	 * Universal setter function for Blueprint - automatically detects type and stores any supported value
	 * @param WorldContextObject - World context for subsystem access
	 * @param Collection - Collection name to store in
	 * @param Key - Key to store the value under
	 * @param Value - Value to store (type automatically detected)
	 * @return Result indicating success or failure
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Hippocache|Universal", meta = (WorldContext = "WorldContextObject", DisplayName = "Hippoo (Set)", CustomStructureParam = "Value", CallInEditor = "true"))
	static FHippocacheResult HippooBlueprint(const UObject* WorldContextObject, FName Collection, const FString& Key, const int32& Value);
	DECLARE_FUNCTION(execHippooBlueprint);

	/**
	 * Universal setter function with TTL for Blueprint - automatically detects type and stores any supported value with expiration
	 * @param WorldContextObject - World context for subsystem access
	 * @param Collection - Collection name to store in
	 * @param Key - Key to store the value under
	 * @param Value - Value to store (type automatically detected)
	 * @param TTLSeconds - Time to live in seconds
	 * @return Result indicating success or failure
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Hippocache|Universal", meta = (WorldContext = "WorldContextObject", DisplayName = "Hippoo With TTL (Set)", CustomStructureParam = "Value", CallInEditor = "true"))
	static FHippocacheResult HippooBlueprintWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const int32& Value, float TTLSeconds);
	DECLARE_FUNCTION(execHippooBlueprintWithTTL);

	/**
	 * Universal getter function for Blueprint - automatically detects type and retrieves value
	 * @param WorldContextObject - World context for subsystem access
	 * @param Collection - Collection name to retrieve from
	 * @param Key - Key to retrieve the value from
	 * @param OutValue - Reference to store the retrieved value (type automatically detected)
	 * @return Result indicating success or failure
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Hippocache|Universal", meta = (WorldContext = "WorldContextObject", DisplayName = "Hippop (Get)", CustomStructureParam = "OutValue", CallInEditor = "true"))
	static FHippocacheResult HippopBlueprint(const UObject* WorldContextObject, FName Collection, const FString& Key, int32& OutValue);
	DECLARE_FUNCTION(execHippopBlueprint);

	// FInstancedStruct functions for Blueprint struct support
	
	/**
	 * Sets a struct value in the cache using FInstancedStruct
	 * @param WorldContextObject - Object to get world context from
	 * @param Collection - The collection name
	 * @param Key - The key to store the value under
	 * @param Value - The struct value to store (as FInstancedStruct)
	 * @return Result of the operation
	 */
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Struct", meta = (DisplayName = "Set Instanced Struct", CallInEditor = "true"))
	static FHippocacheResult SetInstancedStruct(const UObject* WorldContextObject, FName Collection, const FString& Key, const FInstancedStruct& Value);

	/**
	 * Sets a struct value in the cache with TTL using FInstancedStruct
	 * @param WorldContextObject - Object to get world context from
	 * @param Collection - The collection name
	 * @param Key - The key to store the value under
	 * @param Value - The struct value to store (as FInstancedStruct)
	 * @param TTLSeconds - Time to live in seconds (0 = no expiration)
	 * @return Result of the operation
	 */
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Struct", meta = (DisplayName = "Set Instanced Struct With TTL", CallInEditor = "true"))
	static FHippocacheResult SetInstancedStructWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FInstancedStruct& Value, float TTLSeconds);

	/**
	 * Gets a struct value from the cache using FInstancedStruct
	 * @param WorldContextObject - Object to get world context from
	 * @param Collection - The collection name
	 * @param Key - The key to retrieve the value from
	 * @param OutValue - The retrieved struct value (as FInstancedStruct)
	 * @return Result of the operation
	 */
	UFUNCTION(BlueprintCallable, Category = "Hippocache|Struct", meta = (DisplayName = "Get Instanced Struct", CallInEditor = "true"))
	static FHippocacheResult GetInstancedStruct(const UObject* WorldContextObject, FName Collection, const FString& Key, FInstancedStruct& OutValue);


private:
	// Helper functions for CustomThunk implementation
	static FHippocacheResult ExecuteHippooForProperty(const UObject* WorldContextObject, FName Collection, const FString& Key, FProperty* ValueProperty, void* ValuePtr, float TTLSeconds, bool bUseTTL);
	static FHippocacheResult ExecuteHippopForProperty(const UObject* WorldContextObject, FName Collection, const FString& Key, FProperty* ValueProperty, void* ValuePtr);

	// Internal implementation functions for universal setter/getter
	template<typename T>
	static FHippocacheResult HippooImpl(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value)
	{
		// Runtime type detection using Unreal's reflection system
		if constexpr (std::is_same_v<T, UObject*> || std::is_base_of_v<UObject, std::remove_pointer_t<T>>)
		{
			// UObject pointers are not supported for safety reasons
			return FHippocacheResult::Error(EHippocacheErrorCode::UnsupportedType, 
				TEXT("UObject pointers are not supported for cache storage"), 
				FString::Printf(TEXT("Collection: %s, Key: %s, Type: UObject*"), *Collection.ToString(), *Key));
		}
		else
		{
			// Check if T has StaticStruct() method (indicates it's a UE struct)
			if constexpr (requires { T::StaticStruct(); })
			{
				// This is a UE struct - use struct storage
				UE_LOG(LogTemp, VeryVerbose, TEXT("Hippoo: Storing as struct - Collection: %s, Key: %s, Type: %s"), 
					*Collection.ToString(), *Key, *T::StaticStruct()->GetName());
				return SetStruct<T>(WorldContextObject, Collection, Key, Value);
			}
			else
			{
				// This is a primitive type - use variant storage
				UE_LOG(LogTemp, VeryVerbose, TEXT("Hippoo: Storing as primitive - Collection: %s, Key: %s"), 
					*Collection.ToString(), *Key);
				return SetPrimitive<T>(WorldContextObject, Collection, Key, Value);
			}
		}
	}
	
	template<typename T>
	static FHippocacheResult HippooImplWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value, float TTLSeconds)
	{
		// Runtime type detection using Unreal's reflection system
		if constexpr (std::is_same_v<T, UObject*> || std::is_base_of_v<UObject, std::remove_pointer_t<T>>)
		{
			// UObject pointers are not supported for safety reasons
			return FHippocacheResult::Error(EHippocacheErrorCode::UnsupportedType, 
				TEXT("UObject pointers are not supported for cache storage"), 
				FString::Printf(TEXT("Collection: %s, Key: %s, Type: UObject*"), *Collection.ToString(), *Key));
		}
		else
		{
			// Check if T has StaticStruct() method (indicates it's a UE struct)
			if constexpr (requires { T::StaticStruct(); })
			{
				// This is a UE struct - use struct storage with TTL
				UE_LOG(LogTemp, VeryVerbose, TEXT("Hippoo TTL: Storing as struct - Collection: %s, Key: %s, Type: %s, TTL: %.2fs"), 
					*Collection.ToString(), *Key, *T::StaticStruct()->GetName(), TTLSeconds);
				return SetStructWithTTL<T>(WorldContextObject, Collection, Key, Value, TTLSeconds);
			}
			else
			{
				// This is a primitive type - use variant storage with TTL
				UE_LOG(LogTemp, VeryVerbose, TEXT("Hippoo TTL: Storing as primitive - Collection: %s, Key: %s, TTL: %.2fs"), 
					*Collection.ToString(), *Key, TTLSeconds);
				return SetPrimitiveWithTTL<T>(WorldContextObject, Collection, Key, Value, TTLSeconds);
			}
		}
	}
	
	template<typename T>
	static FHippocacheResult HippopImpl(const UObject* WorldContextObject, FName Collection, const FString& Key, T& OutValue)
	{
		// Runtime type detection using Unreal's reflection system
		if constexpr (std::is_same_v<T, UObject*> || std::is_base_of_v<UObject, std::remove_pointer_t<T>>)
		{
			// UObject pointers are not supported for safety reasons
			return FHippocacheResult::Error(EHippocacheErrorCode::UnsupportedType, 
				TEXT("UObject pointers are not supported for cache retrieval"), 
				FString::Printf(TEXT("Collection: %s, Key: %s, Type: UObject*"), *Collection.ToString(), *Key));
		}
		else
		{
			// Check if T has StaticStruct() method (indicates it's a UE struct)
			if constexpr (requires { T::StaticStruct(); })
			{
				// This is a UE struct - use struct retrieval
				UE_LOG(LogTemp, VeryVerbose, TEXT("Hippop: Retrieving as struct - Collection: %s, Key: %s, Type: %s"), 
					*Collection.ToString(), *Key, *T::StaticStruct()->GetName());
				auto Result = GetStruct<T>(WorldContextObject, Collection, Key);
				if (Result.IsSuccess())
				{
					OutValue = Result.Value;
					return FHippocacheResult::Success();
				}
				return Result.Result;
			}
			else
			{
				// This is a primitive type - use variant retrieval
				UE_LOG(LogTemp, VeryVerbose, TEXT("Hippop: Retrieving as primitive - Collection: %s, Key: %s"), 
					*Collection.ToString(), *Key);
				return GetPrimitive<T>(WorldContextObject, Collection, Key, OutValue);
			}
		}
	}

private:
	// Helper function to convert FProperty values to FVariant
	static bool PropertyToVariant(FProperty* Property, void* ValuePtr, FVariant& OutVariant);
	
	// Helper function to extract FVariant values back to FProperty
	static bool VariantToProperty(const FVariant& Variant, FProperty* Property, void* ValuePtr);

};