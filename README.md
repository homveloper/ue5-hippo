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
  <b>⚡ Lightning-fast, thread-safe caching with magical wildcard nodes for Unreal Engine</b>
</p>

## 🌟 Introducing Hippoo & Hippop - Universal Cache Nodes

The crown jewels of Hippocache are the **Hippoo** (setter) and **Hippop** (getter) nodes - revolutionary wildcard Blueprint nodes that automatically detect and handle ANY data type!

### 🎯 Why Hippoo & Hippop?

<table>
<tr>
<td width="50%">

#### ❌ Traditional Approach
```
Need different nodes for each type:
- Set Int32
- Set String  
- Set Vector
- Set Transform
- Set Custom Struct
- ... and 20+ more nodes!
```

</td>
<td width="50%">

#### ✅ Hippocache Approach
```
Just TWO nodes for everything:
- Hippoo (stores ANY value)
- Hippop (retrieves ANY value)

That's it! 🎉
```

</td>
</tr>
</table>

### 🦛 Universal Cache Operations

![Hippocache Blueprint Example](https://blueprintue.com/render/hyijwgji/)

#### 🎯 Hippoo - Store ANY Value

The **Hippoo** node is a wildcard setter that magically accepts ANY data type:

```
[🦛 Hippoo]
├─ Collection: "Game"
├─ Key: "PlayerData"  
├─ Value: ⚡ (Connect ANYTHING here!)
├─ TTL: 300.0
└─ Result: ✅/❌
```

**Examples:**
- `Hippoo → Value: 100` (Integer)
- `Hippoo → Value: "Hello"` (String)
- `Hippoo → Value: (100,200,300)` (Vector)
- `Hippoo → Value: PlayerTransform` (Transform)
- `Hippoo → Value: SaveGameStruct` (Custom Struct)
- `Hippoo → Value: [Literally Any Type!]` 

#### 🎣 Hippop - Retrieve ANY Value

The **Hippop** node retrieves values with automatic type detection:

```
[🦛 Hippop]
├─ Collection: "Game"
├─ Key: "PlayerData"
├─ Value: ⚡ (Outputs stored type)
└─ Result: ✅/❌
```

**Type-Safe Magic:**
- Connect to Integer Variable → Gets Integer
- Connect to Vector → Gets Vector  
- Connect to Custom Struct → Gets that Struct
- Type mismatches are prevented at compile time!

### 💡 Real Blueprint Examples

#### 🎮 Complete Save/Load System

<p align="center">
  <img src="https://via.placeholder.com/1000x400/4CAF50/FFFFFF?text=Save+Game+Blueprint+Example" alt="Save Game Example">
</p>

```blueprint
=== Save Game Flow ===
[Player Presses Save]
    ├─ [Hippoo] Collection: "Save", Key: "PlayerTransform", Value: GetActorTransform()
    ├─ [Hippoo] Collection: "Save", Key: "PlayerHealth", Value: CurrentHealth
    ├─ [Hippoo] Collection: "Save", Key: "PlayerInventory", Value: InventoryArray
    ├─ [Hippoo] Collection: "Save", Key: "GameTime", Value: GetGameTimeInSeconds()
    └─ [Show Toast] "Game Saved!"

=== Load Game Flow ===
[Player Presses Load]
    ├─ [Hippop] Collection: "Save", Key: "PlayerTransform" → SetActorTransform
    ├─ [Hippop] Collection: "Save", Key: "PlayerHealth" → SetHealth
    ├─ [Hippop] Collection: "Save", Key: "PlayerInventory" → RestoreInventory
    ├─ [Hippop] Collection: "Save", Key: "GameTime" → SetGameTime
    └─ [Branch] Result → Success: "Game Loaded!" / Fail: "No Save Found!"
```

#### 🎨 Dynamic UI State Management

<p align="center">
  <img src="https://via.placeholder.com/1000x300/FF9800/FFFFFF?text=UI+State+Blueprint+Example" alt="UI State Example">
</p>

```blueprint
=== Store UI State ===
[Tab Changed Event]
    ├─ [Hippoo] "UI", "ActiveTab", TabWidget.ActiveIndex
    ├─ [Hippoo] "UI", "ScrollPosition", ScrollBox.ScrollOffset
    └─ [Hippoo] "UI", "FilterSettings", CurrentFilterStruct

=== Restore UI State ===
[UI Opened Event]
    ├─ [Hippop] "UI", "ActiveTab" → TabWidget.SetActiveTab
    ├─ [Hippop] "UI", "ScrollPosition" → ScrollBox.SetScrollOffset
    └─ [Hippop] "UI", "FilterSettings" → ApplyFilters
```

## ✨ Features

- 🎯 **Universal Nodes**: Just Hippoo & Hippop for ANY data type
- 🚀 **Blazing Fast**: 660K+ ops/sec with read-write lock separation
- 🔒 **Thread-Safe**: Built with FRWLock for concurrent access
- 🎨 **Blueprint First**: Designed for Blueprint workflows
- ⏰ **Auto Expiration**: TTL-based automatic cleanup
- 📦 **Collections**: Organize data by logical groups

## 🏗️ Architecture

Hippocache uses a clean, efficient architecture:

- **🌐 Game Instance Subsystem**: Core storage engine
- **🦛 Hippoo/Hippop Nodes**: Universal wildcard interface
- **📦 Collections**: Namespace-like organization
- **🔒 Thread Safety**: FRWLock for optimal performance

## 🎮 Blueprint Usage Guide

### 🚀 Quick Start

1. **Store Data** - Use Hippoo with any value type
2. **Retrieve Data** - Use Hippop with matching variable type
3. **Organize** - Use collections to group related data
4. **Set TTL** - Control data lifetime (0 = permanent)

### 📦 Collection Patterns

```
Game Systems:
├─ "Player" → Stats, Position, Inventory
├─ "Enemy" → Spawns, States, Targets
├─ "World" → Time, Weather, Events
└─ "Save" → Complete game state

UI Systems:
├─ "HUD" → Visibility, Values, States
├─ "Menu" → Selected options, History
├─ "Dialog" → Choices, Progress
└─ "Settings" → All preferences
```

### 🎯 Common Patterns

#### Temporary Data with TTL
```blueprint
[Hippoo] "Combat", "DamageBoost", 1.5, TTL: 30.0
// Automatically expires after 30 seconds
```

#### Permanent Settings
```blueprint
[Hippoo] "Settings", "MasterVolume", 0.8, TTL: 0.0
// Never expires until manually removed
```

#### Conditional Caching
```blueprint
[Branch] Is Valid Save Data?
    True → [Hippoo] "Save", "Checkpoint", CheckpointStruct
    False → [Print] "Invalid save data"
```

## 💻 C++ Usage

### 🦛 Using Hippoo/Hippop Pattern in C++

```cpp
#include "HippocacheBlueprintLibrary.h"

// Store ANY type with one pattern
template<typename T>
void CacheValue(const FString& Key, const T& Value)
{
    UHippocacheBlueprintLibrary::SetValue(
        this,           // World context
        "MyCollection", // Collection name
        Key,            // Cache key
        Value,          // Any value type!
        300.0f          // TTL in seconds
    );
}

// Retrieve ANY type with one pattern
template<typename T>
bool GetCachedValue(const FString& Key, T& OutValue)
{
    return UHippocacheBlueprintLibrary::GetValue(
        this,
        "MyCollection",
        Key,
        OutValue
    );
}

// Example usage
void MyGameMode::SaveGameState()
{
    // Store different types with same function
    CacheValue("PlayerScore", 1000);
    CacheValue("PlayerName", FString("Hero"));
    CacheValue("PlayerPos", GetActorLocation());
    CacheValue("SaveData", MySaveStruct);
}

void MyGameMode::LoadGameState()
{
    int32 Score;
    FString Name;
    FVector Position;
    FSaveData SaveData;
    
    // Type-safe retrieval
    if (GetCachedValue("PlayerScore", Score))
    {
        SetScore(Score);
    }
    
    if (GetCachedValue("PlayerPos", Position))
    {
        SetActorLocation(Position);
    }
}
```

### 📚 Alternative: Type-Specific Methods

<details>
<summary>View type-specific API (if you prefer explicit types)</summary>

```cpp
// Explicit type methods
UHippocacheBlueprintLibrary::SetInt32(Context, "Collection", "Key", 100);
UHippocacheBlueprintLibrary::SetVector(Context, "Collection", "Key", FVector::ZeroVector);
UHippocacheBlueprintLibrary::SetString(Context, "Collection", "Key", "Hello");

// Type-specific getters
int32 Value;
if (UHippocacheBlueprintLibrary::GetInt32(Context, "Collection", "Key", Value))
{
    // Use value
}
```

</details>

## 🎯 Practical Examples

### 🎮 Complete Game Save System

```blueprint
=== Auto-Save System ===
[Timer: Every 60 seconds]
    ├─ [Create Struct: AutoSaveData]
    │   ├─ Timestamp = Now()
    │   ├─ PlayerData = GetPlayerData()
    │   ├─ WorldState = GetWorldState()
    │   └─ Checksum = CalculateChecksum()
    ├─ [Hippoo] "AutoSave", "Latest", AutoSaveData, 0.0
    ├─ [Hippoo] "AutoSave", "Backup", AutoSaveData, 3600.0
    └─ [UI Flash] "Auto-saved"
```

### 🌍 Multiplayer Session Cache

```blueprint
=== Host Creates Session ===
[Create Session Success]
    ├─ [Hippoo] "Session", "ID", SessionInfo.SessionID
    ├─ [Hippoo] "Session", "MaxPlayers", 16
    ├─ [Hippoo] "Session", "GameMode", "Deathmatch"
    └─ [Hippoo] "Session", "StartTime", Now()

=== Players Join ===
[On Player Joined]
    ├─ [Hippop] "Session", "GameMode" → Configure Player
    ├─ [Hippoo] "Players", PlayerID, PlayerInfo
    └─ [Broadcast] "Player Joined"
```

### 🎨 Complex UI State

```blueprint
=== Store Complete UI State ===
[Save UI Layout]
    ├─ [Create Struct: UIState]
    │   ├─ OpenWindows[] = GetAllOpenWindows()
    │   ├─ WindowPositions[] = GetAllPositions()
    │   ├─ TabStates[] = GetAllTabStates()
    │   └─ Theme = CurrentTheme
    └─ [Hippoo] "UI", "Layout", UIState, 0.0

=== Restore Complete UI State ===
[Load UI Layout]
    └─ [Hippop] "UI", "Layout" 
        → [Break Struct]
            ├─ RestoreWindows(OpenWindows)
            ├─ RestorePositions(WindowPositions)
            ├─ RestoreTabs(TabStates)
            └─ ApplyTheme(Theme)
```

## 📊 Performance

<table align="center">
<tr>
<th>Operation</th>
<th>Ops/Second</th>
<th>Response Time</th>
</tr>
<tr>
<td>🦛 Hippoo (Write)</td>
<td><b>290K - 395K</b></td>
<td>2-3μs</td>
</tr>
<tr>
<td>🦛 Hippop (Read)</td>
<td><b>660K - 671K</b></td>
<td>1μs</td>
</tr>
</table>

## ⏰ TTL (Time To Live)

| TTL | Behavior | Use Case |
|-----|----------|----------|
| `0.0` | ♾️ Never expires | Settings, Save games |
| `30.0` | ⏱️ 30 seconds | Combat buffs, Power-ups |
| `300.0` | ⏱️ 5 minutes | Session data, Temp calculations |
| `3600.0` | ⏱️ 1 hour | Level cache, Leaderboards |

## 💡 Best Practices

### 🦛 Hippoo/Hippop Guidelines

1. **🎯 Use for Everything** - They handle all types efficiently
2. **📦 Organize with Collections** - Group related data
3. **⏰ Set Appropriate TTLs** - Don't cache forever unless needed
4. **🔍 Check Results** - Always handle success/failure
5. **🧹 Clean Up** - Use Clear() when done with collections

### 🚀 Performance Tips

1. **Batch Operations** - Multiple Hippoo calls in sequence
2. **Read Heavy?** - Cache benefits from concurrent reads
3. **Memory Conscious** - Monitor with Num() function
4. **TTL Strategy** - Shorter TTLs = less memory

## 🔧 Troubleshooting

| Issue | Solution |
|-------|----------|
| 🔍 **Hippop returns default** | Check key spelling & TTL expiration |
| 🔀 **Type mismatch** | Ensure Hippop output matches stored type |
| 💾 **Memory growth** | Review TTLs, use Clear() on unused collections |
| 🐛 **Struct issues** | Verify USTRUCT() macro is present |

## 🤝 Contributing

1. 🍴 Fork the repository
2. 🌿 Create a feature branch
3. 💬 Commit your changes
4. 📤 Push to the branch
5. 🎯 Create a Pull Request

## 📄 License

MIT License - See [LICENSE](LICENSE) file

## 📈 Version History

### 🎉 v1.0.0
- ✨ Revolutionary Hippoo/Hippop wildcard nodes
- 🔒 Thread-safe FRWLock implementation
- 📊 High-performance caching (660K+ reads/sec)
- ⏰ Automatic TTL-based expiration
- 📦 Collection-based organization

---

<p align="center">
  Made with ❤️ for Unreal Engine developers
</p>

<p align="center">
  <a href="https://github.com/homveloper/ue5-hippo/issues">🐛 Report Bug</a>
  ·
  <a href="https://github.com/homveloper/ue5-hippo/issues">✨ Request Feature</a>
</p>
