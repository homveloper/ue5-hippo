// Copyright ActionSquare, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HippocacheResult.generated.h"

/**
 * @brief Enumeration of possible error codes in Hippocache operations.
 */
UENUM(BlueprintType)
enum class EHippocacheErrorCode : uint8
{
	None = 0,				// Success - operation completed successfully
	ItemNotFound,			// Item not found in cache (cache miss)
	ItemExpired,			// Item found but expired
	InvalidSubsystem,		// Subsystem is null or invalid
	InvalidClient,			// Client is null or invalid
	InvalidCollection,		// Collection name is invalid
	InvalidKey,				// Key is empty or invalid
	InvalidValue,			// Value is invalid
	TypeMismatch,			// Type mismatch when retrieving value
	WorldNotFound,			// World context not found
	LocalPlayerNotFound,	// Local player not found
	TimerError,				// Timer operation failed
	MemoryAllocationError,	// Memory allocation failed
	UnsupportedType,		// Type is not supported for cache operations
	UnknownError			// Unknown error occurred
};

/**
 * @brief Result structure for Hippocache operations with error context.
 */
USTRUCT(BlueprintType)
struct HIPPOCACHE_API FHippocacheResult
{
	GENERATED_BODY()

	/** Error code - None means success */
	UPROPERTY(BlueprintReadOnly, Category = "Hippocache")
	EHippocacheErrorCode ErrorCode;

	/** Human-readable error message */
	UPROPERTY(BlueprintReadOnly, Category = "Hippocache")
	FString ErrorMessage;

	/** Additional context information */
	UPROPERTY(BlueprintReadOnly, Category = "Hippocache")
	FString ErrorContext;

	FHippocacheResult()
		: ErrorCode(EHippocacheErrorCode::None)
		, ErrorMessage(TEXT(""))
		, ErrorContext(TEXT(""))
	{}

	FHippocacheResult(EHippocacheErrorCode InErrorCode, const FString& InErrorMessage = TEXT(""), const FString& InErrorContext = TEXT(""))
		: ErrorCode(InErrorCode)
		, ErrorMessage(InErrorMessage)
		, ErrorContext(InErrorContext)
	{}

	/** Creates a success result */
	static FHippocacheResult Success()
	{
		return FHippocacheResult(EHippocacheErrorCode::None);
	}

	/** Creates an error result */
	static FHippocacheResult Error(EHippocacheErrorCode InErrorCode, const FString& InErrorMessage, const FString& InErrorContext = TEXT(""))
	{
		return FHippocacheResult(InErrorCode, InErrorMessage, InErrorContext);
	}

	/** Checks if the operation was successful */
	bool IsSuccess() const { return ErrorCode == EHippocacheErrorCode::None; }

	/** Checks if the result is an error */
	bool IsError() const { return ErrorCode != EHippocacheErrorCode::None; }

	/** Checks if the item was not found (cache miss) */
	bool IsNotFound() const { return ErrorCode == EHippocacheErrorCode::ItemNotFound; }

	/** Checks if the item was expired */
	bool IsExpired() const { return ErrorCode == EHippocacheErrorCode::ItemExpired; }

	/** Bool conversion operator for convenient if checks */
	explicit operator bool() const { return IsSuccess(); }
};

/**
 * @brief Template result structure for operations that return a value.
 */
template<typename T>
struct THippocacheResult
{
	FHippocacheResult Result;
	T Value;

	THippocacheResult()
		: Result(FHippocacheResult::Success())
		, Value(T{})
	{}

	THippocacheResult(const FHippocacheResult& InResult)
		: Result(InResult)
		, Value(T{})
	{}

	THippocacheResult(const T& InValue)
		: Result(FHippocacheResult::Success())
		, Value(InValue)
	{}

	static THippocacheResult Success(const T& InValue)
	{
		THippocacheResult<T> OutResult;
		OutResult.Result = FHippocacheResult::Success();
		OutResult.Value = InValue;
		return OutResult;
	}

	static THippocacheResult Error(EHippocacheErrorCode InErrorCode, const FString& InErrorMessage, const FString& InErrorContext = TEXT(""))
	{
		THippocacheResult<T> OutResult;
		OutResult.Result = FHippocacheResult::Error(InErrorCode, InErrorMessage, InErrorContext);
		OutResult.Value = T{};
		return OutResult;
	}

	bool IsSuccess() const { return Result.IsSuccess(); }
	bool IsError() const { return Result.IsError(); }
	bool IsNotFound() const { return Result.IsNotFound(); }
	bool IsExpired() const { return Result.IsExpired(); }

	/** Bool conversion operator for convenient if checks */
	explicit operator bool() const { return IsSuccess(); }
};