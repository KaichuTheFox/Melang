# Windows 배포

## 개발용 컴파일러

`cmake --install build --prefix build/install`을 실행하면 `bin`에 `melang.exe`와 `melang_runtime.lib`가 함께 설치됩니다. 두 파일이 들어 있는 디렉터리를 PATH에 추가할 수 있습니다.

`melang.exe`는 CMake 구성 때 지정한 COFF 링커 `lld-link.exe`, Windows SDK의 x64 `kernel32.lib`, MSVC의 정적 `libcmt.lib`, `libcpmt.lib`, `libvcruntime.lib` 절대 경로를 사용합니다. 정적 라이브러리는 큰 스택 프레임 검사와 실수 출력에 필요한 코드를 제공합니다. 이 경로들을 유지하면 일반 CMD 또는 PowerShell에서도 실행할 수 있습니다. 개발 도구 설치가 바뀌면 CMake를 다시 구성해 빌드합니다. 공식 LLVM 아카이브가 제작 환경의 DIA SDK 경로를 참조하는 경우, CMake는 현재 Visual Studio 설치의 x64 `diaguids.lib`를 찾습니다.

## 생성된 프로그램

생성된 `.exe`에는 최소 Melang 런타임이 정적으로 링크됩니다. Windows x64에서 Windows 시스템 DLL을 사용해 실행됩니다. 예제 `hello.exe`는 `KERNEL32.dll`만 가져옵니다.

## 향후 배포

Windows 설치 프로그램, 선택적 PATH 등록, 재배포용 링커 구성, VS Code VSIX는 후속 작업입니다.
