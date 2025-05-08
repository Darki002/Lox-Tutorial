namespace Lox.Callables;

public class LoxClass(string name, Dictionary<string, LoxFunction> methods, Dictionary<string, LoxFunction> staticMethods) : ILoxCallable
{
    public string Name => name;

    public int Arity => GetArity();

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var instance = new LoxInstance(this);
        var initializer = FindMethod("init");
        if (initializer != null)
        {
            initializer.Bind(instance).Call(interpreter, arguments);
        }
        return instance;
    }
    
    public LoxFunction? FindMethod(string methodName)
    {
        return methods.GetValueOrDefault(methodName);
    }

    private int GetArity()
    {
        var initializer = FindMethod("init");
        return initializer?.Arity ?? 0;
    }
    
    public override string ToString()
    {
        return name;
    }

    public LoxFunction? GetStatic(Token methodName)
    {
        return staticMethods.GetValueOrDefault(methodName.Lexeme);
    }
}