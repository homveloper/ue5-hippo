#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"

#include "WeirdTestStructs.generated.h"

// Empty struct - what happens?
USTRUCT()
struct FEmptyStruct
{
	GENERATED_BODY()
	
	bool operator==(const FEmptyStruct& Other) const
	{
		return true; // Always equal since it's empty
	}
};

// Struct with only static data
USTRUCT()
struct FStaticOnlyStruct
{
	GENERATED_BODY()
	
	static constexpr int32 StaticValue = 42;
	static const FString StaticString;
	
	bool operator==(const FStaticOnlyStruct& Other) const
	{
		return true; // No instance data to compare
	}
};

// Huge struct with lots of data
USTRUCT()
struct FHugeStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<int32> MassiveArray;
	
	UPROPERTY()
	TMap<FString, FString> HugeMap;
	
	UPROPERTY()
	FString VeryLongString;
	
	UPROPERTY()
	TArray<FVector> LotsOfVectors;
	
	UPROPERTY()
	TArray<FTransform> ManyTransforms;
	
	bool operator==(const FHugeStruct& Other) const
	{
		// Simple comparison by sizes and key values since TMap == is deleted
		if (MassiveArray.Num() != Other.MassiveArray.Num() ||
		    HugeMap.Num() != Other.HugeMap.Num() ||
		    VeryLongString != Other.VeryLongString ||
		    LotsOfVectors.Num() != Other.LotsOfVectors.Num() ||
		    ManyTransforms.Num() != Other.ManyTransforms.Num())
		{
			return false;
		}
		
		// Check some array elements for basic equality
		for (int32 i = 0; i < FMath::Min(10, MassiveArray.Num()); ++i)
		{
			if (MassiveArray[i] != Other.MassiveArray[i])
			{
				return false;
			}
		}
		
		return true;
	}
};

// Nested struct madness
USTRUCT()
struct FInnerStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 Value = 0;
	
	UPROPERTY()
	FString Name;
	
	bool operator==(const FInnerStruct& Other) const
	{
		return Value == Other.Value && Name == Other.Name;
	}
};

USTRUCT()
struct FNestedStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	FInnerStruct Inner;
	
	UPROPERTY()
	TArray<FInnerStruct> InnerArray;
	
	UPROPERTY()
	TMap<FString, FInnerStruct> InnerMap;
	
	bool operator==(const FNestedStruct& Other) const
	{
		// Check inner struct first
		if (!(Inner == Other.Inner))
		{
			return false;
		}
		
		// Check array sizes and sample elements
		if (InnerArray.Num() != Other.InnerArray.Num())
		{
			return false;
		}
		
		// Sample first few elements of array
		for (int32 i = 0; i < FMath::Min(5, InnerArray.Num()); ++i)
		{
			if (!(InnerArray[i] == Other.InnerArray[i]))
			{
				return false;
			}
		}
		
		// Check map sizes (can't use TMap == operator)
		if (InnerMap.Num() != Other.InnerMap.Num())
		{
			return false;
		}
		
		// Sample a few keys from the map
		int32 CheckCount = 0;
		for (const auto& Pair : InnerMap)
		{
			if (const FInnerStruct* OtherValue = Other.InnerMap.Find(Pair.Key))
			{
				if (!(Pair.Value == *OtherValue))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
			
			CheckCount++;
			if (CheckCount >= 5) break; // Only check first 5 entries
		}
		
		return true;
	}
};

// Struct with circular reference potential (modified to avoid UHT error)
USTRUCT()
struct FCircularStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString Name;
	
	UPROPERTY()
	int32 ChildCount = 0;
	
	UPROPERTY()
	FString ChildData; // Instead of actual children, store serialized data
	
	bool operator==(const FCircularStruct& Other) const
	{
		return Name == Other.Name && 
			   ChildCount == Other.ChildCount && 
			   ChildData == Other.ChildData;
	}
};

