using Lox.Errors;

namespace Lox.Callables;

public class LoxFunction(Stmt.Function declaration, Environment? closure, bool isInitializer) : ILoxCallable
{
    public int Arity => declaration.Params?.Count ?? 0;

    public bool IsGetter => declaration.Params is null;
    
    public object? Call(Interpreter interpreter, List<object?>? arguments = null)
    {
        var env = new Environment(closure);

        declaration.Params?
            .Zip(arguments!)
            .ToList()
            .ForEach(zip => env.Define(zip.Second));

        try
        {
            interpreter.ExecuteBlock(declaration.Body, env);
        }
        catch (Return e)
        {
            return isInitializer 
                ? closure!.GetAt(0, 0) 
                : e.Value;
        }

        return isInitializer ? closure!.GetAt(0, 0) : null;
    }

    public LoxFunction Bind(LoxInstance loxInstance)
    {
        var env = new Environment(closure);
        env.Define(loxInstance);
        return new LoxFunction(declaration, env, isInitializer);
    }
    
    public override string ToString()
    {
        return $"<fn {declaration.Name.Lexeme}>";
    }
}