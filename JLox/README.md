# JLox Language Guide

JLox is a dynamically-typed, object-oriented scripting language inspired by Lox from the book "Crafting Interpreters". This guide covers the essential features, syntax, and semantics of JLox.

---

## Table of Contents
- [Basic Syntax](#basic-syntax)
- [Types](#types)
- [Variables](#variables)
- [Expressions](#expressions)
- [Control Flow](#control-flow)
- [Functions](#functions)
- [Classes & Inheritance](#classes--inheritance)
- [Arrays](#arrays)
- [Standard Library](#standard-library)
- [Other Features](#other-features)

---

## Basic Syntax
- Statements end with a semicolon (`;`).
- Blocks are enclosed in `{ ... }`.
- Comments:
  - Single-line: `// comment`
  - Multi-line: `/* comment ... */`

## Types
- `number` (floating point)
- `string`
- `bool` (`true`/`false`)
- `nil` (null)
- `array`
- `class`/`instance`

## Variables
```lox
var x = 42;
var name = "JLox";
```
Variables are declared with `var`. Initialization is optional.

## Expressions
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Logical: `and`, `or`, `!`
- Grouping: `( ... )`
- Assignment: `=`

## Control Flow
### If Statement
```lox
if (x > 0) {
  print "positive";
} else {
  print "non-positive";
}
```

### While Loop
```lox
while (x < 10) {
  print x;
  x = x + 1;
}
```

### For Loop
```lox
for (var i = 0; i < 5; i = i + 1) {
  print i;
}
```

### Break
`break;` exits the nearest loop.

## Functions
```lox
fun add(a, b) {
  return a + b;
}

print add(2, 3); // 5
```
- Functions are declared with `fun`.
- Anonymous functions: You can create anonymous (lambda) functions using `fun` keyword. Example:

```lox
var adder = fun(a, b) { return a + b; };
print adder(2, 3); // 5
```
- Functions are first-class values.

## Classes & Inheritance
```lox
class Animal {
  speak() { print "..."; }
}

class Dog < Animal {
  speak() { print "Woof!"; }
}

var d = Dog();
d.speak(); // Woof!
```
- `class` defines a class. Use `<` for inheritance.
- `init` is the constructor.
- `this` refers to the instance.
- `super` calls superclass methods.
- Class methods: `class fun name() { ... }`
- Operator overloading: You can define custom behavior for operators in classes using the `class operator` syntax. Supported operators are:
  - `+` (PLUS)
  - `-` (MINUS)
  - `*` (STAR)
  - `/` (SLASH)
  - `%` (PERCENT)
  - `==` (EQUAL_EQUAL)
  - `!=` (BANG_EQUAL)
  - `>` (GREATER)
  - `>=` (GREATER_EQUAL)
  - `<` (LESS)
  - `<=` (LESS_EQUAL)

Example:
```lox
class Vector {
  init(x, y) {
    this.x = x;
    this.y = y;
  }
  class operator + (a, b) {
    return Vector(a.x + b.x, a.y + b.y);
  }
}

var v1 = Vector(1, 2);
var v2 = Vector(3, 4);
var v3 = v1 + v2; // Uses overloaded +
```

## Arrays
```lox
var arr = [1, 2, 3];
print arr[0]; // 1
arr[1] = 42;
```
- Arrays use `[ ... ]` for literals.
- Access with `arr[index]`.

## Standard Library
JLox provides several built-in standard library functions:

| Function     | Description                                                                                                      | Example Usage        |
|--------------|------------------------------------------------------------------------------------------------------------------|----------------------|
| `clock()`    | Returns the current time in microseconds.                                                   | `var t = clock();`   |
| `typeof(x)`  | Returns the type of `x` as a string. Possible results: `"number"`, `"string"`, `"array"`, class name, or `"callable"`. | `print typeof(42);`  |
| `Array(size)`| Creates a new array of the given size (number). All elements initialized to `nil`.                               | `var arr = Array(5);`|

You can use these functions directly in your JLox programs. For more details, see the source files in `Lox/Callables/StandardLibrary/`.

## Other Features
- Print: `print expr;`
- Return: `return expr;`
- Variable shadowing is allowed (with warnings).
- Errors and warnings are reported for misuse (e.g., using `break` outside loops, returning from top-level code, etc).

---

For more details, see the source files in `Lox/`.
