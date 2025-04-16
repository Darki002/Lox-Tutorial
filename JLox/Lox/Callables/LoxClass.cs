namespace Lox.Callables;

public class LoxClass(string name) : ILoxCallable
{
    public string Name => name;
    
    public int Arity => 0;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var instance = new LoxInstance(this);
        return instance;
    }
    
    public override string ToString()
    {
        return name;
    }
}