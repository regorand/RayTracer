#define MAX_STACK_SIZE 32

struct Stack {
    uint buf[MAX_STACK_SIZE];
    int currentSize;
};

void push(inout Stack s, uint val) {
    if (s.currentSize < MAX_STACK_SIZE - 1) {
        s.buf[s.currentSize] = val;
        s.currentSize++;
    }
}

uint top(inout Stack s) {
    if (s.currentSize > 0) {
        return s.buf[s.currentSize - 1];
    }
    return -1;
}

uint ptop(inout Stack s) {
    if (s.currentSize > 0) {
        s.currentSize--;
        return s.buf[s.currentSize];
    }
    return -1;
}

void pop(inout Stack s) {
    s.currentSize = max(0, s.currentSize - 1);
}