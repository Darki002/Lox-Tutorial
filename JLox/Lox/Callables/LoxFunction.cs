namespace Lox.Callables;

public class LoxFunction(Stmt.Function declaration) : ILoxCallable
{
    public int Arity => declaration.Params.Count;
    
    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var env = new Environment(interpreter.Globals);

        declaration.Params
            .Zip(arguments)
            .ToList()
            .ForEach(zip => env.Define(zip.First.Lexeme, zip.Second));
        
        interpreter.ExecuteBlock(declaration.Body, env);
        return null;
    }

    public override string ToString()
    {
        return $"<fn {declaration.Name.Lexeme}>";
    }
}