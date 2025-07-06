#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "TestStructs.generated.h"

// Test struct for struct operations
USTRUCT()
struct FTestStruct
{
	GENERATED_BODY()

	UPROPERTY()
	int32 IntValue = 0;

	UPROPERTY()
	FString StringValue;

	UPROPERTY()
	float FloatValue = 0.0f;

	bool operator==(const FTestStruct& Other) const
	{
		return IntValue == Other.IntValue &&
			StringValue == Other.StringValue &&
			FMath::IsNearlyEqual(FloatValue, Other.FloatValue);
	}
};
