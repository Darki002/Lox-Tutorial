namespace Lox.Callables.StandardLibrary;

public class Clock : ILoxCallable
{
    public int Arity { get; } = 0;
    
    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        return (double)DateTime.Now.Ticks / TimeSpan.TicksPerMicrosecond;
    }

    public override string ToString()
    {
        return "<native fn>";
    }
}