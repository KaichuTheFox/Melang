# 컴파일러 구조

```text
.m → Source Manager → Lexer → Parser → AST → Semantic Analyzer
   → LLVM IR → LLVM X86 TargetMachine → COFF .obj
   → lld-link + Melang 런타임 및 Windows 링크 라이브러리 → x64 .exe
```

- `src/source`에서 UTF-8을 검증하고 `import`의 파일 관계를 해석합니다. `src/lexer`와 `src/parser`가 파일별 위치를 가진 토큰과 AST를 구성합니다. `src/semantic`은 진입점, 함수 시그니처, 지역 변수의 스코프, 반환 경로와 null 사용을 검사합니다.
- `src/codegen/llvm`은 LLVM C API로 함수 시그니처를 선언한 뒤 함수 몸체, 분기와 루프를 기본 블록으로 생성합니다. 모듈을 검증하고, `-O2` 또는 `-O3`를 지정하면 해당 LLVM 최적화 파이프라인을 실행한 뒤 `x86_64-pc-windows-msvc` 타깃의 오브젝트를 출력합니다.
- `src/driver`는 임시 디렉터리에서 링크합니다. `build`는 링크가 성공하면 지정한 경로에 결과물을 배치합니다. `run`은 임시 실행 파일을 실행하고 종료 코드를 전달합니다.
- `runtime/src`는 프로그램 시작과 종료, 출력, 정수 나눗셈 검사, 정수 슬라이스 버퍼를 담당합니다. Windows 콘솔에는 UTF-16을 쓰고, 파일이나 파이프로 리다이렉트하면 UTF-8 바이트를 씁니다.

구문 오류는 `파일:줄:열: error MLxxxx: 메시지` 형식으로 표시합니다. 줄과 열은 1부터 시작하며 열은 유니코드 코드 포인트 단위입니다.
