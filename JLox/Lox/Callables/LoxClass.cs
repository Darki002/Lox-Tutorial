namespace Lox.Callables;

public class LoxClass(string name, Dictionary<string, object?> methods) : ILoxCallable
{
    public string Name => name;
    
    public int Arity => 0;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var instance = new LoxInstance(this);
        return instance;
    }
    
    public object? FindMethod(Token token)
    {
        return methods.GetValueOrDefault(token.Lexeme);
    }
    
    public override string ToString()
    {
        return name;
    }
}