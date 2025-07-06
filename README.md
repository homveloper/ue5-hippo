# 🦛 Hippocache Plugin

<p align="center">
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.0+-313131?style=for-the-badge&logo=unreal-engine&logoColor=white" alt="Unreal Engine">
  <img src="https://img.shields.io/badge/Blueprint-Supported-2196F3?style=for-the-badge&logo=blueprint&logoColor=white" alt="Blueprint">
  <img src="https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Mac%20%7C%20Linux-lightgrey?style=for-the-badge" alt="Platform">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Performance-High-brightgreen?style=flat-square" alt="Performance">
  <img src="https://img.shields.io/badge/Thread%20Safe-Yes-success?style=flat-square" alt="Thread Safe">
  <img src="https://img.shields.io/badge/License-MIT-blue?style=flat-square" alt="License">
  <img src="https://img.shields.io/github/stars/homveloper/ue5-hippo?style=flat-square" alt="Stars">
</p>

<p align="center">
  <b>⚡ Lightning-fast, thread-safe caching system for Unreal Engine with Blueprint support</b>
</p>

## ✨ Features

- 🚀 **High Performance**: Optimized read-write lock separation achieving 660K+ ops/sec
- 🔒 **Thread-Safe**: Built with FRWLock for concurrent read access
- 🎨 **Blueprint Support**: Full integration with wildcard nodes (Hippoo/Hippop)
- 📦 **Type Safety**: Support for all UE primitive types and USTRUCT
- ⏰ **Auto Expiration**: TTL-based cache expiration with configurable timeouts
- 🧹 **Memory Efficient**: Automatic cleanup of expired entries

## 📊 Performance Benchmarks

<table align="center">
<tr>
<th>Operation</th>
<th>Ops/Second</th>
<th>Min Time</th>
<th>Avg Time</th>
</tr>
<tr>
<td>🔵 Set Int32</td>
<td><b>395,256</b></td>
<td>2μs</td>
<td>2μs</td>
</tr>
<tr>
<td>🟢 Set String</td>
<td><b>298,507</b></td>
<td>3μs</td>
<td>3μs</td>
</tr>
<tr>
<td>🟣 Set Struct</td>
<td><b>290,697</b></td>
<td>3μs</td>
<td>3μs</td>
</tr>
<tr>
<td>🔴 Get Int32</td>
<td><b>661,812</b></td>
<td>1μs</td>
<td>1μs</td>
</tr>
<tr>
<td>🟡 Get String</td>
<td><b>661,812</b></td>
<td>1μs</td>
<td>1μs</td>
</tr>
<tr>
<td>🟠 Get Struct</td>
<td><b>671,140</b></td>
<td>1μs</td>
<td>1μs</td>
</tr>
</table>

## 🛠️ Installation

1. 📁 Copy the `hippocache` folder to your project's `Plugins` directory
2. 🔄 Regenerate project files
3. ✅ Enable the plugin in your project settings or .uproject file

```json
{
    "Plugins": [
        {
            "Name": "hippocache",
            "Enabled": true
        }
    ]
}
```

## 🎮 Usage

### 🎨 Blueprint Usage

#### 📥 Setting Values (Hippoo Node)

<img src="https://via.placeholder.com/600x200/2196F3/FFFFFF?text=Hippoo+Node+Example" alt="Hippoo Node">

The **Hippoo** node allows you to cache any type of value with automatic type detection:

```
[🦛 Hippoo]
├─ Key: "PlayerScore"
├─ Value: 100
├─ TTL: 300.0 (5 minutes)
└─ Result: ✅ Success/❌ Failure
```

#### 📤 Getting Values (Hippop Node)

<img src="https://via.placeholder.com/600x200/4CAF50/FFFFFF?text=Hippop+Node+Example" alt="Hippop Node">

The **Hippop** node retrieves cached values with type safety:

```
[🦛 Hippop]
├─ Key: "PlayerScore"
├─ Value: 📊 (Retrieved value)
└─ Result: ✅ Success/❌ Failure
```

### 💻 C++ Usage

#### 🔧 Basic Operations

```cpp
// 🎯 Get the subsystem
UHippocacheSubsystem* Cache = GetGameInstance()->GetSubsystem<UHippocacheSubsystem>();

// 💾 Set values
bool bSuccess = Cache->SetInt32("Score", 100, 300.0f); // 5 minute TTL
Cache->SetString("PlayerName", "John", 600.0f);
Cache->SetBool("IsActive", true, 60.0f);

// 📖 Get values
int32 Score;
if (Cache->GetInt32("Score", Score))
{
    // Use the score 🎮
}

// ❓ Check existence
if (Cache->HasKey("PlayerName"))
{
    // Key exists ✅
}

// 🗑️ Remove specific key
Cache->Remove("Score");

// 🧹 Clear all cache
Cache->Clear();
```

#### 📦 Struct Support

```cpp
// 🏗️ Define your struct
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

// 📥 Set struct
FPlayerData PlayerData;
PlayerData.Name = "John";
PlayerData.Score = 100;
PlayerData.PlayTime = 120.5f;

Cache->SetStruct("Player1", FPlayerData::StaticStruct(), &PlayerData, 300.0f);

// 📤 Get struct
FPlayerData RetrievedData;
if (Cache->GetStruct("Player1", FPlayerData::StaticStruct(), &RetrievedData))
{
    // Use retrieved data 🎯
}
```

### 🎯 Supported Types

