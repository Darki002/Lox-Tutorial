namespace Lox.Callables.StandardLibrary;

public class Typeof : ILoxCallable
{
    public int Arity => 1;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var value = arguments[0];
        return value switch
        {
            double => "number",
            string => "string",
            ArrayInstance => "array",
            LoxClass klass => klass.Name,
            ILoxCallable => "callable",
            LoxInstance instance => instance.LoxClass?.Name,
            _ => null
        };
    }
}