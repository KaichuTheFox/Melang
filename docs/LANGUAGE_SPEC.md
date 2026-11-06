# Moulang language specification

## Source files and entry point

- Source files are UTF-8. The compiler accepts an initial BOM and LF, CRLF, or CR line endings.
- An executable has exactly one parameterless main() or void main(). Normal completion returns exit code 0.
- Braces delimit blocks. Separate statements in the same block with newlines. Inside parentheses, line breaks may separate tokens. A double slash starts a comment that ends at the next line break.
- Strings are single-line, double-quoted UTF-8 text, including empty strings. String escape syntax remains unspecified. print(value) writes a numeric, bool, or string value followed by a newline.

## Values and expressions

- A local declaration requires an initializer. Use a type, such as int count = 10, or use var to infer it at compile time. Reassignment must respect the declared type. A local is visible within its declaring block and cannot redeclare a name in that block or a parent block.
- Numeric types are char (unsigned 8-bit), short (signed 16-bit), int and long (each signed 32-bit), float (32-bit), and double (64-bit). Integer literals are int. A literal containing a decimal point or exponent is double. char(65) prints the byte value 65. Overload resolution for int and long remains unspecified.
- Arithmetic and comparison use the operand type when both types match or a common type that preserves both operands' values. The common type for float and int is double. Numeric locals support compound assignment and postfix ++ and -- as statements. String equality compares UTF-8 bytes. Boolean values print as true or false.
- Integer arithmetic wraps at the type's width. Integer division truncates toward zero. Dividing an integer by zero terminates with a runtime error. Floating-point arithmetic follows IEEE 754.
- Implicit numeric conversions in assignment, arguments, and return values must preserve all source values and their precision. A short converts implicitly to float. Use float(value) for an int.
- Explicit integer conversions preserve low bits. Real-to-integer conversion first rounds to the nearest integer, choosing the even integer at a tie. Converting NaN or infinity to an integer, or converting a finite double outside the float range to float, produces a runtime error and exits with code 1.
- Integer bitwise operators are ~, &, ^, |, <<, and >>. A shift uses the left operand's width and rejects negative counts and counts at least that width. Signed right shifts preserve the sign bit.
- Precedence runs from parentheses and postfix expressions through unary operators, multiplication and division, addition and subtraction, shifts, ordering, equality, bitwise &, ^, and |, logical && and ||, null coalescing, and the ternary conditional. Binary operators associate left, except for ??, which associates right. Logical operators and ?? skip the right operand when the left determines the result. A ternary condition must be bool, its result types must be compatible, and only the selected expression is evaluated. Ternaries associate right.

## Functions and control flow

- Function parameters have explicit types. Declare an explicit return type, as in int add(int a, int b), or use def to infer it. A value-returning function must return along every reachable path. Loop termination is analyzed conservatively. Recursive return type inference with circular dependencies requires an explicit return type.
- if, elif, else, and while provide branching and loops. Conditions must be bool. for (i=0 in i++(5)) evaluates its bounds once and iterates from 0 through 4. The loop variable is local to the loop. break and continue target the nearest loop.

## Slices, structs, and nullability

- int numbers[] = [10, 20, 30] and var numbers = [] create integer slices. append(numbers, 40) returns a slice with one more element and preserves the original slice length and elements. Indices start at zero. Out-of-bounds access terminates with exit code 1. Heap-backed slice buffers remain allocated until process exit.
- foreach (item in numbers) evaluates its integer slice, view, or fixed array once. Each element is copied into a local loop variable in order. break and continue are supported.
- numbers[begin:end] is a read-only, zero-copy view over the range from begin to just before end. Bounds must satisfy 0 <= begin <= end <= length. A view supports indexing and further slicing.
- Structs are named value types. Declare fields on separate lines. Position(10, 20) initializes fields in declaration order. pos.x reads a field. Structs may contain other structs and pass through function arguments and return values. Assignment copies the value. Recursive inclusion by value is rejected.
- A float field in positional initialization can accept an exactly representable integer literal directly. Convert an int variable explicitly with float(value). Field assignment and nested field assignment are supported. Copies preserve their own field values.
- Types are non-nullable by default. T? allows null for supported scalar types, structs, and classes. nickname ?? "Unknown" evaluates the fallback only for null. Direct assignment or comparisons with null can establish that a value is present in a local scope. Reassignment and control flow can change this result.
- pos?.x returns null for a null receiver and reads the field otherwise. Nullable class receivers also support safe method calls. A null check that establishes a struct value is present allows direct field access.

## Imports and file I/O

- import "modules/file.m" resolves a relative path from the importing file. Each file is processed once. A file sees its own declarations and the public declarations of files imported directly or transitively.
- Top-level functions, structs, and classes are public by default. public makes this explicit. private limits visibility to the declaring file. Local file declarations take precedence over imported public declarations. Conflicting public names, public declarations exposing private types, import cycles, and an imported main are compile errors.
- readText(path) reads a UTF-8 file into a string. writeText(path, text) writes UTF-8 and replaces an existing file. Relative paths use the program's working directory. I/O errors and invalid UTF-8 cause runtime errors.

## Experimental fixed arrays and classes

- A local fixed integer array can be declared as int values[3] = [10, 20, 30]. The length is part of its type and must match the initializer. Arrays support indexed reads and writes, value copies, and foreach.
- A view of a fixed array borrows its storage. The array cannot be written while that view is in local scope. The borrowed binding cannot be reassigned or returned from the function.
- A class is a reference type. Assignment copies the reference. Members are private by default. Methods can use this.field or the field name. new Counter(0) creates an instance. Class objects remain allocated until process exit.
- A class whose fields all have defaults can use new ClassName(). Declare one explicit constructor with public ClassName(typedParameters) { ... }. Field defaults run before the constructor body. Fields without defaults must be initialized directly in the first statements of the constructor.
- class Child : Parent provides single inheritance. The base default constructor or public zero-argument constructor runs first. A child reference can be assigned to the base type. Access to private base members, member hiding, method overriding, and multiple inheritance are compile errors.

The final fixed array notation and general lifetime and aliasing rules are open. Exception handling is planned for invalid conversions. Multiple inheritance is a future design in which method conflicts must be resolved explicitly. Base selection syntax and layout remain open.