| Type Category | Supported Types |
|--------------|-----------------|
| 🔢 **Primitive** | bool, uint8, int32, int64, float, double |
| 📝 **String** | FString, FName, FText |
| 📦 **Container** | TArray, TMap, TSet |
| 🏗️ **Struct** | Any USTRUCT (including Blueprint structs) |
| 🎭 **Object** | UObject* (soft references recommended) |

## 🏗️ Architecture

### 🌐 Game Instance Subsystem

Hippocache is implemented as a Game Instance Subsystem, providing:
- ♻️ Automatic lifecycle management
- 🌍 Global accessibility
- 💾 Persistence across level transitions

### 🔒 Thread Safety

The plugin uses Unreal Engine's `FRWLock` for optimal performance:
- 👥 Multiple concurrent read operations
- 🔐 Exclusive write operations
- ⚡ Minimal lock contention

### 💾 Memory Management

- 🔄 Automatic cleanup of expired entries
- 📊 Efficient memory usage with FVariant storage
- 🛡️ No memory leaks with proper RAII patterns

## 📚 API Reference

### 🎨 Blueprint Functions

<details>
<summary>Click to expand API reference</summary>

| Function | Description | Parameters |
|----------|-------------|------------|
| `SetInt32` | Cache an integer value | Key, Value, TTL |
| `GetInt32` | Retrieve an integer value | Key, Out Value |
| `SetString` | Cache a string value | Key, Value, TTL |
| `GetString` | Retrieve a string value | Key, Out Value |
| `SetBool` | Cache a boolean value | Key, Value, TTL |
| `GetBool` | Retrieve a boolean value | Key, Out Value |
| `SetFloat` | Cache a float value | Key, Value, TTL |
| `GetFloat` | Retrieve a float value | Key, Out Value |
| `SetStruct` | Cache a struct value | Key, Struct, Value, TTL |
| `GetStruct` | Retrieve a struct value | Key, Struct, Out Value |
| `HasKey` | Check if a key exists | Key |
| `Remove` | Remove a specific key | Key |
| `Clear` | Clear all cached values | - |

</details>

### ⏰ TTL (Time To Live)

- ⏱️ **Default TTL**: 300 seconds (5 minutes)
- ♾️ **No expiration**: Set to 0
- 🚫 **Expired entries**: Not returned by Get operations
- 🧹 **Cleanup**: Happens during write operations

## 💡 Best Practices

1. 🏷️ **Key Naming**: Use descriptive, hierarchical keys
   ```
   ✅ "Player.Stats.Score"
   ✅ "UI.Settings.Volume"
   ❌ "data1"
   ```

2. ⏰ **TTL Values**: Set appropriate TTL based on data volatility
   - 🏃 Short-lived data: 30-60 seconds
   - 📊 Session data: 5-30 minutes
   - 💾 Persistent data: 1+ hours

3. 📦 **Struct Usage**: Prefer small, simple structs for better performance

4. 🔒 **Thread Safety**: No need for external synchronization

5. 💾 **Memory**: Monitor cache size for memory-sensitive applications

## 📖 Examples

### 🎮 Player Statistics Cache

```cpp
// 📊 Cache player stats for 10 minutes
Cache->SetInt32("Player.Score", PlayerScore, 600.0f);
Cache->SetFloat("Player.Health", PlayerHealth, 600.0f);
Cache->SetString("Player.Name", PlayerName, 600.0f);

// 📖 Retrieve all stats
int32 Score;
float Health;
FString Name;

if (Cache->GetInt32("Player.Score", Score) &&
    Cache->GetFloat("Player.Health", Health) &&
    Cache->GetString("Player.Name", Name))
{
    // All stats retrieved successfully ✅
}
```

### 🌐 Session Data Cache

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

// 💾 Cache session data for 1 hour
FSessionData Session;
// ... populate session data
Cache->SetStruct("CurrentSession", FSessionData::StaticStruct(), &Session, 3600.0f);
```

## 🔧 Troubleshooting

### ❗ Common Issues

| Issue | Solution |
|-------|----------|
| 🔍 **Key Not Found** | Check TTL hasn't expired |
| 🔀 **Type Mismatch** | Use same type for Set/Get |
| 📦 **Struct Issues** | Ensure USTRUCT() decoration |

### 🚀 Performance Tips

1. 📦 Batch related operations when possible
2. ⏱️ Use appropriate TTL values
3. 📏 Consider struct size for complex data
4. 📊 Profile your specific use case

## 🤝 Contributing

Contributions are welcome! Please:

1. 🍴 Fork the repository
2. 🌿 Create a feature branch
3. 💬 Commit your changes
4. 📤 Push to the branch
5. 🎯 Create a Pull Request

## 📄 License

This plugin is provided under the MIT License. See [LICENSE](LICENSE) file for details.

## 📈 Version History

### 🎉 v1.0.0 (Current)
- ✨ Initial release with full Blueprint support
- 🔒 Thread-safe operations with FRWLock
- 📊 High-performance caching system
- 🎨 Wildcard Blueprint nodes (Hippoo/Hippop)
- 📦 Support for all UE types including USTRUCT

---

<p align="center">
  Made with ❤️ for Unreal Engine developers
</p>

<p align="center">
  <a href="https://github.com/homveloper/ue5-hippo/issues">🐛 Report Bug</a>
  ·
  <a href="https://github.com/homveloper/ue5-hippo/issues">✨ Request Feature</a>
</p>