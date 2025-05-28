namespace Lox;

public class Environment(Environment? enclosing = null)
{
    public readonly Environment? Enclosing = enclosing;
    
    private readonly List<object?> values = [];

    public void Define(object? value) => values.Add(value);
    
    public object? GetAt(int distance, int index)
    {
        return Ancestor(distance)?.values[index];
    }

    public void AssignAt(int distance, int index, object? value)
    {
        var ancestor = Ancestor(distance);
        ancestor!.values[index] = value;
    }
    
    private Environment? Ancestor(int distance)
    {
        var env = this;
        for (var i = 0; i < distance; i++)
        {
            env = env?.Enclosing;
        }

        return env;
    }
}