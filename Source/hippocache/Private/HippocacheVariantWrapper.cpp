#include "HippocacheVariantWrapper.h"
#include "Misc/Variant.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

FVariantWrapper::FVariantWrapper()
    : VariantType(static_cast<int32>(EVariantTypes::Empty))
{
}

FVariantWrapper::FVariantWrapper(const FVariant& InValue)
    : VariantType(static_cast<int32>(InValue.GetType()))
{
    // Serialize FVariant to byte array
    TArray<uint8> Bytes;
    FMemoryWriter Writer(Bytes);
    Writer << const_cast<FVariant&>(InValue);
    VariantData = MoveTemp(Bytes);
}

template<typename T>
FVariantWrapper::FVariantWrapper(const T& InValue)
    : FVariantWrapper(FVariant(InValue))
{
}

FVariant FVariantWrapper::GetVariant() const
{
    if (VariantData.Num() == 0)
    {
        return FVariant();
    }
    
    // Deserialize FVariant from byte array
    FVariant Result;
    FMemoryReader Reader(VariantData);
    Reader << Result;
    return Result;
}

template<typename T>
T FVariantWrapper::GetValue() const
{
    FVariant Variant = GetVariant();
    return Variant.GetValue<T>();
}

template<typename T>
bool FVariantWrapper::IsType() const
{
    FVariant TestVariant(T{});
    return VariantType == static_cast<int32>(TestVariant.GetType());
}

int32 FVariantWrapper::GetType() const
{
    return VariantType;
}

bool FVariantWrapper::IsEmpty() const
{
    return VariantType == static_cast<int32>(EVariantTypes::Empty);
}

FString FVariantWrapper::GetTypeName() const
{
    // FVariant doesn't have GetTypeName(), so we'll create a simple type name based on the enum
    EVariantTypes Type = static_cast<EVariantTypes>(VariantType);
    
    switch (Type)
    {
    case EVariantTypes::Empty: return TEXT("Empty");
    case EVariantTypes::Ansichar: return TEXT("ANSICHAR");
    case EVariantTypes::Bool: return TEXT("bool");
    case EVariantTypes::Box: return TEXT("FBox");
    case EVariantTypes::BoxSphereBounds: return TEXT("FBoxSphereBounds");
    case EVariantTypes::ByteArray: return TEXT("TArray<uint8>");
    case EVariantTypes::Color: return TEXT("FColor");
    case EVariantTypes::DateTime: return TEXT("FDateTime");
    case EVariantTypes::Double: return TEXT("double");
    case EVariantTypes::Enum: return TEXT("Enum");
    case EVariantTypes::Float: return TEXT("float");
    case EVariantTypes::Guid: return TEXT("FGuid");
    case EVariantTypes::Int8: return TEXT("int8");
    case EVariantTypes::Int16: return TEXT("int16");
    case EVariantTypes::Int32: return TEXT("int32");
    case EVariantTypes::Int64: return TEXT("int64");
    case EVariantTypes::IntRect: return TEXT("FIntRect");
    case EVariantTypes::IntPoint: return TEXT("FIntPoint");
    case EVariantTypes::IntVector: return TEXT("FIntVector");
    case EVariantTypes::LinearColor: return TEXT("FLinearColor");
    case EVariantTypes::Matrix: return TEXT("FMatrix");
    case EVariantTypes::Name: return TEXT("FName");
    case EVariantTypes::Plane: return TEXT("FPlane");
    case EVariantTypes::Quat: return TEXT("FQuat");
    case EVariantTypes::RandomStream: return TEXT("FRandomStream");
    case EVariantTypes::Rotator: return TEXT("FRotator");
    case EVariantTypes::String: return TEXT("FString");
    case EVariantTypes::Timespan: return TEXT("FTimespan");
    case EVariantTypes::Transform: return TEXT("FTransform");
    case EVariantTypes::UInt8: return TEXT("uint8");
    case EVariantTypes::UInt16: return TEXT("uint16");
    case EVariantTypes::UInt32: return TEXT("uint32");
    case EVariantTypes::UInt64: return TEXT("uint64");
    case EVariantTypes::Vector: return TEXT("FVector");
    case EVariantTypes::Vector2d: return TEXT("FVector2D"); // Using correct enum name
    case EVariantTypes::Vector4: return TEXT("FVector4");
    case EVariantTypes::Widechar: return TEXT("WIDECHAR");
    case EVariantTypes::TwoVectors: return TEXT("FTwoVectors");
    case EVariantTypes::NetworkGUID: return TEXT("FNetworkGUID");
    default: return TEXT("Unknown");
    }
}

// ============================================================================
// Explicit template instantiations for all supported types
// ============================================================================

// Basic types
template FVariantWrapper::FVariantWrapper(const int8& InValue);
template FVariantWrapper::FVariantWrapper(const int16& InValue);
template FVariantWrapper::FVariantWrapper(const int32& InValue);
template FVariantWrapper::FVariantWrapper(const int64& InValue);
template FVariantWrapper::FVariantWrapper(const uint8& InValue);
template FVariantWrapper::FVariantWrapper(const uint16& InValue);
template FVariantWrapper::FVariantWrapper(const uint32& InValue);
template FVariantWrapper::FVariantWrapper(const uint64& InValue);
template FVariantWrapper::FVariantWrapper(const float& InValue);
template FVariantWrapper::FVariantWrapper(const double& InValue);
template FVariantWrapper::FVariantWrapper(const bool& InValue);
template FVariantWrapper::FVariantWrapper(const ANSICHAR& InValue);
template FVariantWrapper::FVariantWrapper(const WIDECHAR& InValue);

