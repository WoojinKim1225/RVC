# MyGTestProject

C++ 프로젝트와 GoogleTest를 연동하여 단위 테스트를 실행하는 예제 프로젝트입니다.

## 전제 조건

*   [CMake](https://cmake.org/download/) (버전 3.14 이상)
*   C++ 컴파일러 (GCC, Clang, MSVC 등)

## 빌드 방법

1.  프로젝트를 클론하고 해당 디렉토리로 이동합니다.

2.  빌드 디렉토리를 생성하고 이동합니다. 소스 트리 외부에서 빌드하는 것이 좋습니다.
    ```bash
    mkdir build
    cd build
    ```

3.  CMake를 사용하여 빌드 파일을 생성합니다.
    ```bash
    cmake ..
    ```

4.  프로젝트를 컴파일합니다.
    ```bash
    cmake --build .
    ```

## 테스트 실행 방법

빌드가 완료된 후, `build` 디렉토리에서 다음 명령어를 실행하여 테스트를 수행할 수 있습니다.

```bash
ctest
```

더 자세한 테스트 실행 결과를 보려면 `--verbose` 옵션을 추가하세요.

```bash
ctest --verbose
```