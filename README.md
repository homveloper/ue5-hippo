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
  <b>⚡ Lightning-fast, thread-safe caching system for Unreal Engine with full Blueprint support</b>
</p>

## ✨ Features

- 🚀 **High Performance**: Read-write lock separation achieving 660K+ ops/sec
- 🔒 **Thread-Safe**: Built with FRWLock for optimal concurrent access
- 🎨 **Blueprint First**: Complete Blueprint node library for all operations
- 🎯 **Type Safety**: Support for all UE types - primitives, structs, vectors, colors, etc.
- ⏰ **Auto Expiration**: TTL-based cache with automatic cleanup
- 📦 **Collections**: Organize cache data by collection names

## 🏗️ Architecture

Hippocache is designed with a clean separation of concerns:

- **🌐 Game Instance Subsystem**: Core storage engine with thread-safe operations
- **🎨 Blueprint Library**: User-friendly API for Blueprint and C++
- **📦 Collections**: Organize cached data by logical groups

The subsystem provides low-level struct storage while the Blueprint Library offers convenient typed access for all Unreal Engine types.

## 🎮 Blueprint Usage

### 📥 Setting Values

Hippocache provides specific nodes for each data type:

<table>
<tr>
<th>Node</th>
<th>Description</th>
<th>Example</th>
</tr>
<tr>
<td><b>Set Int32</b></td>
<td>Cache integer values</td>
<td>

```
Collection: "Player"
Key: "Score"
Value: 100
TTL: 300.0
```

</td>
</tr>
<tr>
<td><b>Set String</b></td>
<td>Cache text values</td>
<td>

```
Collection: "Player"
Key: "Name"
Value: "John"
TTL: 600.0
```

</td>
</tr>
<tr>
<td><b>Set Vector</b></td>
<td>Cache 3D positions</td>
<td>

```
Collection: "Player"
Key: "Position"
Value: (X:100, Y:200, Z:50)
TTL: 60.0
```

</td>
</tr>
<tr>
<td><b>Set Transform</b></td>
<td>Cache full transforms</td>
<td>

```
Collection: "Checkpoint"
Key: "Spawn"
Value: Transform
TTL: 0.0 (no expiry)
```

</td>
</tr>
</table>

### 📤 Getting Values

Retrieve cached values with type-specific nodes:

<table>
<tr>
<th>Node</th>
<th>Outputs</th>
<th>Usage</th>
</tr>
<tr>
<td><b>Get Int32</b></td>
<td>
• Result (Success/Fail)<br>
• Value (Integer)
</td>
<td>

```
Collection: "Player"
Key: "Score"
→ Branch on Result
  → Use Value
```

</td>
</tr>
<tr>
<td><b>Get Vector</b></td>
<td>
• Result (Success/Fail)<br>
• Value (Vector)
</td>
<td>

```
Collection: "Player"  
Key: "Position"
→ Set Actor Location
```

</td>
</tr>
</table>

### 🎯 Blueprint Node Reference

#### 📊 Primitive Types
- `Set/Get Int32` - 32-bit integers
- `Set/Get Int64` - 64-bit integers  
- `Set/Get Float` - Single precision
- `Set/Get Double` - Double precision
- `Set/Get Bool` - Boolean values
- `Set/Get Byte` - Byte values

#### 📝 Text Types
- `Set/Get String` - Text strings
- `Set/Get Name` - FName values
- `Set/Get Text` - Localized text

#### 📐 Math Types
- `Set/Get Vector` - 3D vectors
- `Set/Get Vector2D` - 2D vectors
- `Set/Get Vector4` - 4D vectors
- `Set/Get Rotator` - Rotations
- `Set/Get Transform` - Full transforms

#### 🎨 Visual Types
- `Set/Get Color` - RGBA colors
- `Set/Get LinearColor` - Linear colors

#### 📅 Time Types
- `Set/Get DateTime` - Date and time

#### 🏗️ Custom Types
- `Set/Get Struct` - Any USTRUCT

### 📦 Collections

Use collections to organize your cached data:

