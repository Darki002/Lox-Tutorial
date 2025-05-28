namespace Lox.Callables.StandardLibrary;

public class ArrayCallable : ILoxCallable
{
    public int Arity { get; } = 1;
    
    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var size = Convert.ToInt32(arguments[0]);
        return new ArrayInstance(size);
    }
}