// Struct with UObject pointers (dangerous!)
USTRUCT()
struct FObjectPointerStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UObject> ObjectPtr = nullptr;
	
	UPROPERTY()
	TWeakObjectPtr<UObject> WeakObjectPtr = nullptr;
	
	UPROPERTY()
	TSoftObjectPtr<UTexture2D> SoftObjectPtr = nullptr;
	
	bool operator==(const FObjectPointerStruct& Other) const
	{
		return ObjectPtr == Other.ObjectPtr && 
			   WeakObjectPtr == Other.WeakObjectPtr && 
			   SoftObjectPtr == Other.SoftObjectPtr;
	}
};

// Struct with delegates and functions (very weird)
DECLARE_DYNAMIC_DELEGATE_OneParam(FWeirdDelegate, int32, Value);

USTRUCT()
struct FDelegateStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	FWeirdDelegate WeirdDelegate;
	
	// Note: Can't really compare delegates properly
	bool operator==(const FDelegateStruct& Other) const
	{
		return true; // Always "equal" for testing purposes
	}
};

// Struct with extremely specific types
USTRUCT()
struct FSpecialTypesStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGuid UniqueId;
	
	UPROPERTY()
	FDateTime TimeStamp;
	
	UPROPERTY()
	FTimespan Duration;
	
	UPROPERTY()
	FColor Color;
	
	UPROPERTY()
	FLinearColor LinearColor;
	
	UPROPERTY()
	FVector2D Vector2D;
	
	UPROPERTY()
	FVector Vector;
	
	UPROPERTY()
	FVector4 Vector4 = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
	
	UPROPERTY()
	FQuat Quaternion;
	
	UPROPERTY()
	FRotator Rotator;
	
	UPROPERTY()
	FTransform Transform;
	
	bool operator==(const FSpecialTypesStruct& Other) const
	{
		return UniqueId == Other.UniqueId &&
			   TimeStamp == Other.TimeStamp &&
			   Duration == Other.Duration &&
			   Color == Other.Color &&
			   LinearColor == Other.LinearColor &&
			   Vector2D == Other.Vector2D &&
			   Vector == Other.Vector &&
			   Vector4 == Other.Vector4 &&
			   Quaternion == Other.Quaternion &&
			   Rotator == Other.Rotator &&
			   Transform.Equals(Other.Transform);
	}
};

// Inheritance struct (weird hierarchy)
USTRUCT()
struct FBaseStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 BaseValue = 0;
	
	bool operator==(const FBaseStruct& Other) const
	{
		return BaseValue == Other.BaseValue;
	}
};

USTRUCT()
struct FDerivedStruct : public FBaseStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString DerivedValue;
	
	bool operator==(const FDerivedStruct& Other) const
	{
		return FBaseStruct::operator==(Other) && DerivedValue == Other.DerivedValue;
	}
};

// Struct with bitfields and packed data
USTRUCT()
struct FPackedStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool Flag1;
	
	UPROPERTY()
	bool Flag2;
	
	UPROPERTY()
	uint8 SmallValue;
	
	UPROPERTY()
	uint32 PackedData;
	
	FPackedStruct()
		: Flag1(false)
		, Flag2(false)
		, SmallValue(0)
		, PackedData(0)
	{
	}
	
	bool operator==(const FPackedStruct& Other) const
	{
		return Flag1 == Other.Flag1 && 
			   Flag2 == Other.Flag2 && 
			   SmallValue == Other.SmallValue && 
			   PackedData == Other.PackedData;
	}
};

// DataTable row struct
USTRUCT(BlueprintType)
struct FDataTableRowStruct : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemValue = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ItemWeight = 0.0f;
	
	bool operator==(const FDataTableRowStruct& Other) const
	{
		return ItemName == Other.ItemName && 
			   ItemValue == Other.ItemValue && 
			   FMath::IsNearlyEqual(ItemWeight, Other.ItemWeight);
	}
};

