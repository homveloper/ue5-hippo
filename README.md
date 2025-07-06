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

## 🌟 Introducing Hippoo & Hippop - The Magic of Wildcard Caching

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

### 🦛 Hippoo - Universal Setter

<p align="center">
  <img src="https://via.placeholder.com/800x300/2196F3/FFFFFF?text=Hippoo+Node+-+Accepts+ANY+Data+Type!" alt="Hippoo Node">
</p>

The **Hippoo** node is a wildcard setter that magically accepts ANY data type:

```
[🦛 Hippoo]
├─ Collection: "Game"
├─ Key: "PlayerData"  
├─ Value: (Connect ANYTHING here!)
├─ TTL: 300.0
└─ Result: Success/Failure
```

#### 🎨 Examples with Hippoo

```blueprint
// Store an Integer
Hippoo → Value: 100

// Store a String  
Hippoo → Value: "Hello World"

// Store a Vector
Hippoo → Value: (X:100, Y:200, Z:300)

// Store a Transform
Hippoo → Value: Player Transform

// Store a Custom Struct
Hippoo → Value: MyGameStateStruct

// Store LITERALLY ANYTHING!
Hippoo → Value: [Any Blueprint Variable]
```

### 🦛 Hippop - Universal Getter

<p align="center">
  <img src="https://via.placeholder.com/800x300/4CAF50/FFFFFF?text=Hippop+Node+-+Type-Safe+Retrieval!" alt="Hippop Node">
</p>

The **Hippop** node retrieves values with automatic type detection:

```
[🦛 Hippop]
├─ Collection: "Game"
├─ Key: "PlayerData"
├─ Value: (Outputs the EXACT type stored!)
└─ Result: Success/Failure
```

#### 🎯 Type-Safe Magic

Hippop's wildcard pin automatically adapts to whatever you connect:

```blueprint
// If you stored an Int32
Hippop → Value → Set Integer Variable ✅

// If you stored a Vector  
Hippop → Value → Set Actor Location ✅

// If you stored a Transform
Hippop → Value → Set Actor Transform ✅

// Type safety guaranteed!
Hippop → Value → [Connects only to matching types]
```

### 💡 Real-World Example

<table>
<tr>
<th width="50%">🎮 Save Game State</th>
<th width="50%">📖 Load Game State</th>
</tr>
<tr>
<td>

```blueprint
// One node to rule them all!
Hippoo: "Player", "Health", 100.0
Hippoo: "Player", "Position", Location
Hippoo: "Player", "Inventory", ItemArray
Hippoo: "Game", "State", GameStateStruct
Hippoo: "UI", "Settings", SettingsStruct
```

</td>
<td>

```blueprint
// Type-safe retrieval!
Hippop: "Player", "Health" → Float
Hippop: "Player", "Position" → Vector
Hippop: "Player", "Inventory" → Array
Hippop: "Game", "State" → Struct
Hippop: "UI", "Settings" → Struct
```

</td>
</tr>
</table>

## ✨ Features

- 🎯 **Wildcard Nodes**: Hippoo & Hippop handle ANY data type automatically
- 🚀 **High Performance**: Read-write lock separation achieving 660K+ ops/sec
- 🔒 **Thread-Safe**: Built with FRWLock for optimal concurrent access
- 🎨 **Blueprint First**: Complete Blueprint node library for all operations
- ⏰ **Auto Expiration**: TTL-based cache with automatic cleanup
- 📦 **Collections**: Organize cache data by collection names

## 🏗️ Architecture

Hippocache is designed with a clean separation of concerns:

- **🌐 Game Instance Subsystem**: Core storage engine with thread-safe operations
- **🎨 Blueprint Library**: User-friendly API with wildcard nodes
- **🦛 Hippoo/Hippop**: Revolutionary wildcard nodes for any data type
- **📦 Collections**: Organize cached data by logical groups

