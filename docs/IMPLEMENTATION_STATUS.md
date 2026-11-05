# 구현 현황

## Phase 1

- Windows x64용 `melang.exe` 및 CMake 빌드.
- UTF-8 소스 읽기, 렉서, 파서, AST, 진입점의 유일성 검사.
- `main()`, `void main()`, 문자열 리터럴의 `print()`, 줄바꿈과 `//` 주석.
- LLVM IR 생성 및 검증, x64 COFF 오브젝트 생성, LLVM의 COFF 링커 `lld-link`를 통한 PE 실행 파일 생성.
- `--help`, `--version`, `run`, `build`, `build -o`.
- VS Code의 `.m` 인식, 문법 강조, 괄호 자동 닫기, 스니펫, `print` 및 `void` 자동완성, 통합 터미널 실행 명령.

## 검증 결과

- MSVC x64 19.51.36257, CMake 4.3.1, Ninja 1.13.2, LLVM 22.1.8로 Release 빌드와 로컬 설치를 완료했습니다.
- `melang run examples/hello.m`과 `melang build examples/hello.m -o build/hello.exe`로 만든 실행 파일이 각각 `Hello Melang!`을 출력하고 0으로 종료했습니다. 실행 파일의 PE 헤더는 x64이며 가져오는 DLL은 `KERNEL32.dll`입니다.
- [컴파일러 통합 테스트](../tests/integration/compiler.ps1)는 PowerShell과 CMD에서의 실행, UTF-8 BOM, 한글·공백 경로, 잘못된 토큰·문법, 중복 진입점, 누락 파일과 종료 코드를 검사합니다.
- [VS Code 호스트 테스트](../tests/vscode/suite.js)는 VS Code 1.144.0에서 `.m` 인식, `p`의 `print()` 완성과 커서 위치, 통합 터미널 실행의 종료 코드 0을 확인했습니다.

## Phase 2

- `int`, `bool`, `string` 지역 변수, `var` 타입 추론, 재대입 및 정적 타입 검사.
- 정수 산술, 비교, 괄호와 우선순위, 단락 논리 연산, 정수·논리형 출력.
- 통합 테스트로 네이티브 산술과 재대입, 32비트 경계, 단락 평가, 0 나눗셈, 중복·미선언 변수 및 타입 오류를 검사합니다.

## Phase 3과 추가 구현

- 타입이 지정된 매개변수, 명시적 반환형과 `def`, 호출과 `return`.
- `if`, `elif`, `else`, `while`, 머랭 전용 `for`, `break`, `continue`.
- 문자열 동등 비교, 정수 복합 대입과 후위 증감 문장.
- 정수 슬라이스 리터럴, `append`, 읽기 인덱싱과 범위 검사.
- 기본 타입의 `T?`, `null`, `??`, 지역 변수의 null 검사와 제한된 흐름 분석.
- `char`, `short`, `long`, `float`, `double`, 실수 리터럴과 숫자 출력. 숫자 산술·비교, 값 보존 암시적 변환, 명시적 변환의 짝수 반올림과 하위 비트 보존을 지원합니다.
- 값 타입 `struct`, 위치 기반 초기화, 필드 읽기, 구조체 중첩과 함수 인수·반환을 지원합니다.
- 구조체 필드 대입, 정수 `foreach`, 비트 연산, 삼항 조건식, 정수 슬라이스의 복사 없는 범위 뷰를 지원합니다.
- nullable 구조체와 안전 필드 접근 `?.`를 지원합니다.
- 상대 경로 `import`, UTF-8 파일 입출력 `readText`·`writeText`, LLVM 빌드 최적화 `-O0`·`-O2`·`-O3`를 지원합니다.
- VS Code 실행 작업의 파일별 컴파일 오류를 문제 목록에 연결합니다.
- 로컬 설치용 VSIX를 패키징하고 별도 VS Code 프로필에서 설치를 검증했습니다.
- 컴파일과 런타임 검사 오류에 파일·줄·열을 표시합니다. `melang check`와 VS Code의 선택형 실시간 검사는 저장하지 않은 편집 내용도 검사할 수 있습니다.
- 파일 단위 `public`·`private` 선언과 가져온 모듈의 공개 범위 검사를 지원합니다.
- 시험 문법으로 지역 고정 정수 배열과 지역 뷰의 쓰기 충돌·반환 제한을 검사합니다.
- 참조 타입 `class`, 기본·명시적 생성자, 멤버 접근 제한, nullable 메서드 호출과 제한된 단일 상속을 지원합니다.

## 후속 구현

- 정수 외 배열·슬라이스, 함수 경계를 넘는 범용 정적 수명·소유권 검사.
- 메서드 재정의, 모듈 별칭과 한정 이름, 표준 라이브러리 확장.
- GC, 예외, 다중 상속, 제네릭, `union`, 포인터, 비관리 메모리 및 동시성.
- 언어 서버, 소스 수준 디버거, Windows 설치 프로그램.
