# Hippocache Plugin

A high-performance, thread-safe caching system for Unreal Engine with Blueprint support and automatic expiration handling.

## Features

- **Thread-Safe Caching**: Read-write lock separation for optimal concurrent access
- **Automatic Expiration**: Time-based cache expiration with configurable TTL
- **Blueprint Support**: Full Blueprint integration with wildcard nodes
- **Type Safety**: Support for all Unreal Engine primitive types and structs
- **High Performance**: Optimized for both read and write operations
- **Memory Efficient**: Automatic cleanup of expired entries

## Performance

Benchmark results on typical hardware:

| Operation | Ops/Second | Min Time | Avg Time | Max Time |
|-----------|------------|----------|----------|----------|
| Set Int32 | 395,256 | 2μs | 2μs | 67μs |
| Set String | 298,507 | 3μs | 3μs | 42μs |
| Set Struct | 290,697 | 3μs | 3μs | 32μs |
| Get Int32 | 661,812 | 1μs | 1μs | 40μs |
| Get String | 661,812 | 1μs | 1μs | 49μs |
| Get Struct | 671,140 | 1μs | 1μs | 41μs |

## Installation

1. Copy the `hippocache` folder to your project's `Plugins` directory
2. Regenerate project files
3. Enable the plugin in your project settings or .uproject file

## Usage

### Blueprint Usage

#### Setting Values (Hippoo Node)

The Hippoo node allows you to cache any type of value with automatic type detection:

```
[Hippoo]
Key: "PlayerScore"
Value: 100
TTL: 300.0 (5 minutes)
Result: Success/Failure
```

#### Getting Values (Hippop Node)

The Hippop node retrieves cached values with type safety:

```
[Hippop]
Key: "PlayerScore"
Value: (Retrieved value)
Result: Success/Failure
```

### C++ Usage

#### Basic Operations

```cpp
// Get the subsystem
UHippocacheSubsystem* Cache = GetGameInstance()->GetSubsystem<UHippocacheSubsystem>();

// Set values
bool bSuccess = Cache->SetInt32("Score", 100, 300.0f); // 5 minute TTL
Cache->SetString("PlayerName", "John", 600.0f);
Cache->SetBool("IsActive", true, 60.0f);

// Get values
int32 Score;
if (Cache->GetInt32("Score", Score))
{
    // Use the score
}

// Check existence
if (Cache->HasKey("PlayerName"))
{
    // Key exists
}

// Remove specific key
Cache->Remove("Score");

// Clear all cache
Cache->Clear();
```

#### Struct Support

```cpp
// Define your struct
USTRUCT(BlueprintType)
struct FPlayerData
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString Name;
    
    UPROPERTY()
    int32 Score;
    
    UPROPERTY()
    float PlayTime;
};

// Set struct
FPlayerData PlayerData;
PlayerData.Name = "John";
PlayerData.Score = 100;
PlayerData.PlayTime = 120.5f;

Cache->SetStruct("Player1", FPlayerData::StaticStruct(), &PlayerData, 300.0f);

// Get struct
FPlayerData RetrievedData;
if (Cache->GetStruct("Player1", FPlayerData::StaticStruct(), &RetrievedData))
{
    // Use retrieved data
}
```

### Supported Types

- **Primitive Types**: bool, uint8, int32, int64, float, double
- **String Types**: FString, FName, FText
- **Container Types**: TArray, TMap, TSet
- **Struct Types**: Any USTRUCT (including Blueprint structs)
- **Object References**: UObject* (soft references recommended)

## Architecture

### Subsystem

Hippocache is implemented as a Game Instance Subsystem, providing:
- Automatic lifecycle management
- Global accessibility
- Persistence across level transitions

### Thread Safety

The plugin uses Unreal Engine's FRWLock for optimal performance:
- Multiple concurrent read operations
- Exclusive write operations
- Minimal lock contention

### Memory Management

- Automatic cleanup of expired entries
- Efficient memory usage with FVariant storage
- No memory leaks with proper RAII patterns

## API Reference

### Blueprint Functions

| Function | Description |
|----------|-------------|
| `SetInt32` | Cache an integer value |
| `GetInt32` | Retrieve an integer value |
| `SetString` | Cache a string value |
| `GetString` | Retrieve a string value |
| `SetBool` | Cache a boolean value |
| `GetBool` | Retrieve a boolean value |
| `SetFloat` | Cache a float value |
| `GetFloat` | Retrieve a float value |
| `SetStruct` | Cache a struct value |
| `GetStruct` | Retrieve a struct value |
| `HasKey` | Check if a key exists |
| `Remove` | Remove a specific key |
| `Clear` | Clear all cached values |

### TTL (Time To Live)

- Default TTL: 300 seconds (5 minutes)
- Set to 0 for no expiration
- Expired entries are not returned by Get operations
- Cleanup happens during write operations

## Best Practices

1. **Key Naming**: Use descriptive, hierarchical keys (e.g., "Player.Stats.Score")
2. **TTL Values**: Set appropriate TTL based on data volatility
3. **Struct Usage**: Prefer small, simple structs for better performance
4. **Thread Safety**: No need for external synchronization
5. **Memory**: Monitor cache size for memory-sensitive applications

## Examples

### Player Statistics Cache

```cpp
// Cache player stats for 10 minutes
Cache->SetInt32("Player.Score", PlayerScore, 600.0f);
Cache->SetFloat("Player.Health", PlayerHealth, 600.0f);
Cache->SetString("Player.Name", PlayerName, 600.0f);

// Retrieve all stats
int32 Score;
float Health;
FString Name;

if (Cache->GetInt32("Player.Score", Score) &&
    Cache->GetFloat("Player.Health", Health) &&
    Cache->GetString("Player.Name", Name))
{
    // All stats retrieved successfully
}
```

### Session Data Cache

```cpp
USTRUCT()
struct FSessionData
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString SessionId;
    
    UPROPERTY()
    FDateTime StartTime;
    
    UPROPERTY()
    TArray<FString> ConnectedPlayers;
};

// Cache session data for 1 hour
FSessionData Session;
// ... populate session data
Cache->SetStruct("CurrentSession", FSessionData::StaticStruct(), &Session, 3600.0f);
```

## Troubleshooting

### Common Issues

1. **Key Not Found**: Ensure the key hasn't expired and was set correctly
2. **Type Mismatch**: Use the same type for Set and Get operations
3. **Struct Issues**: Ensure structs have proper USTRUCT() decoration

### Performance Tips

1. Batch related operations when possible
2. Use appropriate TTL values to prevent memory bloat
3. Consider struct size when caching complex data
4. Profile your specific use case for optimal settings

## License

This plugin is provided as-is for use in Unreal Engine projects. See LICENSE file for details.

## Contributing

Contributions are welcome! Please submit pull requests or issues on the project repository.

## Version History

- **1.0.0**: Initial release with full Blueprint support and thread-safe operations