## 🎮 Blueprint Usage

### 🌟 Primary Method: Hippoo & Hippop (Recommended)

#### Save ANY Data with Hippoo

```blueprint
BeginPlay Event
├─ Hippoo ["Player", "Name", "Alice", 600.0]
├─ Hippoo ["Player", "Level", 42, 600.0]
├─ Hippoo ["Player", "Position", GetActorLocation(), 60.0]
├─ Hippoo ["Player", "SaveData", SaveGameStruct, 3600.0]
└─ Print "Game data cached!"
```

#### Load ANY Data with Hippop

```blueprint
Load Game Event
├─ Hippop ["Player", "Name"] → Set Name Text
├─ Hippop ["Player", "Level"] → Set Level Text  
├─ Hippop ["Player", "Position"] → Set Actor Location
├─ Hippop ["Player", "SaveData"] → Restore Game State
└─ Branch on Last Result → Success/Fail
```

### 📚 Alternative: Type-Specific Nodes

For cases where you prefer explicit type handling, Hippocache also provides traditional typed nodes:

<details>
<summary>Click to see all type-specific nodes</summary>

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

</details>

### 📦 Collections

Use collections to organize your cached data:

```
🎮 Game State (using Hippoo/Hippop)
├─ "Player" → Any player-related data
├─ "UI" → Any UI state or settings
├─ "Session" → Any session information
└─ "World" → Any world state data
```

## 💻 C++ Usage

### 🦛 Using Hippoo/Hippop in C++

```cpp
#include "HippocacheBlueprintLibrary.h"

// The magic functions behind Hippoo/Hippop
template<typename T>
FHippocacheResult Hippoo(const UObject* WorldContext, 
                        FName Collection, 
                        const FString& Key, 
                        const T& Value, 
                        float TTL = 300.0f)
{
    // Hippoo internally handles type detection
    return UHippocacheBlueprintLibrary::SetValue(
        WorldContext, Collection, Key, Value, TTL
    );
}

template<typename T>
FHippocacheResult Hippop(const UObject* WorldContext,
                        FName Collection,
                        const FString& Key,
                        T& OutValue)
{
    // Hippop automatically converts to correct type
    return UHippocacheBlueprintLibrary::GetValue(
        WorldContext, Collection, Key, OutValue
    );
}

// Example usage
void SaveGameState()
{
    // Store different types with one function pattern
    Hippoo(this, "Player", "Score", 1000);
    Hippoo(this, "Player", "Name", FString("Alice"));
    Hippoo(this, "Player", "Position", GetActorLocation());
    Hippoo(this, "Game", "SaveData", MyCustomStruct);
}

void LoadGameState()
{
    int32 Score;
    FString Name;
    FVector Position;
    FMyCustomStruct SaveData;
    
    // Type-safe retrieval
    if (Hippop(this, "Player", "Score", Score))
    {
        // Use score
    }
    
    if (Hippop(this, "Player", "Position", Position))
    {
        SetActorLocation(Position);
    }
}
```

### 🔧 Traditional Type-Specific Methods

<details>
<summary>Click to see type-specific C++ API</summary>

```cpp
#include "HippocacheBlueprintLibrary.h"

// Type-specific setters
UHippocacheBlueprintLibrary::SetInt32(WorldContext, "Player", "Score", 100);
UHippocacheBlueprintLibrary::SetString(WorldContext, "Player", "Name", "Alice");
UHippocacheBlueprintLibrary::SetVector(WorldContext, "Player", "Position", FVector(0,0,0));

// Type-specific getters
int32 Score;
FString Name;
FVector Position;

if (UHippocacheBlueprintLibrary::GetInt32(WorldContext, "Player", "Score", Score))
{
    // Use score
}
```

</details>

## 🎯 Common Use Cases

### 🎮 Save System with Hippoo/Hippop

