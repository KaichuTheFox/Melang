main() {
    int numbers[] = [10, 20, 30]
    numbers = append(numbers, 40)
    print(numbers[3])

    string? nickname = null
    print(nickname ?? "Unknown")
    nickname = "Melang"
    if (nickname != null) {
        print(nickname)
    }
}
