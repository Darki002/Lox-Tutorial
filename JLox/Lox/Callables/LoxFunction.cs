﻿using Lox.Errors;

namespace Lox.Callables;

public class LoxFunction(Stmt.Function declaration, Environment? closure) : ILoxCallable
{
    public int Arity => declaration.Params.Count;
    
    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var env = new Environment(closure);

        declaration.Params
            .Zip(arguments)
            .ToList()
            .ForEach(zip => env.Define(zip.Second));

        try
        {
            interpreter.ExecuteBlock(declaration.Body, env);
        }
        catch (Return e)
        {
            return e.Value;
        }
        
        return null;
    }

    public object? Bind(LoxInstance loxInstance)
    {
        var env = new Environment(closure);
        env.Define(loxInstance);
        return new LoxFunction(declaration, env);
    }
    
    public override string ToString()
    {
        return $"<fn {declaration.Name.Lexeme}>";
    }
}