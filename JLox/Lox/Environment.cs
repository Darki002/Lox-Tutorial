using Lox.Errors;

namespace Lox;

public class Environment(Environment? enclosing = null)
{
    private readonly Environment? enclosing = enclosing;
    
    private readonly Dictionary<string, object?> values = new();

    public void Define(string name, object? value)
    {
        if (!values.TryAdd(name, value))
        {
            values[name] = value;
        }
    }

    public object? Get(Token name)
    {
        if (values.TryGetValue(name.Lexeme, out var value))
        {
            return value;
        }

        if (enclosing is not null) return enclosing.Get(name);

        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }
    
    public object? GetAt(int distance, int index)
    {
        return Ancestor(distance)?.values.ElementAt(index);
    }

    public void Assign(Token name, object? value)
    {
        if (values.ContainsKey(name.Lexeme))
        {
            values[name.Lexeme] = value;
            return;
        }
        
        if (enclosing is not null)
        {
            enclosing.Assign(name, value);
            return;
        }
        
        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }

    public void AssignAt(int distance, Token name, object? value)
    {
        var ancestor = Ancestor(distance);
        ancestor!.values[name.Lexeme] = value;
    }
    
    private Environment? Ancestor(int distance)
    {
        var env = this;
        for (var i = 0; i < distance; i++)
        {
            env = env?.enclosing;
        }

        return env;
    }
}