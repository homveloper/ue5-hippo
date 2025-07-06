#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HippocacheWrapperStructs.generated.h"

/**
 * Wrapper structs for primitive types to be stored as FInstancedStruct
 * This allows unified handling of all data types in the cache system
 */

USTRUCT()
struct HIPPOCACHE_API FInt32Wrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 Value = 0;
	
	FInt32Wrapper() = default;
	
	explicit FInt32Wrapper(int32 InValue)
		: Value(InValue)
	{
	}
	
	bool operator==(const FInt32Wrapper& Other) const
	{
		return Value == Other.Value;
	}
};

USTRUCT()
struct HIPPOCACHE_API FInt64Wrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	int64 Value = 0;
	
	FInt64Wrapper() = default;
	
	explicit FInt64Wrapper(int64 InValue)
		: Value(InValue)
	{
	}
	
	bool operator==(const FInt64Wrapper& Other) const
	{
		return Value == Other.Value;
	}
};

USTRUCT()
struct HIPPOCACHE_API FFloatWrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	float Value = 0.0f;
	
	FFloatWrapper() = default;
	
	explicit FFloatWrapper(float InValue)
		: Value(InValue)
	{
	}
	
	bool operator==(const FFloatWrapper& Other) const
	{
		return FMath::IsNearlyEqual(Value, Other.Value);
	}
};

USTRUCT()
struct HIPPOCACHE_API FDoubleWrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	double Value = 0.0;
	
	FDoubleWrapper() = default;
	
	explicit FDoubleWrapper(double InValue)
		: Value(InValue)
	{
	}
	
	bool operator==(const FDoubleWrapper& Other) const
	{
		return FMath::IsNearlyEqual(Value, Other.Value);
	}
};

USTRUCT()
struct HIPPOCACHE_API FStringWrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString Value;
	
	FStringWrapper() = default;
	
	explicit FStringWrapper(const FString& InValue)
		: Value(InValue)
	{
	}
	
	bool operator==(const FStringWrapper& Other) const
	{
		return Value == Other.Value;
	}
};

USTRUCT()
struct HIPPOCACHE_API FDateTimeWrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	FDateTime Value;
	
	FDateTimeWrapper() = default;
	
	explicit FDateTimeWrapper(const FDateTime& InValue)
		: Value(InValue)
	{
	}
	
	bool operator==(const FDateTimeWrapper& Other) const
	{
		return Value == Other.Value;
	}
};