```
🎮 Game State
├─ "Player" → Score, Health, Name
├─ "UI" → Settings, Preferences
└─ "Session" → ID, StartTime, Players

🌍 World State
├─ "Enemies" → Positions, States
├─ "Items" → Locations, Types
└─ "Checkpoints" → Transforms
```

## 💻 C++ Usage

### 🔧 Basic Setup

```cpp
#include "HippocacheBlueprintLibrary.h"

// All operations go through the Blueprint Library
using Cache = UHippocacheBlueprintLibrary;
```

### 📥 Setting Values

```cpp
// Set with TTL (Time To Live)
FHippocacheResult Result = Cache::SetInt32WithTTL(
    WorldContext, 
    "Player",      // Collection
    "Score",       // Key
    1000,          // Value
    300.0f         // TTL in seconds
);

if (Result)  // Uses bool operator
{
    // Success!
}

// Set without TTL (permanent until removed)
Cache::SetString(WorldContext, "Player", "Name", "Alice");
Cache::SetVector(WorldContext, "Player", "Position", FVector(100, 200, 0));
Cache::SetBool(WorldContext, "Game", "IsPaused", false);
```

### 📤 Getting Values

```cpp
// Get with result checking
int32 Score;
if (Cache::GetInt32(WorldContext, "Player", "Score", Score))
{
    UE_LOG(LogTemp, Log, TEXT("Player Score: %d"), Score);
}

// Get multiple values
FString PlayerName;
FVector Position;
bool bIsPaused;

if (Cache::GetString(WorldContext, "Player", "Name", PlayerName) &&
    Cache::GetVector(WorldContext, "Player", "Position", Position) &&
    Cache::GetBool(WorldContext, "Game", "IsPaused", bIsPaused))
{
    // All values retrieved successfully
}
```

### 🏗️ Struct Support

```cpp
// Define your struct
USTRUCT(BlueprintType)
struct FGameSession
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString SessionId;
    
    UPROPERTY()
    int32 PlayerCount;
    
    UPROPERTY()
    FDateTime StartTime;
    
    UPROPERTY()
    TArray<FString> PlayerNames;
};

// Set struct
FGameSession Session;
Session.SessionId = "GAME-12345";
Session.PlayerCount = 4;
Session.StartTime = FDateTime::Now();
Session.PlayerNames = {"Alice", "Bob", "Charlie", "Diana"};

Cache::SetStructWithTTL(
    WorldContext,
    "Game",                          // Collection
    "CurrentSession",                // Key
    FGameSession::StaticStruct(),    // Struct type
    &Session,                        // Data
    3600.0f                         // 1 hour TTL
);

// Get struct
FGameSession RetrievedSession;
if (Cache::GetStruct(
    WorldContext,
    "Game",
    "CurrentSession",
    FGameSession::StaticStruct(),
    &RetrievedSession))
{
    // Use retrieved session data
}
```

### 🔨 Utility Operations

```cpp
// Check if key exists
bool bExists;
if (Cache::HasKey(WorldContext, "Player", "Score", bExists) && bExists)
{
    // Key exists in cache
}

// Remove specific key
Cache::Remove(WorldContext, "Player", "Score");

// Clear entire collection
Cache::Clear(WorldContext, "Player");

// Get collection size
int32 ItemCount;
if (Cache::Num(WorldContext, "Player", ItemCount))
{
    UE_LOG(LogTemp, Log, TEXT("Player collection has %d items"), ItemCount);
}
```

## 📊 Performance Benchmarks

<table align="center">
<tr>
<th>Operation</th>
<th>Ops/Second</th>
<th>Response Time</th>
</tr>
<tr>
<td>🔵 Set Operations</td>
<td><b>290K - 395K</b></td>
<td>2-3μs</td>
</tr>
<tr>
<td>🟢 Get Operations</td>
<td><b>660K - 671K</b></td>
<td>1μs</td>
</tr>
</table>

## ⏰ TTL (Time To Live)

Control how long data stays in cache:

| TTL Value | Behavior | Use Case |
|-----------|----------|----------|
| `0.0` | Never expires | Permanent game settings |
| `30.0` | 30 seconds | Temporary UI state |
| `300.0` | 5 minutes | Player session data |
| `3600.0` | 1 hour | Level cache |

