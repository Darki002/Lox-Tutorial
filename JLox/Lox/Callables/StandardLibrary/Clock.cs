namespace Lox.Callables.StandardLibrary;

public class Clock : ILoxCallable
{
    public int Arity { get; } = 0;
    
    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        return DateTime.Now.Millisecond / 1000.0;
    }

    public override string ToString()
    {
        return "<native fn>";
    }
}