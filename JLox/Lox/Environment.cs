using Lox.Errors;

namespace Lox;

public class Environment
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

        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }

    public void Assign(Token name, object? value)
    {
        if (values.ContainsKey(name.Lexeme))
        {
            values[name.Lexeme] = value;
        }
        else
        {
            throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
        }
    }
}