// String types
template FVariantWrapper::FVariantWrapper(const FString& InValue);
template FVariantWrapper::FVariantWrapper(const FName& InValue);

// Time types
template FVariantWrapper::FVariantWrapper(const FDateTime& InValue);
template FVariantWrapper::FVariantWrapper(const FTimespan& InValue);

// Math types
template FVariantWrapper::FVariantWrapper(const FVector& InValue);
template FVariantWrapper::FVariantWrapper(const FVector2D& InValue);
template FVariantWrapper::FVariantWrapper(const FVector4& InValue);
template FVariantWrapper::FVariantWrapper(const FRotator& InValue);
template FVariantWrapper::FVariantWrapper(const FQuat& InValue);
template FVariantWrapper::FVariantWrapper(const FTransform& InValue);
template FVariantWrapper::FVariantWrapper(const FMatrix& InValue);
template FVariantWrapper::FVariantWrapper(const FPlane& InValue);

// Geometry types
template FVariantWrapper::FVariantWrapper(const FBox& InValue);
template FVariantWrapper::FVariantWrapper(const FIntRect& InValue);
template FVariantWrapper::FVariantWrapper(const FIntPoint& InValue);
template FVariantWrapper::FVariantWrapper(const FIntVector& InValue);

// Color types
template FVariantWrapper::FVariantWrapper(const FColor& InValue);
template FVariantWrapper::FVariantWrapper(const FLinearColor& InValue);

// Other types
template FVariantWrapper::FVariantWrapper(const FGuid& InValue);

// GetValue instantiations
template int8 FVariantWrapper::GetValue<int8>() const;
template int16 FVariantWrapper::GetValue<int16>() const;
template int32 FVariantWrapper::GetValue<int32>() const;
template int64 FVariantWrapper::GetValue<int64>() const;
template uint8 FVariantWrapper::GetValue<uint8>() const;
template uint16 FVariantWrapper::GetValue<uint16>() const;
template uint32 FVariantWrapper::GetValue<uint32>() const;
template uint64 FVariantWrapper::GetValue<uint64>() const;
template float FVariantWrapper::GetValue<float>() const;
template double FVariantWrapper::GetValue<double>() const;
template bool FVariantWrapper::GetValue<bool>() const;
template ANSICHAR FVariantWrapper::GetValue<ANSICHAR>() const;
template WIDECHAR FVariantWrapper::GetValue<WIDECHAR>() const;
template FString FVariantWrapper::GetValue<FString>() const;
template FName FVariantWrapper::GetValue<FName>() const;
template FDateTime FVariantWrapper::GetValue<FDateTime>() const;
template FTimespan FVariantWrapper::GetValue<FTimespan>() const;
template FVector FVariantWrapper::GetValue<FVector>() const;
template FVector2D FVariantWrapper::GetValue<FVector2D>() const;
template FVector4 FVariantWrapper::GetValue<FVector4>() const;
template FRotator FVariantWrapper::GetValue<FRotator>() const;
template FQuat FVariantWrapper::GetValue<FQuat>() const;
template FTransform FVariantWrapper::GetValue<FTransform>() const;
template FMatrix FVariantWrapper::GetValue<FMatrix>() const;
template FPlane FVariantWrapper::GetValue<FPlane>() const;
template FBox FVariantWrapper::GetValue<FBox>() const;
template FIntRect FVariantWrapper::GetValue<FIntRect>() const;
template FIntPoint FVariantWrapper::GetValue<FIntPoint>() const;
template FIntVector FVariantWrapper::GetValue<FIntVector>() const;
template FColor FVariantWrapper::GetValue<FColor>() const;
template FLinearColor FVariantWrapper::GetValue<FLinearColor>() const;
template FGuid FVariantWrapper::GetValue<FGuid>() const;

// IsType instantiations
template bool FVariantWrapper::IsType<int8>() const;
template bool FVariantWrapper::IsType<int16>() const;
template bool FVariantWrapper::IsType<int32>() const;
template bool FVariantWrapper::IsType<int64>() const;
template bool FVariantWrapper::IsType<uint8>() const;
template bool FVariantWrapper::IsType<uint16>() const;
template bool FVariantWrapper::IsType<uint32>() const;
template bool FVariantWrapper::IsType<uint64>() const;
template bool FVariantWrapper::IsType<float>() const;
template bool FVariantWrapper::IsType<double>() const;
template bool FVariantWrapper::IsType<bool>() const;
template bool FVariantWrapper::IsType<ANSICHAR>() const;
template bool FVariantWrapper::IsType<WIDECHAR>() const;
template bool FVariantWrapper::IsType<FString>() const;
template bool FVariantWrapper::IsType<FName>() const;
template bool FVariantWrapper::IsType<FDateTime>() const;
template bool FVariantWrapper::IsType<FTimespan>() const;
template bool FVariantWrapper::IsType<FVector>() const;
template bool FVariantWrapper::IsType<FVector2D>() const;
template bool FVariantWrapper::IsType<FVector4>() const;
template bool FVariantWrapper::IsType<FRotator>() const;
template bool FVariantWrapper::IsType<FQuat>() const;
template bool FVariantWrapper::IsType<FTransform>() const;
template bool FVariantWrapper::IsType<FMatrix>() const;
template bool FVariantWrapper::IsType<FPlane>() const;
template bool FVariantWrapper::IsType<FBox>() const;
template bool FVariantWrapper::IsType<FIntRect>() const;
template bool FVariantWrapper::IsType<FIntPoint>() const;
template bool FVariantWrapper::IsType<FIntVector>() const;
template bool FVariantWrapper::IsType<FColor>() const;
template bool FVariantWrapper::IsType<FLinearColor>() const;
template bool FVariantWrapper::IsType<FGuid>() const;