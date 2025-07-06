#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HippocacheVariantWrapper.generated.h"

// Forward declare FVariant to avoid UHT issues
class FVariant;

/**
 * Wrapper struct for FVariant to make it compatible with FInstancedStruct
 * Supports all built-in types defined in EVariantTypes automatically
 * 
 * Note: We store the variant data as raw bytes to avoid UHT issues with FVariant
 */
USTRUCT(BlueprintType)
struct HIPPOCACHE_API FVariantWrapper
{
    GENERATED_BODY()

private:
    // Store FVariant as opaque data to avoid UHT issues
    UPROPERTY()
    TArray<uint8> VariantData;
    
    UPROPERTY()
    int32 VariantType;

public:
    // Constructors
    FVariantWrapper();
    
    // Constructor from FVariant (implemented in .cpp)
    explicit FVariantWrapper(const FVariant& InValue);

    // Template constructor for any supported type (implemented in .cpp)
    template<typename T>
    explicit FVariantWrapper(const T& InValue);

    // Get the underlying FVariant (implemented in .cpp)
    FVariant GetVariant() const;

    // Template getter for any supported type (implemented in .cpp)
    template<typename T>
    T GetValue() const;

    // Check if the variant holds a specific type (implemented in .cpp)
    template<typename T>
    bool IsType() const;

    // Get the variant type (implemented in .cpp)
    int32 GetType() const;

    // Check if empty (implemented in .cpp)
    bool IsEmpty() const;

    // Get type name as string for debugging (implemented in .cpp)
    FString GetTypeName() const;
};