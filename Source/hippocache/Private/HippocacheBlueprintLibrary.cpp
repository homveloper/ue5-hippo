#include "HippocacheBlueprintLibrary.h"
#include "HippocacheSubsystem.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include "JsonObjectConverter.h"

// Helper function to get subsystem with error handling
FHippocacheResult UHippocacheBlueprintLibrary::GetSubsystemSafe(const UObject* WorldContextObject, UHippocacheSubsystem*& OutSubsystem)
{
	return UHippocacheSubsystem::Get(WorldContextObject, OutSubsystem);
}

// ============================================================================
// Template helper functions for FVariant-based primitive type handling
// ============================================================================
// Note: Template implementations are now in the header file for proper linking

// ============================================================================
// Int32 operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetInt32WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, int32 Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<int32>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetInt32(const UObject* WorldContextObject, FName Collection, const FString& Key, int32 Value)
{
	return SetPrimitive<int32>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetInt32(const UObject* WorldContextObject, FName Collection, const FString& Key, int32& OutValue)
{
	return GetPrimitive<int32>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Int64 operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetInt64WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, int64 Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<int64>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetInt64(const UObject* WorldContextObject, FName Collection, const FString& Key, int64 Value)
{
	return SetPrimitive<int64>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetInt64(const UObject* WorldContextObject, FName Collection, const FString& Key, int64& OutValue)
{
	return GetPrimitive<int64>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Float operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetFloatWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, float Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<float>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetFloat(const UObject* WorldContextObject, FName Collection, const FString& Key, float Value)
{
	return SetPrimitive<float>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetFloat(const UObject* WorldContextObject, FName Collection, const FString& Key, float& OutValue)
{
	return GetPrimitive<float>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Double operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetDoubleWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, double Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<double>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetDouble(const UObject* WorldContextObject, FName Collection, const FString& Key, double Value)
{
	return SetPrimitive<double>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetDouble(const UObject* WorldContextObject, FName Collection, const FString& Key, double& OutValue)
{
	return GetPrimitive<double>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// String operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetStringWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FString& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FString>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetString(const UObject* WorldContextObject, FName Collection, const FString& Key, const FString& Value)
{
	return SetPrimitive<FString>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetString(const UObject* WorldContextObject, FName Collection, const FString& Key, FString& OutValue)
{
	return GetPrimitive<FString>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// DateTime operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetDateTimeWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FDateTime& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FDateTime>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetDateTime(const UObject* WorldContextObject, FName Collection, const FString& Key, const FDateTime& Value)
{
	return SetPrimitive<FDateTime>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetDateTime(const UObject* WorldContextObject, FName Collection, const FString& Key, FDateTime& OutValue)
{
	return GetPrimitive<FDateTime>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Bool operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetBoolWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, bool Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<bool>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetBool(const UObject* WorldContextObject, FName Collection, const FString& Key, bool Value)
{
	return SetPrimitive<bool>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetBool(const UObject* WorldContextObject, FName Collection, const FString& Key, bool& OutValue)
{
	return GetPrimitive<bool>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Int8 operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetInt8WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, int8 Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<int8>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetInt8(const UObject* WorldContextObject, FName Collection, const FString& Key, int8 Value)
{
	return SetPrimitive<int8>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetInt8(const UObject* WorldContextObject, FName Collection, const FString& Key, int8& OutValue)
{
	return GetPrimitive<int8>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// UInt8 (Byte) operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetByteWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, uint8 Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<uint8>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetByte(const UObject* WorldContextObject, FName Collection, const FString& Key, uint8 Value)
{
	return SetPrimitive<uint8>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetByte(const UObject* WorldContextObject, FName Collection, const FString& Key, uint8& OutValue)
{
	return GetPrimitive<uint8>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Name operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetNameWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, FName Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FName>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetName(const UObject* WorldContextObject, FName Collection, const FString& Key, FName Value)
{
	return SetPrimitive<FName>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetName(const UObject* WorldContextObject, FName Collection, const FString& Key, FName& OutValue)
{
	return GetPrimitive<FName>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Vector operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetVectorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FVector>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetVector(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector& Value)
{
	return SetPrimitive<FVector>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetVector(const UObject* WorldContextObject, FName Collection, const FString& Key, FVector& OutValue)
{
	return GetPrimitive<FVector>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Vector2D operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetVector2DWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector2D& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FVector2D>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetVector2D(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector2D& Value)
{
	return SetPrimitive<FVector2D>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetVector2D(const UObject* WorldContextObject, FName Collection, const FString& Key, FVector2D& OutValue)
{
	return GetPrimitive<FVector2D>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Vector4 operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetVector4WithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector4& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FVector4>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetVector4(const UObject* WorldContextObject, FName Collection, const FString& Key, const FVector4& Value)
{
	return SetPrimitive<FVector4>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetVector4(const UObject* WorldContextObject, FName Collection, const FString& Key, FVector4& OutValue)
{
	return GetPrimitive<FVector4>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Rotator operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetRotatorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FRotator& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FRotator>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetRotator(const UObject* WorldContextObject, FName Collection, const FString& Key, const FRotator& Value)
{
	return SetPrimitive<FRotator>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetRotator(const UObject* WorldContextObject, FName Collection, const FString& Key, FRotator& OutValue)
{
	return GetPrimitive<FRotator>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Transform operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetTransformWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTransform& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FTransform>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetTransform(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTransform& Value)
{
	return SetPrimitive<FTransform>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetTransform(const UObject* WorldContextObject, FName Collection, const FString& Key, FTransform& OutValue)
{
	return GetPrimitive<FTransform>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Color operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetColorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FColor& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FColor>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetColor(const UObject* WorldContextObject, FName Collection, const FString& Key, const FColor& Value)
{
	return SetPrimitive<FColor>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetColor(const UObject* WorldContextObject, FName Collection, const FString& Key, FColor& OutValue)
{
	return GetPrimitive<FColor>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// LinearColor operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetLinearColorWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FLinearColor& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FLinearColor>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetLinearColor(const UObject* WorldContextObject, FName Collection, const FString& Key, const FLinearColor& Value)
{
	return SetPrimitive<FLinearColor>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetLinearColor(const UObject* WorldContextObject, FName Collection, const FString& Key, FLinearColor& OutValue)
{
	return GetPrimitive<FLinearColor>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Quat operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetQuatWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FQuat& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FQuat>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetQuat(const UObject* WorldContextObject, FName Collection, const FString& Key, const FQuat& Value)
{
	return SetPrimitive<FQuat>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetQuat(const UObject* WorldContextObject, FName Collection, const FString& Key, FQuat& OutValue)
{
	return GetPrimitive<FQuat>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Timespan operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetTimespanWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTimespan& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FTimespan>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetTimespan(const UObject* WorldContextObject, FName Collection, const FString& Key, const FTimespan& Value)
{
	return SetPrimitive<FTimespan>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetTimespan(const UObject* WorldContextObject, FName Collection, const FString& Key, FTimespan& OutValue)
{
	return GetPrimitive<FTimespan>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Guid operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::SetGuidWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FGuid& Value, float TTLSeconds)
{
	return SetPrimitiveWithTTL<FGuid>(WorldContextObject, Collection, Key, Value, TTLSeconds);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetGuid(const UObject* WorldContextObject, FName Collection, const FString& Key, const FGuid& Value)
{
	return SetPrimitive<FGuid>(WorldContextObject, Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::GetGuid(const UObject* WorldContextObject, FName Collection, const FString& Key, FGuid& OutValue)
{
	return GetPrimitive<FGuid>(WorldContextObject, Collection, Key, OutValue);
}

// ============================================================================
// Cache management operations
// ============================================================================
FHippocacheResult UHippocacheBlueprintLibrary::Remove(const UObject* WorldContextObject, FName Collection, const FString& Key)
{
	UHippocacheSubsystem* Subsystem = nullptr;
	FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
	if (Result.IsError())
	{
		return Result;
	}

	return Subsystem->Remove(Collection, Key);
}

FHippocacheResult UHippocacheBlueprintLibrary::Clear(const UObject* WorldContextObject, FName Collection)
{
	UHippocacheSubsystem* Subsystem = nullptr;
	FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
	if (Result.IsError())
	{
		return Result;
	}

	return Subsystem->Clear(Collection);
}

FHippocacheResult UHippocacheBlueprintLibrary::Num(const UObject* WorldContextObject, FName Collection, int32& OutCount)
{
	UHippocacheSubsystem* Subsystem = nullptr;
	FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
	if (Result.IsError())
	{
		return Result;
	}

	return Subsystem->Num(Collection, OutCount);
}

// ============================================================================
// Result Helper Functions for Blueprint
// ============================================================================

bool UHippocacheBlueprintLibrary::IsSuccess(const FHippocacheResult& Result)
{
	return Result.IsSuccess();
}

bool UHippocacheBlueprintLibrary::IsError(const FHippocacheResult& Result)
{
	return Result.IsError();
}

bool UHippocacheBlueprintLibrary::IsNotFound(const FHippocacheResult& Result)
{
	return Result.IsNotFound();
}

bool UHippocacheBlueprintLibrary::IsExpired(const FHippocacheResult& Result)
{
	return Result.IsExpired();
}

bool UHippocacheBlueprintLibrary::IsTypeMismatch(const FHippocacheResult& Result)
{
	return Result.ErrorCode == EHippocacheErrorCode::TypeMismatch;
}

EHippocacheErrorCode UHippocacheBlueprintLibrary::GetErrorCode(const FHippocacheResult& Result)
{
	return Result.ErrorCode;
}

FString UHippocacheBlueprintLibrary::GetErrorMessage(const FHippocacheResult& Result)
{
	return Result.ErrorMessage;
}

FString UHippocacheBlueprintLibrary::GetErrorContext(const FHippocacheResult& Result)
{
	return Result.ErrorContext;
}

FString UHippocacheBlueprintLibrary::DescribeResult(const FHippocacheResult& Result)
{
	if (Result.IsSuccess())
	{
		return TEXT("Success");
	}
	
	FString Description;
	switch (Result.ErrorCode)
	{
	case EHippocacheErrorCode::ItemNotFound:
		Description = TEXT("Item Not Found");
		break;
	case EHippocacheErrorCode::ItemExpired:
		Description = TEXT("Item Expired");
		break;
	case EHippocacheErrorCode::InvalidSubsystem:
		Description = TEXT("Invalid Subsystem");
		break;
	case EHippocacheErrorCode::InvalidClient:
		Description = TEXT("Invalid Client");
		break;
	case EHippocacheErrorCode::InvalidCollection:
		Description = TEXT("Invalid Collection");
		break;
	case EHippocacheErrorCode::InvalidKey:
		Description = TEXT("Invalid Key");
		break;
	case EHippocacheErrorCode::InvalidValue:
		Description = TEXT("Invalid Value");
		break;
	case EHippocacheErrorCode::TypeMismatch:
		Description = TEXT("Type Mismatch");
		break;
	case EHippocacheErrorCode::WorldNotFound:
		Description = TEXT("World Not Found");
		break;
	case EHippocacheErrorCode::LocalPlayerNotFound:
		Description = TEXT("Local Player Not Found");
		break;
	case EHippocacheErrorCode::TimerError:
		Description = TEXT("Timer Error");
		break;
	case EHippocacheErrorCode::MemoryAllocationError:
		Description = TEXT("Memory Allocation Error");
		break;
	case EHippocacheErrorCode::UnsupportedType:
		Description = TEXT("Unsupported Type");
		break;
	case EHippocacheErrorCode::UnknownError:
	default:
		Description = TEXT("Unknown Error");
		break;
	}
	
	if (!Result.ErrorMessage.IsEmpty())
	{
		Description += FString::Printf(TEXT(": %s"), *Result.ErrorMessage);
	}
	
	if (!Result.ErrorContext.IsEmpty())
	{
		Description += FString::Printf(TEXT(" (%s)"), *Result.ErrorContext);
	}
	
	return Description;
}

// ============================================================================
// FInstancedStruct functions for Blueprint struct support
// ============================================================================

FHippocacheResult UHippocacheBlueprintLibrary::SetInstancedStruct(const UObject* WorldContextObject, FName Collection, const FString& Key, const FInstancedStruct& Value)
{
	UHippocacheSubsystem* Subsystem = nullptr;
	FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
	if (Result.IsError())
	{
		return Result;
	}

	return Subsystem->SetStruct(Collection, Key, Value);
}

FHippocacheResult UHippocacheBlueprintLibrary::SetInstancedStructWithTTL(const UObject* WorldContextObject, FName Collection, const FString& Key, const FInstancedStruct& Value, float TTLSeconds)
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

FHippocacheResult UHippocacheBlueprintLibrary::GetInstancedStruct(const UObject* WorldContextObject, FName Collection, const FString& Key, FInstancedStruct& OutValue)
{
	UHippocacheSubsystem* Subsystem = nullptr;
	FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
	if (Result.IsError())
	{
		return Result;
	}

	return Subsystem->GetStruct(Collection, Key, OutValue);
}

// ============================================================================
// Universal Setter/Getter Implementation - Hippoo & Hippop
// ============================================================================
// Note: Template implementations are now in the header file for proper linking

// ============================================================================
// CustomThunk Blueprint Universal Functions Implementation
// ============================================================================

DEFINE_FUNCTION(UHippocacheBlueprintLibrary::execHippooBlueprint)
{
	// Get parameters from Blueprint stack
	P_GET_OBJECT_REF(UObject, WorldContextObject);
	P_GET_PROPERTY(FNameProperty, Collection);
	P_GET_PROPERTY(FStrProperty, Key);
	
	// Get the Value parameter - this is the wildcard parameter
	FProperty* ValueProperty = nullptr;
	Stack.Step(Context, ValueProperty);
	void* ValuePtr = Stack.MostRecentPropertyAddress;
	
	// If ValueProperty is null, get it from MostRecentProperty
	if (!ValueProperty)
	{
		ValueProperty = Stack.MostRecentProperty;
	}
	
	P_FINISH;
	
	if (!ValueProperty || !ValuePtr)
	{
		*(FHippocacheResult*)RESULT_PARAM = FHippocacheResult::Error(EHippocacheErrorCode::InvalidValue, 
			TEXT("Failed to get value parameter"), TEXT("CustomThunk HippooBlueprint"));
		return;
	}
	
	FHippocacheResult Result = ExecuteHippooForProperty(WorldContextObject, Collection, Key, ValueProperty, ValuePtr, 0.0f, false);
	*(FHippocacheResult*)RESULT_PARAM = Result;
}

DEFINE_FUNCTION(UHippocacheBlueprintLibrary::execHippooBlueprintWithTTL)
{
	// Get parameters from Blueprint stack
	P_GET_OBJECT_REF(UObject, WorldContextObject);
	P_GET_PROPERTY(FNameProperty, Collection);
	P_GET_PROPERTY(FStrProperty, Key);
	
	// Get the Value parameter - this is the wildcard parameter
	FProperty* ValueProperty = nullptr;
	Stack.Step(Context, ValueProperty);
	void* ValuePtr = Stack.MostRecentPropertyAddress;
	
	// If ValueProperty is null, get it from MostRecentProperty
	if (!ValueProperty)
	{
		ValueProperty = Stack.MostRecentProperty;
	}
	
	P_GET_PROPERTY(FFloatProperty, TTLSeconds);
	P_FINISH;
	
	if (!ValueProperty || !ValuePtr)
	{
		*(FHippocacheResult*)RESULT_PARAM = FHippocacheResult::Error(EHippocacheErrorCode::InvalidValue, 
			TEXT("Failed to get value parameter"), TEXT("CustomThunk HippooBlueprintWithTTL"));
		return;
	}
	
	FHippocacheResult Result = ExecuteHippooForProperty(WorldContextObject, Collection, Key, ValueProperty, ValuePtr, TTLSeconds, true);
	*(FHippocacheResult*)RESULT_PARAM = Result;
}

DEFINE_FUNCTION(UHippocacheBlueprintLibrary::execHippopBlueprint)
{
	// Get parameters from Blueprint stack
	P_GET_OBJECT_REF(UObject, WorldContextObject);
	P_GET_PROPERTY(FNameProperty, Collection);
	P_GET_PROPERTY(FStrProperty, Key);
	
	// Get the OutValue parameter - this is the wildcard parameter
	FProperty* ValueProperty = nullptr;
	Stack.Step(Context, ValueProperty);
	void* ValuePtr = Stack.MostRecentPropertyAddress;
	
	// If ValueProperty is null, get it from MostRecentProperty
	if (!ValueProperty)
	{
		ValueProperty = Stack.MostRecentProperty;
	}
	
	P_FINISH;
	
	if (!ValueProperty || !ValuePtr)
	{
		*(FHippocacheResult*)RESULT_PARAM = FHippocacheResult::Error(EHippocacheErrorCode::InvalidValue, 
			TEXT("Failed to get value parameter"), TEXT("CustomThunk HippopBlueprint"));
		return;
	}
	
	FHippocacheResult Result = ExecuteHippopForProperty(WorldContextObject, Collection, Key, ValueProperty, ValuePtr);
	*(FHippocacheResult*)RESULT_PARAM = Result;
}

// Template helper function to execute Hippoo with type dispatch
template<typename T>
FHippocacheResult ExecuteHippooWithType(const UObject* WorldContextObject, FName Collection, const FString& Key, const T& Value, float TTLSeconds, bool bUseTTL)
{
	return bUseTTL ? UHippocacheBlueprintLibrary::Hippoo(WorldContextObject, Collection, Key, Value, TTLSeconds) 
	               : UHippocacheBlueprintLibrary::Hippoo(WorldContextObject, Collection, Key, Value);
}

// Helper function to convert FProperty values to FVariant
bool UHippocacheBlueprintLibrary::PropertyToVariant(FProperty* Property, void* ValuePtr, FVariant& OutVariant)
{
	if (!Property || !ValuePtr)
	{
		return false;
	}

	if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		bool Value = BoolProp->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	{
		uint8 Value = ByteProp->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		int32 Value = IntProp->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}
	else if (FInt64Property* Int64Prop = CastField<FInt64Property>(Property))
	{
		int64 Value = Int64Prop->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		float Value = FloatProp->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
	{
		double Value = DoubleProp->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}
	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		FString Value = StrProp->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}
	else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
	{
		FName Value = NameProp->GetPropertyValue(ValuePtr);
		OutVariant = FVariant(Value);
		return true;
	}

	// Unsupported property type for FVariant conversion
	return false;
}

// Helper function to extract FVariant values back to FProperty
bool UHippocacheBlueprintLibrary::VariantToProperty(const FVariant& Variant, FProperty* Property, void* ValuePtr)
{
	if (!Property || !ValuePtr)
	{
		return false;
	}

	if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		bool Value = Variant.GetValue<bool>();
		BoolProp->SetPropertyValue(ValuePtr, Value);
		return true;
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	{
		uint8 Value = Variant.GetValue<uint8>();
		ByteProp->SetPropertyValue(ValuePtr, Value);
		return true;
	}
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		int32 Value = Variant.GetValue<int32>();
		IntProp->SetPropertyValue(ValuePtr, Value);
		return true;
	}
	else if (FInt64Property* Int64Prop = CastField<FInt64Property>(Property))
	{
		int64 Value = Variant.GetValue<int64>();
		Int64Prop->SetPropertyValue(ValuePtr, Value);
		return true;
	}
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		float Value = Variant.GetValue<float>();
		FloatProp->SetPropertyValue(ValuePtr, Value);
		return true;
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
	{
		double Value = Variant.GetValue<double>();
		DoubleProp->SetPropertyValue(ValuePtr, Value);
		return true;
	}
	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		FString Value = Variant.GetValue<FString>();
		StrProp->SetPropertyValue(ValuePtr, Value);
		return true;
	}
	else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
	{
		FName Value = Variant.GetValue<FName>();
		NameProp->SetPropertyValue(ValuePtr, Value);
		return true;
	}

	// Unsupported property type or type mismatch
	return false;
}

// Helper function to execute Hippoo based on property type using template dispatch
FHippocacheResult UHippocacheBlueprintLibrary::ExecuteHippooForProperty(const UObject* WorldContextObject, FName Collection, const FString& Key, FProperty* ValueProperty, void* ValuePtr, float TTLSeconds, bool bUseTTL)
{
	if (!ValueProperty || !ValuePtr)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidValue, TEXT("Invalid property or value pointer"), TEXT("ExecuteHippooForProperty"));
	}
	
	// Check if it's a struct property - handle with FInstancedStruct
	if (FStructProperty* StructProp = CastField<FStructProperty>(ValueProperty))
	{
		UScriptStruct* StructType = StructProp->Struct;
		if (!StructType)
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::UnsupportedType, TEXT("Invalid struct type"), TEXT("ExecuteHippooForProperty"));
		}
		
		// Convert all structs to FInstancedStruct and let the template Hippoo handle it
		FInstancedStruct InstancedStruct;
		InstancedStruct.InitializeAs(StructType, static_cast<uint8*>(ValuePtr));
		return ExecuteHippooWithType(WorldContextObject, Collection, Key, InstancedStruct, TTLSeconds, bUseTTL);
	}
	else
	{
		// All non-struct types: use FVariant wrapper via SetPrimitive template
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		// Create FVariant from the property value and wrap it
		FVariant Variant;
		if (!PropertyToVariant(ValueProperty, ValuePtr, Variant))
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::UnsupportedType, 
				FString::Printf(TEXT("Cannot convert property type to FVariant: %s"), *ValueProperty->GetClass()->GetName()), 
				TEXT("ExecuteHippooForProperty"));
		}

		// Wrap in FVariantWrapper and store as FInstancedStruct
		FVariantWrapper Wrapper(Variant);
		FInstancedStruct InstancedStruct = FInstancedStruct::Make<FVariantWrapper>(Wrapper);
		
		return bUseTTL ? Subsystem->SetStructWithTTL(Collection, Key, InstancedStruct, FTimespan::FromSeconds(TTLSeconds))
		               : Subsystem->SetStruct(Collection, Key, InstancedStruct);
	}
}

// Template helper function to execute Hippop with type dispatch
template<typename T>
FHippocacheResult ExecuteHippopWithType(const UObject* WorldContextObject, FName Collection, const FString& Key, T& OutValue)
{
	return UHippocacheBlueprintLibrary::Hippop(WorldContextObject, Collection, Key, OutValue);
}

// Helper function to execute Hippop based on property type using template dispatch
FHippocacheResult UHippocacheBlueprintLibrary::ExecuteHippopForProperty(const UObject* WorldContextObject, FName Collection, const FString& Key, FProperty* ValueProperty, void* ValuePtr)
{
	if (!ValueProperty || !ValuePtr)
	{
		return FHippocacheResult::Error(EHippocacheErrorCode::InvalidValue, TEXT("Invalid property or value pointer"), TEXT("ExecuteHippopForProperty"));
	}
	
	// Check if it's a struct property - handle with FInstancedStruct
	if (FStructProperty* StructProp = CastField<FStructProperty>(ValueProperty))
	{
		UScriptStruct* StructType = StructProp->Struct;
		if (!StructType)
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::UnsupportedType, TEXT("Invalid struct type"), TEXT("ExecuteHippopForProperty"));
		}
		
		// Convert all structs to FInstancedStruct and let the template Hippop handle it
		FInstancedStruct InstancedStruct;
		FHippocacheResult Result = ExecuteHippopWithType(WorldContextObject, Collection, Key, InstancedStruct);
		if (Result.IsSuccess() && InstancedStruct.IsValid() && InstancedStruct.GetScriptStruct() == StructType)
		{
			// Copy the struct data back to the original memory location
			StructType->CopyScriptStruct(ValuePtr, InstancedStruct.GetMemory());
		}
		else if (Result.IsSuccess())
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, 
				TEXT("Retrieved struct type does not match expected type"), 
				FString::Printf(TEXT("Expected: %s"), *StructType->GetName()));
		}
		return Result;
	}
	else
	{
		// All non-struct types: retrieve FVariant wrapper via FInstancedStruct
		UHippocacheSubsystem* Subsystem = nullptr;
		FHippocacheResult Result = GetSubsystemSafe(WorldContextObject, Subsystem);
		if (Result.IsError())
		{
			return Result;
		}

		// Retrieve FVariantWrapper from storage
		FInstancedStruct InstancedStruct;
		Result = Subsystem->GetStruct(Collection, Key, InstancedStruct);
		if (Result.IsError())
		{
			return Result;
		}

		// Extract FVariantWrapper and convert back to property
		if (InstancedStruct.IsValid() && InstancedStruct.GetScriptStruct() == FVariantWrapper::StaticStruct())
		{
			const FVariantWrapper* Wrapper = InstancedStruct.GetPtr<FVariantWrapper>();
			if (!Wrapper)
			{
				return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, 
					TEXT("Retrieved InstancedStruct does not contain valid FVariantWrapper"), 
					TEXT("ExecuteHippopForProperty"));
			}

			// Convert variant back to property
			FVariant Variant = Wrapper->GetVariant();
			if (!VariantToProperty(Variant, ValueProperty, ValuePtr))
			{
				return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, 
					FString::Printf(TEXT("Cannot convert FVariant back to property type: %s"), *ValueProperty->GetClass()->GetName()), 
					TEXT("ExecuteHippopForProperty"));
			}

			return FHippocacheResult::Success();
		}
		else
		{
			return FHippocacheResult::Error(EHippocacheErrorCode::TypeMismatch, 
				TEXT("Retrieved data is not a FVariantWrapper"), 
				TEXT("ExecuteHippopForProperty"));
		}
	}
}