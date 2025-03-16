using System.Collections.Specialized;
using Lox.Errors;

namespace Lox;

public class Environment(Environment? enclosing = null)
{
    private readonly Environment? enclosing = enclosing;
    
    private readonly OrderedDictionary variables = new();

    public void Define(string name, object? value)
    {
        variables[name] = value;
    }

    public object? Get(Token name)
    {
        if (variables.Contains(name.Lexeme)) return variables[name.Lexeme];

        if (enclosing is not null) return enclosing.Get(name);

        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }
    
    public object? GetAt(int distance, int index)
    {
        return Ancestor(distance)?.variables[index];
    }

    public void Assign(Token name, object? value)
    {
        if (variables.Contains(name.Lexeme))
        {
            variables[name.Lexeme] = value;
            return;
        }
        
        if (enclosing is not null)
        {
            enclosing.Assign(name, value);
            return;
        }
        
        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }

    public void AssignAt(int distance, int index, object? value)
    {
        var ancestor = Ancestor(distance);
        ancestor!.variables[index] = value;
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