class Counter {
    int count

    public Counter(int start) {
        count = start
    }

    public void increment() {
        count = count + 1
    }

    public int value() {
        return count
    }
}

main() {
    int values[3] = [10, 20, 30]
    Counter counter = new Counter(0)
    Counter alias = counter
    foreach (item in values[0:3]) {
        alias.increment()
        print(item)
    }
    print(counter.value())
}
