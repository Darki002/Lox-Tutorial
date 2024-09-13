using Lox.Errors;

namespace Lox;

public class Environment(Environment? enclosing = null)
{
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
}