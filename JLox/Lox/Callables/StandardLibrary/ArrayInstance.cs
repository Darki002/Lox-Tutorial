using System.Text;
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

    public object? GetValue(double index)
    {
        return values[(int)index];
    }
    
    public void SetValue(double index, object? value)
    {
        values[(int)index] = value;
    }

    public override string ToString()
    {
        var sb = new StringBuilder();
        sb.Append('[');
        for (var index = 0; index < values.Length; index++)
        {
            sb.Append(Interpreter.Stringify(values[index]));
            if (index < values.Length - 1) sb.Append(", ");
        }

        sb.Append(']');
        return sb.ToString();
    }

    public readonly struct ArrayGet(object?[] values, Token name) : ILoxCallable
    {
        public int Arity => 1;

        public object? Call(Interpreter interpreter, List<object?> arguments)
        {
            var index = Convert.ToInt32(arguments[0]);
            AssertIndexInRange(name, values, index);
            return values[index];
        }
    }
    
    public readonly struct ArraySet(object?[] values, Token name) : ILoxCallable
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