# NBVS
NBVS — Node Based Visual Scripting (C++/ImGui)

개요
- C++/Dear ImGui + ImNodes + GLFW + OpenGL 기반 최소 템플릿
- macOS + VS Code에서 바로 빌드 가능(CMake)

필요 패키지(macOS)
1) Homebrew 설치 후 의존성 설치

```zsh
brew install cmake glfw glew
```

빌드 & 실행
```zsh
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build . --config RelWithDebInfo -j
./nbvs
```

문제 해결
- GLEW not found: `brew install glew` 후, 다시 CMake
- 링크 오류 시: Xcode Command Line Tools가 필요할 수 있음 `xcode-select --install`

구조
- `src/main.cpp`: 엔트리포인트. ImGui/ImNodes 초기화와 간단한 그래프 데모
- `CMakeLists.txt`: 의존성(Imgui/ImNodes/GLFW) 자동 가져오기, GLEW 사용

다음 단계 제안
- 노드 데이터 모델 정의(Node/Pin/Link, 직렬화 JSON)
- Undo/Redo(명령 패턴) 추가
- 노드 실행 엔진(위상정렬 + 타입 검사)

