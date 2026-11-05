def add(int a, int b) {
    return a + b
}

main() {
    int total = 0
    for (i=0 in i++(5)) {
        total = add(total, i)
    }

    if (total > 10) {
        print("Over")
    } elif (total == 10) {
        print("Total")
    } else {
        print("Under")
    }

    while (total > 8) {
        print(total)
        total = total - 1
    }
}