## 💡 Best Practices

### 🏷️ Collection Naming

```cpp
✅ Good:
"Player.Stats"     // Hierarchical
"UI.Settings"      // Clear purpose
"Session.Data"     // Organized

❌ Avoid:
"data"            // Too generic
"temp123"         // Unclear
""                // Empty
```

### 🔑 Key Naming

```cpp
✅ Good:
"PlayerScore"      // Clear and concise
"LastCheckpoint"   // Descriptive
"AudioVolume"      // Specific

❌ Avoid:
"x"               // Too short
"var_1"           // Non-descriptive
```

### 💾 Memory Management

1. **Set appropriate TTLs** - Don't cache forever unless needed
2. **Clear collections** - Clean up when done
3. **Monitor size** - Use `Num()` to track items
4. **Batch operations** - Group related cache operations

## 🎯 Common Use Cases

### 🎮 Game Session Management

```cpp
// Blueprint: Begin Play
Set String: Collection="Session", Key="ID", Value=GenerateSessionID()
Set DateTime: Collection="Session", Key="StartTime", Value=Now()
Set Int32: Collection="Session", Key="PlayerCount", Value=0

// Blueprint: Player Joined
Get Int32: Collection="Session", Key="PlayerCount" → Count
Set Int32: Collection="Session", Key="PlayerCount", Value=Count+1
```

### 🎨 UI State Persistence

```cpp
// Blueprint: Save UI Settings
Set Float: Collection="UI", Key="MasterVolume", Value=Slider.Value
Set Bool: Collection="UI", Key="ShowHints", Value=Checkbox.Checked
Set LinearColor: Collection="UI", Key="CrosshairColor", Value=ColorPicker.Color

// Blueprint: Load UI Settings
Get Float: Collection="UI", Key="MasterVolume" → Set Slider
Get Bool: Collection="UI", Key="ShowHints" → Set Checkbox
Get LinearColor: Collection="UI", Key="CrosshairColor" → Set Color
```

### 🌍 World State Caching

```cpp
// Blueprint: Save Checkpoint
Set Transform: Collection="Checkpoint", Key="Player", Value=GetActorTransform()
Set Float: Collection="Checkpoint", Key="Health", Value=CurrentHealth
Set Int32: Collection="Checkpoint", Key="Ammo", Value=CurrentAmmo

// Blueprint: Load Checkpoint
Get Transform: Collection="Checkpoint", Key="Player" → SetActorTransform
Get Float: Collection="Checkpoint", Key="Health" → Set Health
Get Int32: Collection="Checkpoint", Key="Ammo" → Set Ammo
```

## 🔧 Troubleshooting

### ❗ Common Issues

| Issue | Solution |
|-------|----------|
| 🔍 **Result = False** | Check collection/key names and TTL expiration |
| 🔀 **Wrong value returned** | Ensure using matching Get/Set node types |
| 💾 **Memory growth** | Set appropriate TTLs, clear old collections |
| 🐛 **Crash on struct** | Verify struct has USTRUCT() macro |

## 🤝 Contributing

1. 🍴 Fork the repository
2. 🌿 Create a feature branch
3. 💬 Commit your changes
4. 📤 Push to the branch
5. 🎯 Create a Pull Request

## 📄 License

This plugin is provided under the MIT License. See [LICENSE](LICENSE) file for details.

## 📈 Version History

### 🎉 v1.0.0 (Current)
- ✨ Full Blueprint node library for all UE types
- 🔒 Thread-safe operations with FRWLock
- 📊 High-performance caching (660K+ reads/sec)
- 📦 Collection-based organization
- ⏰ Automatic TTL expiration

---

<p align="center">
  Made with ❤️ for Unreal Engine developers
</p>

<p align="center">
  <a href="https://github.com/homveloper/ue5-hippo/issues">🐛 Report Bug</a>
  ·
  <a href="https://github.com/homveloper/ue5-hippo/issues">✨ Request Feature</a>
</p>