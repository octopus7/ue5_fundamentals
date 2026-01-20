---
trigger: always_on
---

md 파일 작성은 한국어 영어 병행표기, 응답은 한국어, 코드상 주석은 영어

## 프로젝트 구조

## 엔진 버전
- **Unreal Engine 5.7.1**

## 빌드 규칙
- **빌드는 항상 사용자가 직접 수행**
- AI가 자동으로 빌드 명령어(`Build.bat`, `UnrealBuildTool` 등)를 실행하지 않음
- 사용자가 Visual Studio나 Unreal Editor에서 직접 빌드
- 엔진 경로: `E:\WorkTemp\Epic Games\UE_5.7`

## C++ 코딩 규칙 (Unreal Engine)

### 헤더 파일 include 순서
- **`.generated.h` 파일은 반드시 마지막 `#include`여야 함**
- 올바른 순서:
  1. `#include "CoreMinimal.h"` (첫 번째)
  2. 기타 Engine/Module 헤더들
  3. 동일 모듈 내 헤더들
  4. `#include "ClassName.generated.h"` (마지막)

```cpp
// 올바른 예시
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"  // 반드시 마지막!
```

> [!IMPORTANT]
> **AI가 헤더 파일(`.h`)에 include 구문을 수정한 경우, 반드시 HeaderReorder 도구를 실행해야 합니다:**

> 이 도구는 자동으로 `.generated.h`가 마지막 include가 되도록 재정렬합니다.

> [!TIP]
> **수동 실행이 필요한 경우 (헤더 순서 에러 발생 시):**
> 
> ```powershell
> cd ..  # 레포지터리 루트로 이동 (ue5_fundamentals)
> cd .\Tools\HeaderReorder
> dotnet run -- "..\..\BasicProject" --all
> ```

---

> [!IMPORTANT]
> 이 규칙들은 모든 작업에 자동으로 적용되며, 새로운 채팅 세션에서도 이 파일을 참조하여 일관성을 유지합니다.