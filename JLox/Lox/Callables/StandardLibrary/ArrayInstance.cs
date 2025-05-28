using Lox.Errors;

namespace Lox.Callables.StandardLibrary;

public class ArrayInstance(int size) : LoxInstance(null)
{
    private readonly object?[] values = new object?[size];
    
    public override object? Get(Token name)
    {
        return name.Lexeme switch
        {
            "get" => new ArrayGet(values),
            "set" => new ArraySet(values),
            "length" => values.Length,
            _ => throw new RuntimeError(name, $"Undefined property '{name.Lexeme}'.")
        };
    }
    
    private readonly struct ArrayGet(object?[] values) : ILoxCallable
    {
        public int Arity => 1;

        public object? Call(Interpreter interpreter, List<object?> arguments)
        {
            var index = Convert.ToInt32(arguments[0]);
            return values[index];
        }
    }
    
    private readonly struct ArraySet(object?[] values) : ILoxCallable
    {
        public int Arity => 2;

        public object? Call(Interpreter interpreter, List<object?> arguments)
        {
            var index = Convert.ToInt32(arguments[0]);
            values[index] = arguments[1];
            return null;
        }
    }
}