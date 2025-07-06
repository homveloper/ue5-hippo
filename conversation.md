  C++ 개발자를 위해 템플릿 기반의 제네릭 함수를 제공합니다.



    1 // HippocacheManager.h
    2 
    3 class FHippocacheManager
    4 {
    5 public:
    6     /**
    7      * @brief 템플릿을 사용하여 모든 USTRUCT 타입을 캐시에 저장합니다.
    8      * @tparam T 저장할 구조체 타입
    9      */
   10     template<typename T>
   11     void Set(const FString& Key, const T& Value, float TTLSeconds = 0.f);      
   12 
   13     /**
   14      * @brief 템플릿을 사용하여 캐시에서 USTRUCT 타입의 데이터를 가져옵니다.   
   15      * @tparam T 가져올 구조체 타입
   16      * @return 성공 시 true, 실패(키 없음, 타입 불일치) 시 false
   17      */
   18     template<typename T>
   19     bool Get(const FString& Key, T& OutValue);
   20 
   21     // UObject* 타입은 별도로 처리합니다 (GC 문제 때문에 TWeakObjectPtr 사용)  
   22     void SetObject(const FString& Key, UObject* Value, float TTLSeconds = 0.f);
   23     UObject* GetObject(const FString& Key);
   24 
   25     // ... 기타 내부 함수 및 멤버 ...
   26 };



   * 내부 구현: 이 템플릿 함수들은 내부적으로 Value를 FInstancedStruct로 변환하여 TMap에 저장합니다. 이렇게 함으로써 하나의 캐시 저장소에 여러 다른 타입의 데이터를 함께 보관할 수 있습니다.
   * `UObject` 처리: UObject 포인터는 가비지 컬렉션(GC)의 영향을 받으므로, FInstancedStruct와는 별도로 TWeakObjectPtr을 사용하여 안전하게 관리합니다.

  Blueprint API (UHippocacheSubsystem)


  블루프린트 사용자를 위해 와일드카드 핀을 사용하는 UFUNCTION을 제공합니다.


   * SetStructValue(Key, Value, TTL): Value 핀이 파란색 와일드카드 핀으로 표시되어 어떤 구조체든 연결할 수 있습니다.
   * GetStructValue(Key, Result, Value): Result는 실행 핀(Success/NotFound)으로 나뉘고, Value 핀은 원하는 구조체 타입 변수에 연결하여 사용할 수 있습니다.

  이처럼 C++과 블루프린트 각각의 환경에 최적화된 인터페이스를 제공하는 방향으로 개발을 진행하겠습니다.



 "하나의 거대한 관리자(Manager)" 대신, "필요할 때마다 생성해서 사용하는 독립적인 저장소(`Storage`)" 개념으로 아키텍처를 수정하겠습니다. 그리고 이 모든 저장소 인스턴스들을 내부적으로 관리하는 코어 로직을 두는 방향으로 진행하겠습니다.

  수정된 아키텍처 계획


   1. `FHippocacheStorage` (저장소 클래스):
       * 이것이 개발자가 직접 생성하고 상호작용할 주체입니다.
       * 각 FHippocacheStorage 인스턴스는 고유한 데이터 저장 공간을 가집니다.
       * C++에서는 MakeShared<FHippocacheStorage>() 등으로 직접 생성하여 사용합니다.
       * 블루프린트에서는 "Create Hippocache Storage" 같은 팩토리 함수를 통해 생성하고, 변수에 저장하여 사용하게 됩니다.
       * 제네릭 Set<T>, Get<T> 함수와 UObject용 SetObject, GetObject 함수를 제공합니다.


   2. `UHippocacheSubsystem` (중앙 관리 및 BP 인터페이스):
       * 이 서브시스템은 이제 저장소들을 생성하고 관리하는 팩토리 및 관리자 역할을 합니다.
       * 내부적으로 생성된 모든 FHippocacheStorage 인스턴스들을 추적하고, TTL이 만료된 데이터를 정리하는 등 전역적인 관리를 수행합니다. (예: TMap<FName, TSharedPtr<FHippocacheStorage>> 형태로 관리)
       * 블루프린트를 위한 함수들을 제공합니다.
           * CreateStorage(FName StorageName): 새로운 저장소를 생성하거나 기존 저장소를 가져옵니다. FName을 키로 사용하여 저장소를 식별합니다.
           * SetStructValue(FHippocacheStorageHandle& Target, FString Key, ...): 특정 저장소에 값을 저장합니다. FHippocacheStorageHandle은 블루프린트에서 C++의 TSharedPtr를 안전하게 다루기 위한 Wrapper 구조체입니다.
           * GetStructValue(FHippocacheStorageHandle& Target, FString Key, ...): 특정 저장소에서 값을 가져옵니다.



    1 graph TD
    2     subgraph "User Interaction"
    3         A[C++ Code] -- Creates/Uses --> C(FHippocacheStorage);  
    4         B[Blueprints] -- Uses --> D(UHippocacheSubsystem);      
    5     end
    6 
    7     subgraph "Core System"
    8         D -- Creates & Manages --> C;
    9         C -- Contains --> E{TMap<FString, FCachedItem>};        
   10         D -- Manages all instances of --> C;
   11     end
   12 
   13     style C fill:#f9f,stroke:#333,stroke-width:2px
   14     style D fill:#ccf,stroke:#333,stroke-width:2px


  이점


   * 모듈성 및 유연성: 기능별, 목적별로 여러 개의 독립된 캐시 저장소를 만들어 사용할 수 있습니다. (예: 'PlayerProfile' 캐시, 'SessionData' 캐시 등)
   * 명확한 소유권: 각 저장소의 생명주기를 개발자가 직접 관리할 수 있습니다.
   * 테스트 용이성: FHippocacheStorage 자체를 테스트하는 것이 매우 간단해집니다.


  1. 디렉토리 구조 생성
  hippocache/Source/ 아래에 다음과 같이 폴더를 구성하겠습니다.   
   * hippocache/Source/hippocache/Public: Public 헤더 파일 위치  
   * hippocache/Source/hippocache/Private: Private 소스 파일 위치
   * hippocache/Source/hippocache/Private/Tests: 테스트 코드 위치


  2. 파일 생성
  TDD 원칙에 따라 테스트 파일을 먼저 구상하고, 그 다음 실제 구현 파일을 만들겠습니다.


   * Private/Tests/HippocacheStorageTest.cpp: FHippocacheStorage의 기능을 검증할 테스트 코드입니다.
   * Public/HippocacheStorage.h: FHippocacheStorage 클래스 및 관련 데이터 구조의 선언부입니다.     
   * Private/HippocacheStorage.cpp: FHippocacheStorage 클래스의 구현부입니다.
