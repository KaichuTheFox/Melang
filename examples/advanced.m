import "modules/format.m"

struct Position {
    int x
    int y
}

main() {
    Position pos = Position(10, 20)
    pos.x = 30
    print(pos.x)

    var numbers = [10, 20, 30, 40]
    foreach (number in numbers[1:3]) {
        print(number)
    }

    Position? missing = null
    print(missing?.x ?? 0)
    print(label((pos.x & 1) == 0))
}
