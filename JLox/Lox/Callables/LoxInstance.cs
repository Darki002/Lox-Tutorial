using Lox.Errors;

namespace Lox.Callables;

public class LoxInstance(LoxClass? loxClass)
{
    private readonly Dictionary<string, object?> fields = [];
    
    public object? Get(Token name)
    {
        if (fields.TryGetValue(name.Lexeme, out var value))
        {
            return value;
        }

        var method = loxClass?.FindMethod(name.Lexeme);
        if (method is not null) return method.Bind(this);

        throw new RuntimeError(name, $"Undefined property '{name.Lexeme}'.");
    }
    
    public void Set(Token name, object? value)
    {
        fields[name.Lexeme] = value;
    }
    
    public override string ToString()
    {
        return $"{loxClass?.Name} instance";
    }
}