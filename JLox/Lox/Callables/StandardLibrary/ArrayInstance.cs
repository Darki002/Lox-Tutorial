using Lox.Errors;

namespace Lox.Callables.StandardLibrary;

public class ArrayInstance(int size) : LoxInstance(null)
{
    private readonly object?[] values = new object?[size];
    
    public override object Get(Token name)
    {
        return name.Lexeme switch
        {
            "get" => new ArrayGet(values, name),
            "set" => new ArraySet(values, name),
            "length" => values.Length,
            _ => throw new RuntimeError(name, $"Undefined property '{name.Lexeme}'.")
        };
    }
    
    private readonly struct ArrayGet(object?[] values, Token name) : ILoxCallable
    {
        public int Arity => 1;

        public object? Call(Interpreter interpreter, List<object?> arguments)
        {
            var index = Convert.ToInt32(arguments[0]);
            AssertIndexInRange(name, values, index);
            return values[index];
        }
    }
    
    private readonly struct ArraySet(object?[] values, Token name) : ILoxCallable
    {
        public int Arity => 2;

        public object? Call(Interpreter interpreter, List<object?> arguments)
        {
            var index = Convert.ToInt32(arguments[0]);
            AssertIndexInRange(name, values, index);
            values[index] = arguments[1];
            return null;
        }
    }

    private static void AssertIndexInRange(Token token, object?[] array, int index)
    {
        if (index < 0 || index >= array.Length)
        {
            throw new RuntimeError(token, "Index must be non-negative and less than the size of the array");
        }
    }
}