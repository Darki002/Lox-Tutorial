namespace Lox.Callables;

public class LoxClass(LoxClass? metaClass, string name, Dictionary<string, LoxFunction> methods) : LoxInstance(metaClass), ILoxCallable
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
        if (initializer == null) return 0;
        return initializer.Arity;
    }
    
    public override string ToString()
    {
        return name;
    }
}