```blueprint
Save Game Event
├─ Create SaveGameStruct
│   ├─ Set all game data
│   └─ Set timestamp
├─ Hippoo ["Save", "Current", SaveGameStruct, 0.0]  // No expiry
├─ Hippoo ["Save", "Backup", SaveGameStruct, 3600.0] // 1hr backup
└─ Show "Game Saved" notification
```

### 🎨 Dynamic UI State

```blueprint
UI State Changed Event
├─ Hippoo ["UI", "TabIndex", SelectedTab.Index, 1800.0]
├─ Hippoo ["UI", "Filters", FilterStruct, 1800.0]
├─ Hippoo ["UI", "SortOrder", SortEnum, 1800.0]
└─ Continue...

Restore UI Event  
├─ Hippop ["UI", "TabIndex"] → Set Active Tab
├─ Hippop ["UI", "Filters"] → Apply Filters
├─ Hippop ["UI", "SortOrder"] → Apply Sort
└─ Refresh UI
```

### 🌍 Checkpoint System

```blueprint
Reach Checkpoint Event
├─ Hippoo ["Checkpoint", "Transform", PlayerTransform, 0.0]
├─ Hippoo ["Checkpoint", "Health", CurrentHealth, 0.0]
├─ Hippoo ["Checkpoint", "Inventory", InventoryArray, 0.0]
├─ Hippoo ["Checkpoint", "WorldState", WorldStateStruct, 0.0]
└─ Play checkpoint sound

Respawn Event
├─ Hippop ["Checkpoint", "Transform"] → Set Actor Transform
├─ Hippop ["Checkpoint", "Health"] → Set Health
├─ Hippop ["Checkpoint", "Inventory"] → Restore Inventory
├─ Hippop ["Checkpoint", "WorldState"] → Restore World
└─ Fade in from black
```

## 📊 Performance Benchmarks

<table align="center">
<tr>
<th>Operation</th>
<th>Ops/Second</th>
<th>Response Time</th>
</tr>
<tr>
<td>🦛 Hippoo (Set)</td>
<td><b>290K - 395K</b></td>
<td>2-3μs</td>
</tr>
<tr>
<td>🦛 Hippop (Get)</td>
<td><b>660K - 671K</b></td>
<td>1μs</td>
</tr>
</table>

## ⏰ TTL (Time To Live)

Control how long data stays in cache:

| TTL Value | Behavior | Use Case |
|-----------|----------|----------|
| `0.0` | Never expires | Save games, settings |
| `60.0` | 1 minute | Temporary calculations |
| `300.0` | 5 minutes | Session data |
| `3600.0` | 1 hour | Level cache |

## 💡 Best Practices

### 🦛 Hippoo/Hippop Best Practices

1. **Use for rapid prototyping** - Change data types without changing nodes
2. **Perfect for save systems** - Store entire save states with one node
3. **Great for dynamic data** - Handle any runtime-determined types
4. **Ideal for mod support** - Modders can store custom data types

### 📦 Collection Strategies

```
✅ Recommended:
"Player"      // All player data
"Game"        // Game state
"UI"          // UI state
"Session"     // Temporary session data
"Save"        // Persistent save data

❌ Avoid:
"Data"        // Too generic
"Temp"        // Unclear purpose
"Cache"       // Redundant
```

## 🔧 Troubleshooting

### ❗ Common Issues

| Issue | Solution |
|-------|----------|
| 🔍 **Hippop returns default value** | Check key name and TTL expiration |
| 🔀 **Type mismatch on Hippop** | Ensure you're connecting to the correct type |
| 💾 **Memory growth** | Set appropriate TTLs, use Clear() |
| 🐛 **Struct not working** | Verify struct has USTRUCT() macro |

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
- ✨ Revolutionary Hippoo/Hippop wildcard nodes
- 🔒 Thread-safe operations with FRWLock
- 📊 High-performance caching (660K+ reads/sec)
- 🎨 Complete Blueprint node library
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